# rpi-blink

## Image
2017-04-10-raspbian-jessie-lite.img

## Update
	$ sudo apt-get update
	$ sudo apt-get upgrade

## Install nodejs
	$ curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -
	$ sudo apt-get install -y nodejs

## Install git
	$ sudo apt-get install git

## Install pigpio C-library (if not already installed)
	$ wget abyz.co.uk/rpi/pigpio/pigpio.zip
	$ unzip pigpio.zip
	$ cd PIGPIO
	$ make
	$ sudo make install
	$ cd ..

## Clone project
	$ git clone https://github.com/meg768/rpi-blink.git

## Install pigpio (node module)
	$ cd rpi-blink
	$ npm install pigpio --save


## Links
http://www.sweclockers.com/forum/trad/1461722-ssh-till-raspberry-pi-fran-macbook-connection-refused
https://nodejs.org/en/download/package-manager/#debian-and-ubuntu-based-linux-distributions
https://github.com/fivdi/pigpio#installation
https://www.raspberrypi.org/documentation/configuration/wireless/wireless-cli.md
https://www.npmjs.com/package/i2c

https://pinout.xyz/pinout/pin2_5v_power
https://www.raspberrypi.org/forums/viewtopic.php?f=63&t=102243
https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c
http://www.instructables.com/id/Use-a-1-ATTiny-to-drive-addressable-RGB-LEDs/
