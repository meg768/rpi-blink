
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


		function pause(ms) {

			return new Promise(function(resolve, reject) {
				setTimeout(resolve, ms);
			});
		}


		function setColor(bar, red, green, blue, wait) {

			if (wait == undefined)
				wait = 300;

			return new Promise(function(resolve, reject) {
				bar.setColor(red, green, blue, 200).then(function() {
					return pause(wait);
				})
				.then(function() {
					resolve();
				})
				.catch(function(error) {
					reject(error);
				});
			});
		}

		function setNewColor() {
			return new Promise(function(resolve, reject) {
				var red   = random([0, 128]);
				var green = random([0, 128]);
				var blue  = random([0, 128]);

				setColor(bars[_index % 4], red, green, blue, 100).then(function() {
					_index++;
					resolve();
				})
				.catch(function(error) {
					reject(error);
				});
			});

		}

		function loop() {
			return new Promise(function(resolve, reject) {
				setNewColor().then(function() {
					loop();
				})
				.catch(function(error) {
					reject(error);
				});
			});
		}

		var strip = new NeopixelStrip({device:'/dev/i2c-1'});

		var bars = [];
		var bar1  = new strip.segment(0, 8); //NeopixelSegment({strip:strip, offset:0, length:8});
		var bar2  = new strip.segment(8, 8); //NeopixelSegment({strip:strip, offset:8, length:8});
		var bar3  = new strip.segment(16, 8); //NeopixelSegment({strip:strip, offset:8, length:8});
		var bar4  = new strip.segment(24, 8); //NeopixelSegment({strip:strip, offset:8, length:8});

		var bars = [bar1, bar2, bar3, bar4];

//		bar1 = bar2 = bar3 = bar4 = strip;

		var promise = Promise.resolve();

		if (args.command == 'init') {
			promise = promise.then(function() {
				return strip.initialize(40);
			});
		}
		else if (args.command == 'set') {
			promise = promise.then(function() {
				return strip.initialize(40);
			});
			promise = promise.then(function() {
				return strip.setColor(args.red, args.green, args.blue);
			});

		}
		else {
/*
				promise = promise.then(function() {
					return strip.initialize(32);
				})

				promise = promise.then(function() {
					return strip.fadeToColor(0, 0, 0);
				})
*/
				promise = promise.then(function() {

					return loop();
				})
		}

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
