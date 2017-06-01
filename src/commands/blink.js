
module.exports.command  = 'blink <pin>';
module.exports.describe = 'Blink on the specified pin';


module.exports.builder = function(args) {

	args.option('delay', {alias: 'd', describe:'Delay', default: 100});

	args.wrap(null);

	args.check(function(argv) {
		if (argv.pin <= 0 || argv.pin == undefined)
			throw new Error('Invalid pin number');

		return true;
	});

};

function delay(ms) {
	return new Promise(function(resolve, reject) {
		setTimeout(resolve, ms);
	});
}

module.exports.handler = function(args) {
	try {
		var Gpio = require('pigpio').Gpio;

		var led = new Gpio(args.pin, {mode: Gpio.OUTPUT});

		led.digitalWrite(1);

		delay(1000).then(function() {
			led.digitalWrite(0);
			return delay(args.delay);
		})
		.then(function() {
			led.digitalWrite(1);
			return delay(args.delay);
		})
		.then(function() {
			led.digitalWrite(0);
			return Promise.resolve();

		})

	}
	catch(error) {
		console.log(error.message);
	}

};
