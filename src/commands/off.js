
var Module = new function() {

	function defineArgs(args) {

		args.wrap(null);

		args.check(function(argv) {
			if (argv.pin <= 0 || argv.pin == undefined)
				throw new Error('Invalid pin number');

			return true;
		});

	}

	function run(args) {

		try {
			var Gpio = require('pigpio').Gpio;
			var led = new Gpio(args.pin, {mode: Gpio.OUTPUT});
			led.digitalWrite(0);

		}
		catch(error) {
			console.log(error.message);
		}
	}

	module.exports.command  = 'off <pin>';
	module.exports.describe = 'Turn off the specified pin';
	module.exports.builder  = defineArgs;
	module.exports.handler  = run;



};
