LEGO MINDSTORMS EV3 source code - Extended version 
===============================

## What this is 
This is the master branch of the development tree used by Robomatter and National Instruments to create the extended firmware.

Additional features include:
* opcodes to turn off auto-id
* opcodes to force a connection type for the sensors
* opcodes to allow additional 3rd party VMs to co-exist with the LMS2012 VM
* Greatly reduced shutdown times
* Fixes for the I2C subsystem
* non-blocking opcodes to access the I2C devices, through additional ioctls

Please note that this does NOT include the sources for the ROBOTC and LabVIEW VMs.  These will be released by their respecive companies on other repositories.  Links will be provided to them here, once this is done.

See [the release announcement on BotBench][1] for more information.

## Credits
* Xander Soldaat, botbench.com/Robomatter
* James Levitt, National Instruments

## Things you will need

* A Linux (virtual) installation.  I used Ubuntu 14.04 on VMware Workstation, but I am sure other combinations also work.   
The reason for the Linux installation requirement is due to the case-sensitive filenames used by the Linux kernel source repository. Attempting to clone the source repository to a non-case sensitive file system such as Mac OSX HFS+ or Windows FAT would result in a corrupted repository.
* Eclipse.  I used Helios Service Release 1, but I know more recent versions will also work.
* Java JRE (for Eclipse) - it seems to work fine with the Open Source JRE in the openjdk-7-jre package.
* Code Sourcery Lite for ARM version 2009q1-203.  You can [download here directly][4].
* The mkimage program from u-boot-tools package to compile kernel.
* The convert program from imagemagick package.
* A USB to serial port dongle.  You need to splice an NXT cable and hook up dig0 (pin 5) and dig1 (pin 6) to TX and RX, not 100% which way around.  GND is pin 3.  The brick’s console is on port 1 and has a baud rate of 115200 8N1.  I have a pre-made one with an NXT socket, it’s not actually as fancy as it sounds. A guide can be found [here][5].
* An SD card to put your custom firmware on. It doesn’t use up a lot of space, but I’d stick with a simple 2GB one
* A pair of flat-nosed pliers, for removing the SD card, or stick a tab to it, like in [this guide][7]
* A Netgear WNA1100 WiFi dongle.  It is currently the only WiFi dongle that is supported by the EV3’s firmware.

## Getting Started

Use Git to clone this repo:

    git clone https://github.com/mindboards/ev3sources-xtended.git

The scripts in the EV3 sources expect the source code to live in a projects folder in your home directory. You will need to create a  projects symlink to the ev3sources folder that was created when you cloned the Git repo.

To import this project in Eclipse, check out the [Wiki article][6]

## Contributing

To make changes to the source code, click on the **Fork** button at the top of this page. This will create a copy of this repository under your own GitHub account. You can make changes to this repository as you wish. [See this page for more information about Forking.][2]

If you want to make a change to this shared repo, submit a **Pull Request**, which people can discuss and decide whether to apply your changes or not. [See this page for more information about Pull Requests.][3].

  [1]: http://botbench.com/blog/2014/05/05/ev3-extended-firmware-vm-now-on-github/
  [2]: https://help.github.com/articles/fork-a-repo
  [3]: https://help.github.com/articles/using-pull-requests
  [4]: http://go.mentor.com/2ig4q
  [5]: http://botbench.com/blog/2013/08/15/ev3-creating-console-cable/
  [6]: https://github.com/mindboards/ev3sources/wiki/Eclipse-import
  [7]: http://botbench.com/blog/2013/10/29/ev3-adding-a-pull-tab-to-your-micro-sd-card/
