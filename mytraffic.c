
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system_misc.h> /* cli(), *_flags */
#include <linux/uaccess.h>
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/timer.h> /* Used for Linux timer */
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");


#define RED_GPIO 67
#define YELLOW_GPIO 68
#define GREEN_GPIO 44
#define BTN0_GPIO 26
#define BTN1_GPIO 46

int mytraffic_major = 61;
int BTN0_irq_num = 0;
int BTN1_irq_num = 0;
int operational_mode = 0; // 0 = Normal mode, 1 = Flashing-Red mode, 2 = Flashing-Yellow mode
int frequency = 1; // Length of cycle = 1/user_input
int pedestrian_button = 0;

// Define waitqueue
wait_queue_head_t wq;
static struct task_struct* my_task; // Used for new thread
//init_waitqueue_head (&wq);
DECLARE_WAIT_QUEUE_HEAD(wq);

static int mytraffic_init(void);
int mytraffic_main(void* i);
static irqreturn_t btn0_irq_handler(int irq,void *dev_id);
static irqreturn_t btn1_irq_handler(int irq,void *dev_id);
static int mytraffic_open(struct inode *inode, struct file *filp);
static int mytraffic_release(struct inode *inode, struct file *filp);
static ssize_t mytraffic_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t mytraffic_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void normal_mode(void); // Might need to include frequency later
void flashing_red(void);
void flashing_yellow(void);
void turn_off_lights(void);
void turn_on_lights(void);

static int mytraffic_open(struct inode *inode, struct file *filp){
	return 0;
}


static int mytraffic_release(struct inode *inode, struct file *filp){
	return 0;
}


static ssize_t mytraffic_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
    char traffic_data[250];
    int data_len = 0;
    int red_state = gpio_get_value(RED_GPIO);
    int yellow_state = gpio_get_value(YELLOW_GPIO);
    int green_state = gpio_get_value(GREEN_GPIO);

    if (*f_pos > 0) {
        return 0;
    }

    data_len = snprintf(traffic_data, sizeof(traffic_data),
                        "Operational Mode: %d\n"
                        "Frequency: %d\n"
                        "Pedestrian Button: %d\n"
                        "Red Light: %s\n"
                        "Yellow Light: %s\n"
                        "Green Light: %s\n",
                        operational_mode, frequency, pedestrian_button,
                        red_state ? "On" : "Off",
                        yellow_state ? "On" : "Off",
                        green_state ? "On" : "Off");
    //Limit the 'bite' to 250 arbitrary
    if (data_len > count) {
        data_len = count;
    }

    if (copy_to_user(buf, traffic_data, data_len)) {
        return -EFAULT;
    }

    *f_pos += data_len;

    return data_len;
}


static ssize_t mytraffic_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
    int new_freq;
    char input_buff[10];
    //Set max Hz to 9
    if (count > 9)
        count = 9;

    if (copy_from_user(input_buff, buf, count)) {
        return -EFAULT;
    }

    input_buff[count] = '\0';

    //Error handling
    if (kstrtoint(input_buff, 10, &new_freq) < 0) {
        printk(KERN_ALERT "Invalid input for frequency\n");
        return -EINVAL;
    }

    if (new_freq < 1) {
        printk(KERN_ALERT "Frequency must be greater than 0\n");
        return -EINVAL;
    }

    frequency = new_freq;

    return count;
}


static irqreturn_t btn0_irq_handler(int irq,void *dev_id){
	static unsigned long flags = 0;
	local_irq_save(flags);
	
	// Increment status and jump into according mode
	switch(operational_mode){
		case 0:
			// Normal-Mode -> Flashing-Red
			operational_mode += 1;
			break;
		case 1:
			// Flashing-Red -> Flashing-Yellow
			operational_mode += 1;
			break;
		case 2:
			// Flashing-Yellow -> Normal Mode
			operational_mode = 0;
			break;
		case 3:
			operational_mode = 0;
			break;
	}
	local_irq_restore(flags);
	return IRQ_HANDLED;
}

