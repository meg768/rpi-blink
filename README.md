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
