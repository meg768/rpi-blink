
module.exports.command  = 'on <pin>';
module.exports.describe = 'Turn on the specified pin';


module.exports.builder = function(args) {

	args.wrap(null);

	args.check(function(argv) {
		if (argv.pin <= 0 || argv.pin == undefined)
			throw new Error('Invalid pin number');

		return true;
	});

};


module.exports.handler = function(args) {
	try {
		var Gpio = require('pigpio').Gpio;
		console.log('Turning on!');
		var led = new Gpio(args.pin, {mode: Gpio.OUTPUT});
		led.digitalWrite(1);

	}
	catch(error) {
		console.log(error.message);
	}

};
