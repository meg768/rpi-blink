
module.exports.command  = 'pwm <pin>';
module.exports.describe = 'PWM on the specified pin';


module.exports.builder = function(args) {


	args.wrap(null);


};


module.exports.handler = function(args) {
	try {
		var Gpio = require('pigpio').Gpio;
		var led = new Gpio(args.pin, {mode: Gpio.OUTPUT});
		var dutyCycle = 0;

		setInterval(function () {
		  led.pwmWrite(dutyCycle);

		  dutyCycle += 1;
		  if (dutyCycle > 255) {
		    dutyCycle = 0;
		  }
	  }, 5);


	}
	catch(error) {
		console.log(error.stack);
	}

};
