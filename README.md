# EC535
EC535 Lab

### Running the Game

## Compile EmbedmonServer and EmbedmonClient2
Add the EmbnedmonServer and EmbedmonClient2 to your workspace. 
Run qmake and make to generate executables EmbedmonServer and EmbedmonClient2. 
Copy the binaries to your $WORKSPACE/rootfs/root folder. 
## Set a static iPv4 address for Client and Server instances of the game. 
Before flashing the SD Card - modify the 'instances' file in $WORKSPACE/rootfs/etc/networking in the following way:

#networking file for SERVER BBB
#change address to 192.168.1.3 for CLIENT BBB

auto eth0  
iface eth0 inet dhcp  
  pre-up /etc/network/nfs_check  
  wait-delay 15  
  hostname $(hostname)  
  
iface eth0 inet static  
  address 192.168.1.2    
  netmask 255.255.255.0  

For EmbedmonServer: set the ip to 192.168.1.2
Then rebuild your rootfs and bootfs, create sdcard.img and flash the microSD.
For EmbedmonClient2: set the ip to 192.168.1.3
Then rebuild your rootfs and bootfs, create sdcard.img and flash the microSD.
Keep track of which microSD has the server instance configuration, and which one has the client instance configuration.

## Boot up the BBBs
Connect and ethernet cable between the Beaglebone Blacks before connecting to power. Connect the serial connectors as in Lab4,
one for each Beaglebone in a separate COM port (keep track of which COM port will be used for EmbedmonServer, and which one for
EmbedmonClient2).
Then, insert the server-configured MicroSD to the intended Server BBB, and the client-configured MicroSD to the intended Client BBB. 
Boot up the game by keeping the S2 button pressed (the one closest to the MicroSD card on the board) for a few seconds before connecting
the power, and for a few seconds after the screen lights up (a thin object like a pencil helps with this part). 

## Start the game
First, start EmbedmonServer by passing the argument in your serial screen: ./EmbedmonServer
Then, start EmbedmonClient2 by passing the argument in your serial screen: ./EmbedmonClient2

## Play the game!
Instructions TBD
