var wpi = require('wiring-pi');

module.exports.command  = 'test';
module.exports.describe = 'Test Wiring Pi';


module.exports.builder = function(args) {

	args.option('mode', {alias: 'm', describe:'Mode', default: 0});

	args.wrap(null);


};


module.exports.handler = function(args) {

	try {

		var id = wpi.wiringPiI2CSetup(0x26);
		wpi.wiringPiI2CWriteReg8(id, args.mode);
		wpi.wiringPiI2CClose(id);

	}
	catch(error) {
		console.log(error.stack);
	}

};
