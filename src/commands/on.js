var Gpio = require('pigpio').Gpio;

var Module = new function() {

	function defineArgs(args) {

		args.option('pin', {alias: 'p', describe:'Pin number'});

		args.wrap(null);

	}

	function run(argv) {

		try {
			console.log('dfg');
			var led = new Gpio(argv.pin, {mode: Gpio.OUTPUT});
			led.digitalWrite(1);

		}
		catch(error) {
			console.log(error.message);
		}
	}

	module.exports.command  = 'on <pin>';
	module.exports.describe = 'Turn on the specified pin';
	module.exports.builder  = defineArgs;
	module.exports.handler  = run;



};
