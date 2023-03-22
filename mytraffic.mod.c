#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa16cf51b, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x9fd05d89, "kthread_stop" },
	{ 0xfe990052, "gpio_free" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x59d97a38, "finish_wait" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0xf63f0aeb, "prepare_to_wait_event" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xb8f3845c, "gpiod_set_raw_value" },
	{ 0x7c32d0f0, "printk" },
	{ 0x5d3e9144, "wake_up_process" },
	{ 0x4ccccd0, "kthread_create_on_node" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x5871f40, "gpiod_to_irq" },
	{ 0xdea88963, "gpiod_set_debounce" },
	{ 0xd260b89d, "gpiod_direction_output_raw" },
	{ 0xa89bfca5, "gpiod_direction_input" },
	{ 0x28eca8fe, "gpio_to_desc" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x1f6e82a4, "__register_chrdev" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

