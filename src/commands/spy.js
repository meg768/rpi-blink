
module.exports.command  = 'spy';
module.exports.describe = 'Spy Blink';





module.exports.builder = function(args) {

/*
	args.option('command', {alias: 'c', describe:'Command',  default:'none', choices: ['set', 'wipe', 'fade', 'demo', 'none', 'init']});
	args.option('red', {alias: 'r', describe:'red', default: 64});
	args.option('green', {alias: 'g', describe:'green', default: 0});
	args.option('blue', {alias: 'b', describe:'blue', default: 0});
	args.option('pause', {alias: 'p', describe:'Pause', default: 100});
	args.option('length', {alias: 'l', describe:'Length', default: undefined});
*/
	args.wrap(null);


};

module.exports.handler = function(args) {

	try {
		var NeopixelStrip = require('../js/neopixel-strip.js');

		var _index = 0;
		var _strip = new NeopixelStrip();

		function pause(ms) {

			return new Promise(function(resolve, reject) {
				setTimeout(resolve, ms);
			});
		}




		function loop() {
			return new Promise(function(resolve, reject) {
				Promise.resolve().then(function() {
					loop();
				})
				.catch(function(error) {
					reject(error);
				});
			});
		}

		functiuon run() {

			var promise = Promise.resolve();


			promise.then(function(result) {
				console.log('OK');
			})
			.catch(function(error) {
				console.log(error);
			});

		}

		run();


	}
	catch(error) {
		console.log(error.stack);
	}

};