static irqreturn_t btn1_irq_handler(int irq,void *dev_id){
	static unsigned long flags = 0;
	local_irq_save(flags);
	if(operational_mode == 0){
		pedestrian_button = 1;
	}
	local_irq_restore(flags);
	return IRQ_HANDLED;
}

/*
 *
 *	NOT USED YET, maybe for 5 points extra
 *
 *
static irqreturn_t btn2_irq_handler(int irq,void *dev_id){
	static unsigned long flags = 0;
	local_irq_save(flags);
	
	if(gpio_get_value(BTN0_GPIO) == 1 || gpio_get_value(BTN1_GPIO) == 1){
		// Both are up!
		turn_on_lights();
		operational_mode = 3; // new mode to enter from 	
	}

	local_irq_restore(flags);
	return IRQ_HANDLED;
}
*/

/* Structure that declares the usual file */
/* access functions */
struct file_operations mytraffic_fops = {
        read: mytraffic_read,   // Used for -l
        write: mytraffic_write, // Used for creating timers and setting -m
        open: mytraffic_open,
        release: mytraffic_release
};


static int  mytraffic_init(void){
	
	int result;
	result = register_chrdev(mytraffic_major, "mytraffic", &mytraffic_fops);
	if (result < 0)
        {
                printk(KERN_ALERT "mytraffic: cannot obtain major number %d\n", mytraffic_major);
                return result;
        }
	
	// Obtain and initialize GPIO for lights
	int red = gpio_request(RED_GPIO, "red_light");
	int yellow = gpio_request(YELLOW_GPIO, "yellow_light");
	int green = gpio_request(GREEN_GPIO, "green_light");
	int btn0 = gpio_request(BTN0_GPIO, "btn0");
	int btn1 = gpio_request(BTN1_GPIO, "btn1");

	if(red != 0 || yellow != 0 || green != 0  || btn0 != 0 || btn1 != 0){
		printk(KERN_INFO "Error gpio request\n");
		return 1;
	}
	
	gpio_direction_input(BTN0_GPIO);	
	gpio_direction_input(BTN1_GPIO);	

	if(gpio_direction_output(RED_GPIO, 0) != 0 || gpio_direction_output(YELLOW_GPIO, 0) != 0 || gpio_direction_output(GREEN_GPIO, 0) != 0){
		printk(KERN_INFO "Error: Setting GPIO direction failed!\n");
		return 1;
	}
	
	// Set debouncing 
	if(gpio_set_debounce(BTN0_GPIO,200) < 0 || gpio_set_debounce(BTN1_GPIO,200) < 0){
		printk(KERN_INFO "Error: Can't set GPIO debounce\n");
	}
	
	// Get IRQ numbers
	BTN0_irq_num = gpio_to_irq(BTN0_GPIO);
	BTN1_irq_num = gpio_to_irq(BTN1_GPIO);

	int irq0 = request_irq(BTN0_irq_num, (void *) btn0_irq_handler, IRQF_TRIGGER_FALLING , "btn0", NULL);
	int irq1 = request_irq(BTN1_irq_num, (void *) btn1_irq_handler, IRQF_TRIGGER_FALLING , "btn1", NULL);

	// TEST IDEA add request IRQ for 
	//int irq2 = request_irq(BTN0_irq_num, (void *) btn2_irq_handler, IRQF_TRIGGER_HIGH , "btn0", NULL);
	//int irq3 = request_irq(BTN1_irq_num, (void *) btn2_irq_handler, IRQF_TRIGGER_HIGH , "btn1", NULL);

	if(irq0 + irq1 != 0){
		printk(KERN_INFO "Error: Requesting IRQ failed!\n");
		return 1;
	}
	
	my_task = kthread_create(mytraffic_main, NULL, "myLights");
	if (my_task != NULL) {
	    wake_up_process(my_task);
	} else {
	    return -1;
	}
	

	return 0;
}


