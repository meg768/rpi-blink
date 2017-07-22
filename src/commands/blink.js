
module.exports.command  = 'blink';
module.exports.describe = 'Blink on the specified pin';


module.exports.builder = function(args) {

	args.option('red', {alias: 'r', describe:'red', default: 64});
	args.option('green', {alias: 'g', describe:'green', default: 0});
	args.option('blue', {alias: 'b', describe:'blue', default: 0});
	args.option('segment', {alias: 's', describe:'segment', default: 0});

	args.wrap(null);

};

function delay(ms) {
	return new Promise(function(resolve, reject) {
		setTimeout(resolve, ms);
	});
}

module.exports.handler = function(args) {

	try {
		var socket = require('socket.io-client')('http://app-o.se');

		socket.on('connect', function(data) {

			var data = {};
			data.red = args.red;
			data.green = args.green;
			data.blue = args.blue;
			data.segment = args.segment;

			socket.emit('broadcast', {room:'lamp', event:'color', message:data});

			socket.disconnect();
		});


	}
	catch(error) {
		console.log(error.stack);
	}

};
