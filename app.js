#!/usr/bin/env node

var Gpio = require('pigpio').Gpio;

var led = new Gpio(17, {mode: Gpio.OUTPUT});

console.log('rpi-blink');


var dutyCycle = 0;

setInterval(function () {
  led.pwmWrite(dutyCycle);

  dutyCycle += 5;
  if (dutyCycle > 255) {
    dutyCycle = 0;
  }
}, 20);