int mytraffic_main(void* i){
	// What ought to be a main function
	while (!kthread_should_stop()) {
		switch(operational_mode){
			case (0):
				normal_mode();		
				break;
			case (1):
				flashing_red();
				break;
			case (2):
				flashing_yellow();
				break;
		}
	}
	// Apparently it doesnt recognize kthread_exit
	//kthread_exit(); 
	return 0;
}

void flashing_red(void){
	// Continuous flashing of red light, one cycle on one cycle off
	turn_off_lights();

	while(operational_mode == 1){
		gpio_set_value(RED_GPIO,1);
		wait_event_interruptible_timeout(wq, 1==0, 1 * (HZ / frequency));
		gpio_set_value(RED_GPIO,0);
		wait_event_interruptible_timeout(wq, 1==0, 1 * (HZ / frequency));
	}

}

void flashing_yellow(void){
	// Continuously flash yellow, one cycle on one cycle off
	turn_off_lights();

	while(operational_mode == 2){
		gpio_set_value(YELLOW_GPIO,1);
		wait_event_interruptible_timeout(wq, 1==0, 1 * (HZ / frequency));
		gpio_set_value(YELLOW_GPIO,0);
		wait_event_interruptible_timeout(wq, 1==0, 1 * (HZ / frequency));
	}
}

void turn_off_lights(void){
	gpio_set_value(RED_GPIO,0);
	gpio_set_value(YELLOW_GPIO,0);
	gpio_set_value(GREEN_GPIO,0);
}

void turn_on_lights(void){
	gpio_set_value(RED_GPIO,1);
	gpio_set_value(YELLOW_GPIO,1);
	gpio_set_value(GREEN_GPIO,1);
}

void normal_mode(void){
	// Green for 3 cycles, yellow for one cycle, red for 2 cycles
	turn_off_lights();

	while(operational_mode == 0){
		if(pedestrian_button == 0){		
			gpio_set_value(GREEN_GPIO,1); // Green runs for 3 cycles
			wait_event_interruptible_timeout(wq, 1==0, 3 * (HZ / frequency));
			gpio_set_value(GREEN_GPIO,0); // Turn off green
		
		
			gpio_set_value(YELLOW_GPIO,1); // Yellow runs for 1 cyclee
			wait_event_interruptible_timeout(wq, 1==0, 1 * (HZ / frequency));
			gpio_set_value(YELLOW_GPIO,0); // Turn off yellow


			gpio_set_value(RED_GPIO,1); // Red runs for 2 cyclee
			wait_event_interruptible_timeout(wq, 1==0, 2 * (HZ / frequency));
			gpio_set_value(RED_GPIO,0); // Turn off red
		}else if(pedestrian_button == 1){
			turn_off_lights();
			gpio_set_value(GREEN_GPIO,1); // Green runs for 3 cycles
			wait_event_interruptible_timeout(wq, 1==0, 3 * (HZ / frequency));
			gpio_set_value(GREEN_GPIO,0); // Turn off green
		
		
			gpio_set_value(YELLOW_GPIO,1); // Yellow runs for 1 cyclee
			gpio_set_value(RED_GPIO,1); // Red runs for 2 cyclee
			wait_event_interruptible_timeout(wq, 1==0, 5 * (HZ / frequency));
			gpio_set_value(YELLOW_GPIO,0); // Turn off yellow
			gpio_set_value(RED_GPIO,0); // Turn off red

			pedestrian_button = 0;
		}
	}	
}


void mytraffic_exit(void){
	
	// Free GPIO
	gpio_free(RED_GPIO);	
	gpio_free(YELLOW_GPIO);	
	gpio_free(GREEN_GPIO);	
	gpio_free(BTN0_GPIO);
	gpio_free(BTN1_GPIO);
	// TRY
	//kthread_exit(my_task); 
	kthread_stop(my_task);
        unregister_chrdev(mytraffic_major, "mytraffic");
}

module_init(mytraffic_init);
module_exit(mytraffic_exit);


