
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


function NeopixelStrip(options) {

	var _this = this;
	var _wire = undefined;

	_this.wait = function(loop) {
		if (loop == undefined)
			loop = 100;

		return new Promise(function(resolve, reject) {

			_this.readReply().then(function() {
				resolve();
			})
			.catch(function(error) {
				if (loop > 0) {
					_this.pause(100).then(function() {
						return _this.wait(loop - 1);
					})
					.then(function() {
						resolve();
					})
					.catch(function(error) {
						reject(error);
					});

				}
				else {
					reject(error);
				}
			});

		});

	}

	_this.readReply = function() {
		return new Promise(function(resolve, reject) {

			_this.read(1).then(function(bytes) {
				console.log('Read bytes', bytes);

				if (bytes.length > 0 && bytes[0] == 6) {
					return Promise.resolve();
				}
				else {
					console.log('Invalid reply, keep reading...');
					return Promise.reject(new Error('Invalid reply'));
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

	_this.setColor = function(red, green, blue) {
		red   = parseInt(red);
		green = parseInt(green);
		blue  = parseInt(blue);

		console.log('Setting color to', [red, green, blue]);

		return _this.send([0x10, red, green, blue]);
	}

	_this.fadeToColor = function(red, green, blue, steps) {

		//return _this.setColor(red, green, blue);
		console.log('Fading to color', [red, green, blue]);

		if (steps == undefined)
			steps = 255;

		red    = parseInt(red);
		green  = parseInt(green);
		blue   = parseInt(blue);
		steps  = parseInt(steps);

		return _this.send([0x13, red, green, blue, steps]);
	}


	_this.setStripLength = function(length) {
		return _this.send([0x12, parseInt(length)]);
	}

	_this.send = function(bytes) {

		return new Promise(function(resolve, reject) {
			_this.write(bytes).then(function() {
				return _this.wait();
			})
			.then(function() {
				resolve();
			})
			.catch(function(error) {
				reject(error);
			});
		});

	};


	_this.write = function(bytes) {
		return new Promise(function(resolve, reject) {
			_wire.write(bytes, function(error) {
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

		console.log('Initiating...');
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
			return strip.setStripLength(args.length);

		})
		/*
		.then(function() {
			return strip.writeByte(0x14);

		})
		*/
		.then(function() {
			return strip.pause(1000);
		})


		.then(function() {
			return strip.setColor(128, 0, 0);

		})
		.then(function() {
			return strip.pause(1000);
		})
		.then(function() {
			return strip.fadeToColor(0, 128, 0);
		})
		.then(function() {
			return strip.pause(1000);
		})
		.then(function() {
			return strip.fadeToColor(0, 0, 128);
		})
		.then(function() {
			return strip.pause(1000);
		})
		.then(function() {
			return strip.fadeToColor(0, 128, 128);
		})
		.then(function() {
			return strip.pause(1000);
		})
		.then(function() {
			return strip.fadeToColor(128, 128, 0);
		})
		.then(function() {
			return strip.pause(1000);
		})
		.then(function() {
			return strip.fadeToColor(128, 128, 128);
		})
		.then(function() {
			return strip.pause(1000);
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
