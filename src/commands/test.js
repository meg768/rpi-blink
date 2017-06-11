
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


function NeopixelStrip(options) {

	const ACK = 6;
	const NAK = 21;

	const CMD_INITIALIZE    = 0x10;
	const CMD_SET_COLOR     = 0x11;
	const CMD_FADE_TO_COLOR = 0x12;
	const CMD_WIPE_TO_COLOR = 0x13;
	const CMD_SET_PIXELS    = 0x14;

	var _this = this;
	var _wire = undefined;
	var _length = 0;

	_this.waitForReply = function(loop) {

		// Default to make 25 tries
		if (loop == undefined)
			loop = 100;

		return new Promise(function(resolve, reject) {

			_this.read(1).then(function(bytes) {
				return Promise.resolve(bytes.length > 0 && bytes[0] == ACK ? ACK : NAK);
			})
			.catch(function(error) {
				// If read failure, assume we got back NAK
				return Promise.resolve(NAK);
			})
			.then(function(status) {
				if (status == ACK) {
					return Promise.resolve();
				}
				else {
					if (loop > 0) {
						return _this.pause(100).then(function() {
							return _this.waitForReply(loop - 1);
						});
					}
					else
						return Promise.reject(new Error('Device timed out.'));
				}
			})

			.then(function() {
				resolve();
			})
			.catch(function(error) {
				reject(error);
			});

		});

	}

	_this.pause = function(ms) {

		return new Promise(function(resolve, reject) {
			setTimeout(resolve, ms);
		});
	}

	_this.foo = function(red, green, blue) {
		var bytes = [];

		console.log('Setting pixels');
		bytes.push(CMD_SET_PIXELS);

		for (var i = 0; i < _length; i++) {
			bytes.push(red);
			bytes.push(green);
			bytes.push(blue);

		}

		return _this.send(bytes);

	}
	_this.setColor = function(red, green, blue) {
		red   = parseInt(red);
		green = parseInt(green);
		blue  = parseInt(blue);

		console.log('Setting color to', [red, green, blue]);

		return _this.send([CMD_SET_COLOR, red, green, blue]);
	}

	_this.wipeToColor = function(red, green, blue, delay) {

		console.log('Wiping to color', [red, green, blue]);

		if (delay == undefined)
			delay = 1;

		red    = parseInt(red);
		green  = parseInt(green);
		blue   = parseInt(blue);
		delay  = parseInt(delay);

		return _this.send([CMD_WIPE_TO_COLOR, red, green, blue, delay]);
	}

	_this.fadeToColor = function(red, green, blue, steps) {

		console.log('Fading to color', [red, green, blue]);

		if (steps == undefined)
			steps = 10;

		red    = parseInt(red);
		green  = parseInt(green);
		blue   = parseInt(blue);
		steps  = parseInt(steps);

		return _this.send([CMD_FADE_TO_COLOR, red, green, blue, steps]);
	}


	_this.initialize = function(length) {
		_length = length;
		return _this.send([CMD_INITIALIZE, parseInt(length)]);
	}

	_this.send = function(bytes) {

		return new Promise(function(resolve, reject) {
			_this.write(bytes).then(function() {
				return _this.waitForReply();
			})
			.then(function() {
				resolve();
			})
			.catch(function(error) {
				reject(error);
			});
		});

	};


	_this.write = function(data) {
		return new Promise(function(resolve, reject) {
			_wire.write(data, function(error) {
				if (error)
					reject(error);
				else
					resolve();
			});


		});

	}

	_this.read = function(bytes) {
		return new Promise(function(resolve, reject) {
			_wire.read(bytes, function(error, result) {
				if (error)
					reject(error)
				else
					resolve(result);
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
	args.option('red', {alias: 'r', describe:'red', default: 8});
	args.option('green', {alias: 'g', describe:'green', default: 8});
	args.option('blue', {alias: 'b', describe:'blue', default: 8});
	args.option('wait', {alias: 'w', describe:'Wait', default: 100});
	args.option('length', {alias: 'l', describe:'Length', default: 8});

	args.wrap(null);


};

module.exports.handler = function(args) {

	try {

		var strip = new NeopixelStrip({device:'/dev/i2c-1'})

		var promise = Promise.resolve();



		promise.then(function() {
			return strip.initialize(args.length);

		})

		.then(function() {
			return strip.foo(128, 0, 0);

		})
		.then(function() {
			return strip.pause(1);
		})

		.then(function() {
			return strip.foo(0, 128, 0);

		})
		.then(function() {
			return strip.foo(0, 0, 128);

		})


		.then(function() {
			return strip.fadeToColor(0, 128, 0);
		})
		.then(function() {
			return strip.pause(1);
		})
		.then(function() {
			return strip.fadeToColor(0, 0, 128);
		})
		.then(function() {
			return strip.pause(1);
		})
		.then(function() {
			return strip.wipeToColor(0, 128, 128);
		})
		.then(function() {
			return strip.pause(1);
		})
		.then(function() {
			return strip.fadeToColor(128, 128, 0);
		})
		.then(function() {
			return strip.pause(1);
		})
		.then(function() {
			return strip.wipeToColor(128, 128, 128);
		})
		.then(function() {
			return strip.pause(1);
		})
		.then(function() {
			return strip.fadeToColor(0, 0, 0);
		})


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
