#!/usr/bin/env node

var Gpio = require('pigpio').Gpio;

var motor = new Gpio(10, {mode: Gpio.OUTPUT});

console.log('rpi-blink');
