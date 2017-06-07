
module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


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

		var I2C = require('i2c');
		var wire = new I2C(0x26, {device: '/dev/i2c-1'}); // point to your i2c address, debug provides REPL interface

		var command = 0;;
		var params = [];

		if (args.command == 'color') {
			command = 0x10;
			params = [parseInt(args.red), parseInt(args.green), parseInt(args.blue)];
		}
		if (args.command == 'wipe') {
			command = 0x11;
			params = [parseInt(args.red), parseInt(args.green), parseInt(args.blue), parseInt(args.wait)];
		}

		Promise.resolve().then(function() {
			return write(wire, 0x10, [0, 0, 0]);
		})
		.then(function() {
			return pause(1000);
		})
		.then(function() {
			return write(wire, 0x10, [128, 128, 128]);
		})
		.then(function() {
			return write(wire, 0x11, [0, 0, 128, 255]);
		})
		.then(function() {
			return write(wire, 0x11, [0, 128, 0, 255]);
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
