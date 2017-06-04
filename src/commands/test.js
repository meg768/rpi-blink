var wpi = require('wiring-pi');

module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


module.exports.builder = function(args) {

	args.option('mode', {alias: 'm', describe:'Mode', default: 0});
	args.option('color', {alias: 'c', describe:'command', default: 0});
	args.option('red', {alias: 'r', describe:'red', default: 255});
	args.option('green', {alias: 'g', describe:'green', default: 255});
	args.option('blue', {alias: 'b', describe:'blue', default: 255});

	args.wrap(null);


};


module.exports.handler = function(args) {

	try {

		var I2C = require('i2c');
		var wire = new I2C(0x26, {device: '/dev/i2c-1'}); // point to your i2c address, debug provides REPL interface

		wire.writeBytes(27, [parseInt(args.red), parseInt(args.green), parseInt(args.blue)], function(error) {
			if (error)
				console.log(error);
		});

		wire.on('data', function(data) {
			console.log(data);
		  // result for continuous stream contains data buffer, address, length, timestamp
		});


	}
	catch(error) {
		console.log(error.stack);
	}

};
