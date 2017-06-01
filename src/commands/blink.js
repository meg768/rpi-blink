
module.exports.command  = 'blink <pin>';
module.exports.describe = 'Blink on the specified pin';


module.exports.builder = function(args) {

	args.option('delay', {alias: 'd', describe:'Delay', default: 500});
	args.option('iterations', {alias: 'i', describe:'Iterations', default: 3});

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
		var Gpio    = require('pigpio').Gpio;
		var led     = new Gpio(args.pin, {mode: Gpio.OUTPUT});
		var promise = Promise.resolve();
		var mode    = false;

		function onoff(ms) {
			return new Promise(function(resolve, reject) {
				console.log('writing', mode)
				led.digitalWrite(0);

				delay(ms).then(function() {
					led.digitalWrite(1);
				})
				.then(function() {
					led.digitalWrite(0);

				})
				.then(resolve);
			});

		}

		console.log(args.iterations);

		for (var i = 0; i < args.iterations; i++) {
			promise = promise.then(function() {
				console.log(i);
				return onoff(args.delay);
			});
		}

		promise.then(function() {
			console.log('Finished.');
		})
		.catch(function(error) {
			console.error(error.stack);

		});

	}
	catch(error) {
		console.log(error.stack);
	}

};
