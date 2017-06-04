var wpi = require('wiring-pi');

module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


module.exports.builder = function(args) {

	args.option('mode', {alias: 'm', describe:'Mode', default: 0});
	args.option('command', {alias: 'c', describe:'command', default: 0});

	args.wrap(null);


};


module.exports.handler = function(args) {

	try {

		var i2c = require('i2c');
		var wire = new i2c(0x26, {device: '/dev/i2c-1'}); // point to your i2c address, debug provides REPL interface

		wire.writeBytes(parseInt(args.command), [2, 4], function(error) {
			if (error)
				console.log(error);
		});

		wire.on('data', function(data) {
		  // result for continuous stream contains data buffer, address, length, timestamp
		});


	}
	catch(error) {
		console.log(error.stack);
	}

};
