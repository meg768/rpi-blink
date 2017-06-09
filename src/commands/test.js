
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


function NeopixelStrip(options) {

	var _this = this;
	var _wire = undefined;

	_this.pause = function(ms) {

		return new Promise(function(resolve, reject) {
			setTimeout(resolve, ms);
		});
	}

	_this.setColor = function(red, green, blue) {
		red   = parseInt(red);
		green = parseInt(green);
		blue  = parseInt(blue);

		console.log('Setting color to', [red, green, blue]);

		return _this.write(0x10, [red, green, blue]);
	}

	_this.fadeToColor = function(red, green, blue, steps) {

		return _this.setColor(red, green, blue);
		console.log('Fading to color', [red, green, blue]);

//return _this.setColor(red, )
		if (steps == undefined)
			steps = 128;

		red    = parseInt(red);
		green  = parseInt(green);
		blue   = parseInt(blue);
		steps  = parseInt(steps);

		return _this.write(0x13, [red, green, blue, steps]);
	}

	_this.colorWipe = function(red, green, blue, delay) {
		red   = parseInt(red);
		green = parseInt(green);
		blue  = parseInt(blue);

		if (delay == undefined)
			delay = 100;

		return _this.write(0x11, [red, green, blue, delay]);
	}

	_this.setStripLength = function(length) {
		return _this.write(0x12, [parseInt(length)]);
	}

	_this.write = function(command, params) {
		return new Promise(function(resolve, reject) {
			_wire.writeBytes(parseInt(command), params, function(error) {
				if (error)
					reject(error);
				else
					resolve();
			});


		});

	}

	function init() {
		var I2C = require('i2c');

		if (options.address == undefined)
			options.address = 0x26;

		if (options.device == undefined)
			options.device = '/dev/i2c-1';

		_wire = new I2C(options.address, {device: options.device});


	}

	init();

};


module.exports.builder = function(args) {

	args.option('command', {alias: 'c', describe:'Command',  default:'color', choices: ['color', 'wipe']});
	args.option('red', {alias: 'r', describe:'red', default: 255});
	args.option('green', {alias: 'g', describe:'green', default: 255});
	args.option('blue', {alias: 'b', describe:'blue', default: 255});
	args.option('wait', {alias: 'w', describe:'Wait', default: 100});
	args.option('length', {alias: 'l', describe:'Length', default: 8});

	args.wrap(null);


};

function read(wire) {
	return new Promise(function(resolve, reject) {
		wire.read(1, function(error, result) {
			if (error)
				reject(error);
			else
				resolve(result);
		});

	});
}

function write(wire, command, params) {
	return new Promise(function(resolve, reject) {
		wire.writeBytes(parseInt(command), params, function(error) {
			if (error)
				reject(error);
			else
				resolve();
		});


	});

}

function pause(ms) {
	return new Promise(function(resolve, reject) {
		setTimeout(resolve, ms);
	});
}

module.exports.handler = function(args) {

	try {

		var strip = new NeopixelStrip({device:'/dev/i2c-1'})

		var promise = Promise.resolve();



		promise.then(function() {
			return strip.setStripLength(args.length);

		})

		.then(function() {
			return strip.pause(1000);
		})

		.then(function() {
			return strip.setColor(8, 0, 0);

		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.fadeToColor(0, 8, 0);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.fadeToColor(0, 0, 8);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.fadeToColor(0, 8, 8);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.fadeToColor(8, 8, 0);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.fadeToColor(8, 8, 8);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.fadeToColor(0, 0, 0);
		})
		/*

		Promise.resolve().then(function() {
			return strip.setStripLength(8);
		})
		.then(function() {
			return strip.setColor(0, 0, 0);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.setColor(255, 0, 0);
		})
		.then(function() {
			return strip.pause(1000);
		})
		.then(function() {
			return strip.fadeToColor(0, 255, 0);
		})
		.then(function() {
			return strip.pause(1000);
		})
*/
		.then(function(result) {
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
