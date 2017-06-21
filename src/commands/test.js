
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';

function NeopixelSegment(options) {

	var _this   = this;
	var _strip  = undefined;
	var _length = undefined;
	var _offset = undefined;

	function init() {
		_strip  = options.strip;
		_length = options.length;
		_offset = options.offset;
	};

	_this.setColor = function(red, green, blue) {
		return _strip.setColor(_offset, _length, red, green, blue);
	}

	_this.fadeToColor = function(red, green, blue, steps) {
		return _strip.fadeToColor(_offset, _length, red, green, blue, steps);
	}

	_this.wipeToColor = function(red, green, blue, delay) {
		return _strip.wipeToColor(_offset, _length, red, green, blue, delay);
	}


	init();
};

function NeopixelStrip(options) {

	const ACK = 6;
	const NAK = 21;

	const CMD_INITIALIZE    = 0x10;
	const CMD_SET_COLOR     = 0x11;
	const CMD_FADE_TO_COLOR = 0x12;
	const CMD_WIPE_TO_COLOR = 0x13;

	var _this          = this;         // That
	var _wire          = undefined;    // I2C communication
	var _length        = 0;            // Length of Neopixels
	var _debug         = 1;            // Output log messages to console?
	var _timeout       = 5000;         // Read/write timeout in ms
	var _retryInterval = 100;          // Milliseconds to wait before retrying read/write

	function debug() {
		if (_debug)
			console.log.apply(this, arguments);
	}

	_this.waitForReply = function(timestamp) {

		if (timestamp == undefined)
			timestamp = new Date();

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
					var now = new Date();

					if (now.getTime() - timestamp.getTime() < _timeout) {
						return _this.pause(_retryInterval).then(function() {
							return _this.waitForReply(timestamp);
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




	_this.setColor = function(offset, length, red, green, blue) {
		red   = parseInt(red);
		green = parseInt(green);
		blue  = parseInt(blue);

		console.log('Setting color to', [red, green, blue]);

		return _this.send(CMD_SET_COLOR, [offset, length, red, green, blue]);
	}

	_this.wipeToColor = function(offset, length, red, green, blue, delay) {

		console.log('Wiping to color', [red, green, blue]);

		if (delay == undefined)
			delay = 1;

		red    = parseInt(red);
		green  = parseInt(green);
		blue   = parseInt(blue);
		delay  = parseInt(delay);

		return _this.send(CMD_WIPE_TO_COLOR, [offset, length, red, green, blue, delay]);
	}

	_this.fadeToColor = function(offset, length, red, green, blue, steps) {

		console.log('Fading to color', [red, green, blue]);

		if (steps == undefined)
			steps = 64;

		red    = parseInt(red);
		green  = parseInt(green);
		blue   = parseInt(blue);
		steps  = parseInt(steps);

		return _this.send(CMD_FADE_TO_COLOR, [offset, length, red, green, blue, steps]);
	}


	_this.initialize = function(length) {
		_length = length;
		return _this.send(CMD_INITIALIZE, [parseInt(length)]);
	}

	_this.send = function(command, bytes, timestamp) {

		if (timestamp == undefined)
			timestamp = new Date();

		return new Promise(function(resolve, reject) {

			_this.write(command, bytes).then(function() {
				return _this.waitForReply();
			})
			.catch(function(error) {
				var now = new Date();

				if (now.getTime() - timestamp.getTime() < _timeout) {

					return _this.pause(_retryInterval).then(function() {
						debug('send() failed, trying to send again...');
						return _this.send(command, bytes, timestamp);
					});
				}
				else {
					return Promise.reject(new Error('Device timed out. Could not write to device'));

				}
			})
			.then(function() {
				resolve();
			})
			.catch(function(error) {
				reject(error);
			});
		});

	};




	_this.write = function(command, data) {
		return new Promise(function(resolve, reject) {
			console.log('Writing bytes', command, data);
			_wire.writeBytes(command, data, function(error) {
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
				if (error) {
					console.log(error.message);
					reject(error)

				}
				else {
					console.log('Got bytes', result);
					resolve(result);

				}
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

	args.option('command', {alias: 'c', describe:'Command',  default:'none', choices: ['set', 'wipe', 'fade', 'demo', 'none']});
	args.option('red', {alias: 'r', describe:'red', default: 64});
	args.option('green', {alias: 'g', describe:'green', default: 0});
	args.option('blue', {alias: 'b', describe:'blue', default: 0});
	args.option('pause', {alias: 'p', describe:'Pause', default: 100});
	args.option('length', {alias: 'l', describe:'Length', default: undefined});

	args.wrap(null);


};

module.exports.handler = function(args) {

	try {

		var strip = new NeopixelStrip({device:'/dev/i2c-1'})

		var bar1 = new NeopixelSegment({strip: strip, offset:0, length:8);
		var bar2 = new NeopixelSegment({strip: strip, offset:8, length:8);
		var promise = Promise.resolve();

		promise.then(function() {
			return strip.initialize(16);
		})
		.then(function() {
			return bar1.fadeToColor(128, 0, 0);
		})
		.then(function(result) {
			return strip.pause(2000);
		})
		.then(function() {
			return bar1.fadeToColor(0, 128, 0);

		})
		.then(function(result) {
			return strip.pause(2000);
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
