# Linux Device Driver

The purpose of this repository is to practice linux device driver from scratch, and test each code before upload here.

## Description

The following topics will be practised here:
* Character driver
* IOCTL driver
* PROCFS driver
* Interrupt driver - top half
* Static tasklet driver - bottom half
* Dynamic tasklet driver - bottom half
* Kthread create
* Kthread run
* Kthread seeking spinlock
* Tasklet with kthread seeking spinlock which handled by IRQ - bottom half
* I/O port
* I/O region
* I/O port memory map
* Setup timer jiffies
* Busy wait in timer jiffies
* Schedule timer jiffies
* Schedule timeout in timer jiffies
* Wait queue event in timer jiffies
* Sysfs
* Mutex
* Workqueue in static method
* Workqueue in dynamic method
* Own workqueue
* I2C
* USB
* GPIO platform driver

## Getting Started

### Dependencies

* Host can be any OS like Fedora 40 Linux in this case or Ubuntu, Windows etc.  
* A linux OS like Ubuntu 24 in this case is installed on a VirtualBox installed on the host (Kernel release version: 6.8.0-35-generic; GCC version: 13.2.0 GNU nake version: 4.3).

### Installing

* VirtialBox 7.0.18 is installed on the host.
* Ubuntu 24 is installed on the VirtualBox.
* Install following in Ubuntu (in case, when running "make" command, and it does not recognize the kernel,reinstall the kast 4 ones below):
    * sudo apt install bzip2 tar gcc make perl git
    * sudo apt install linux-header-generic
    * sudo apt install linux-header-$(uname -r) 
    * sudo apt-get install build-essential 
    * sudo apt-get install dkms 

### Executing program

#### Character driver

* On Driver folder, run "make" command in a terminal while you are in root.
* run "insmod chr_device_drv.ko".
* run "lsmod | head -5" or "cat /proc/devices" to see the driver was installed. "dmesg" also shows you a report ("dmesg -c" clears previous log).
* On Application folder, run "cc app_chr_drv.c -o app_chr_drv" in the terminal.
* Run "./app_chr_drv" command in a terminal while you are in root. 
* You can see 3 options: option 1 allows you to write, and option 2 allows you to read whatever you wrote in option 1.

#### IOCTL driver
* Follow corresponding order for "Character driver" above.

#### PROCFS driver
* Follow corresponding order for "Character driver" above. "cat /proc/chr_proc" shows you what have been inserted in terminal in Application side. "chr_proc" was defined in application side.

#### Interrupt driver - top half
* Follow corresponding order for "Character driver" above. "cat /proc/interrupts" shows you that interrupt 1 was incrementing. No need for application side for this practice.Run "dmesg" to see log.

#### Static tasklet driver - bottom half
* Follow corresponding order for "Interrupt driver - top half" above. 

#### Dynamic tasklet driver - bottom half
* Follow corresponding order for "Interrupt driver - top half" above. 

#### Kthread create

* Create kthread and wake up the process. Follow corresponding order for "Interrupt driver - top half" above. 

#### Kthread run

* Run kthread performs create kthread and wake up process. Follow corresponding order for "Interrupt driver - top half" above. 

#### Kthread seeking spinlock

* Follow corresponding order for "Interrupt driver - top half" above. 

#### Tasklet with kthread seeking spinlock which handled by IRQ - bottom half

* Follow corresponding order for "Interrupt driver - top half" above. 

#### I/O port

* Access I/O port of memory; however, it cannot be seen using "cat /proc/ioports" since region has not been asssigned. Follow corresponding order for "Character driver" above. 

#### I/O region

* Access I/O region of memory. Now, I/O port can be seen using "cat /proc/ioports". Follow corresponding order for "Character driver" above.

#### I/O port memory map

* I/O port can be seen using "cat /proc/ioports". Follow corresponding order for "Character driver" above. 

#### Setup timer jiffies

* Follow corresponding order for "Character driver" above. 

#### Busy wait in timer jiffies

* Relax CPU. Follow corresponding order for "Character driver" above.

#### Schedule timer jiffies

* Use scheduling instead of relax CPU. cheduleFollow corresponding order for "Character driver" above.

#### Schedule timeout in timer jiffies

* Follow corresponding order for "Character driver" above.

#### Wait queue event in timer jiffies

* Follow corresponding order for "Character driver" above.

#### Sysfs

* After installing *.ko driver using insmod command, using command "echo 123 > /sys/kernel/my_sysfs/sysfs_val", 123 will be stored, and using command "cat /sys/kernel/my_sysfs/sysfs_val", 123 will be shown back. Also, my_sysfs can be observed using "ls -l /sys/kernel/", and more details can be observed using "ls -l /sys/kernel/my_sysfs/sysfs_val".

#### Mutex

* Follow corresponding order for "Character driver" above.

#### Workqueue in static method

* After installing *.ko driver using insmod command, using command "cat/dev/my_device", and later in the log observed using command "dmesg": "Device file opened...", "Read function...", and "Device file closed...".

#### Workqueue in dynamic method

* Follow corresponding order for "Workqueue in static method" above.

#### Own workqueue

* Follow corresponding order for "Workqueue in static method" above.

#### I2C

* Nedd BBB hardware to implement.

#### USB

* Follow corresponding order for "Workqueue in static method" above. After installing the driver using insmod command, it can be probed by "lsmod | head -5".

#### GPIO platform driver

* Nedd BBB hardware to implement.

## Help

TBD

## Authors

Contributors names and contact info

TBD

## Version History

* 0.1
    * Initial Release

## License

This project is licensed under the FREE License - see the LICENSE.md file for details

## Acknowledgments

Inspiration, code snippets, etc.
* [ah](https://github.com/ahasanzadeh/)