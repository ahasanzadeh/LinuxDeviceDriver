# Linux Device Driver

The purpose of this repository is to practice linux device driver from scratch, and test each code before upload here.

## Description

The following topics will be practised here:
* Character driver
* 

## Getting Started

### Dependencies

* Host can be any OS like Fedora 40 Linux in this case or Ubuntu, Windows etc.  
* A linux OS like Ubuntu 24 in this case is installed on a VirtualBox installed on the host.

### Installing

* VirtialBox 7.0.18 is installed on the host.
* Ubuntu 24 is installed on the VirtualBox.

### Executing program

#### Character driver

* On Driver folder, run "make" command in a terminal while you are in root.
* run "insmod chr_device_drv.ko".
* run "lsmod | head -5" or "cat /proc/devices" to see the driver installed driver.
* On Application folder, run "cc app_chr_drv.c -o app_chr_drv".
* Run "./app_chr_drv" command in a terminal while you are in root. 
* You can see 3 options: option 1 allows you to write, and option 2 allows you to read whatever you wrote in option via 1.


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