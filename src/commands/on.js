
module.exports.command  = 'on';
module.exports.describe = 'Turn on the specified pin';


module.exports.builder = function(args) {

	args.option('pin', {alias: 'p', describe:'Pin number'});

	args.wrap(null);

	args.check(function(argv) {
		if (argv.pin <= 0 || argv.pin == undefined)
			throw new Error('Invalid pin number');

		return true;
	});

};


module.exports.handler = new function() {
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
