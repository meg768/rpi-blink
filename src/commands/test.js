
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';



var random = require('yow').random;
var NeopixelStrip = require('../js/neopixel-strip.js');


module.exports.builder = function(args) {

	args.option('command', {alias: 'c', describe:'Command',  default:'none', choices: ['set', 'wipe', 'fade', 'demo', 'none', 'init']});
	args.option('red', {alias: 'r', describe:'red', default: 64});
	args.option('green', {alias: 'g', describe:'green', default: 0});
	args.option('blue', {alias: 'b', describe:'blue', default: 0});
	args.option('pause', {alias: 'p', describe:'Pause', default: 100});
	args.option('length', {alias: 'l', describe:'Length', default: undefined});

	args.wrap(null);


};

module.exports.handler = function(args) {

	try {
		var _index = 0;
		var _strip = new NeopixelStrip();

		function loop() {
			return new Promise(function(resolve, reject) {
				var red   = random([0, 128]);
				var green = random([0, 128]);
				var blue  = random([0, 128]);

				_strip.setColor(red, green, blue, 0, 32).then(function() {
					_index = (_index + 1) % 4;
					setTimeout(loop, 100);
					resolve();
				})
				.catch(function(error) {
					reject(error);
				});
			});
		}


		var promise = Promise.resolve();

		promise = promise.then(function() {
			return loop();
		});

		promise.then(function(result) {
			console.log('OK');
		})
		.catch(function(error) {
			console.log(error);
		});


	}
	catch(error) {
		console.log(error.stack);
	}

};
