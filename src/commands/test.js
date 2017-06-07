
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


function NeopixelStrip(options) {

	var _wire = undefined;

	this.pause = function(ms) {
		return new Promise(function(resolve, reject) {
			setTimeout(resolve, ms);
		});
	}

	this.setColor = function(red, green, blue) {
		return write(wire, 0x10, [red, green, blue]);
	}

	this.colorWipe(red, green, blue, delay) {
		return write(wire, 0x11, [red, green, blue, delay]);
	}

	this.setStripLength(size) {
		return write(wire, 0x12, [size]);
	}

	function write(command, params) {
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

		if (options.length != undefined)
			this.setStripLength(options.length);

	}


};


module.exports.builder = function(args) {

	args.option('command', {alias: 'c', describe:'Command',  default:'color', choices: ['color', 'wipe']});
	args.option('red', {alias: 'r', describe:'red', default: 255});
	args.option('green', {alias: 'g', describe:'green', default: 255});
	args.option('blue', {alias: 'b', describe:'blue', default: 255});
	args.option('wait', {alias: 'w', describe:'Wait', default: 100});

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

		var strip = new NeopixelStrip({device:'/dev/i2c-1', length:4})

		if (args.command == 'color') {
			command = 0x10;
			params = [parseInt(args.red), parseInt(args.green), parseInt(args.blue)];
		}
		if (args.command == 'wipe') {
			command = 0x11;
			params = [parseInt(args.red), parseInt(args.green), parseInt(args.blue), parseInt(args.wait)];
		}

		Promise.resolve().then(function() {
			return strip.setStripLength(wire, 0x12, [4]);
		})
		.then(function() {
			return strip.setColor(0, 0, 0);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.setColor(128, 128, 128);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.setColor(0, 0, 0);
		})
		.then(function() {
			return strip.pause(500);
		})
		.then(function() {
			return strip.setColor(128, 0, 0);
		})
		.then(function() {
			return pause(500);
		})
		.then(function() {
			return strip.setColor(0, 128, 0);
		})
		.then(function() {
			return pause(500);
		})
		.then(function() {
			return strip.setColor(0, 0, 128);
		})
		.then(function() {
			return pause(500);
		})

		.then(function() {
			return strip.colorWipe(128, 0, 0);
		})
		.then(function() {
			return pause(500);
		})
		.then(function() {
			return strip.colorWipe(0, 128, 0);
		})
		.then(function() {
			return pause(500);
		})
		.then(function() {
			return strip.colorWipe(0, 0, 128);
		})
		.then(function() {
			return pause(500);
		})
		.then(function(result) {
			console.log('OK');
		})
		.catch(function(error) {
			console.log(error.message);
		});


	}
	catch(error) {
		console.log(error.stack);
	}

};
