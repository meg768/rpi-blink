
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
		var _strip = new NeopixelStrip();

		function loop(index) {

			if (index == undefined)
				index = 0;

			return new Promise(function(resolve, reject) {

				var promise = Promise.resolve();

				Promise.resolve().then(function() {
					var red   = random([0, 128]);
					var green = random([0, 128]);
					var blue  = random([0, 128]);

					console.log('Setting segment', index, 'to color', [red, green, blue]);

					return _strip.setColor(red, green, blue, index * 8 + 1, 6);
				})

				.then(function() {
					return _strip.pause(100);
				})

				.then(function() {
					index++;

					if (index >= 4)
						index = 0;

					return loop(index);
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
