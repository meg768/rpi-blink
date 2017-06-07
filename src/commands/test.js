
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
		wire.read(length, function(error, result) {
			if (error)
				reject(error);
			else
				resolve(result);
		});

	});
}

function write(wire, command, args) {
	return new Promise(function(resolve, reject) {
		wire.writeBytes(parseInt(command), params, function(error) {
			if (error)
				reject(error);
			else
				resolve();
		});

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

		write(wire, command, params).then(function() {
			return read(wire);
		})
		.then(function(result) {
			console.log(result);
		})
		.catch(function(error) {
			console.log(error);
		});


	}
	catch(error) {
		console.log(error.stack);
	}

};
