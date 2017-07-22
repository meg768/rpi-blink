
module.exports.command  = 'spy';
module.exports.describe = 'Spy Blink';





module.exports.builder = function(args) {

/*
	args.option('command', {alias: 'c', describe:'Command',  default:'none', choices: ['set', 'wipe', 'fade', 'demo', 'none', 'init']});
	args.option('red', {alias: 'r', describe:'red', default: 64});
	args.option('green', {alias: 'g', describe:'green', default: 0});
	args.option('blue', {alias: 'b', describe:'blue', default: 0});
	args.option('pause', {alias: 'p', describe:'Pause', default: 100});
	args.option('length', {alias: 'l', describe:'Length', default: undefined});
*/
	args.wrap(null);


};

module.exports.handler = function(args) {

	var MySQL = require('../js/mysql.js');

	try {
		function process(db) {

			return new Promise(function(resolve, reject) {


				var data = {};

				Promise.resolve().then(function() {
					return db.query('select date, close from quotes where symbol = ? order by date desc limit 1', ['SPY']);
				})
				.then(function(result) {
					data.SPY = result[0].close;
					return Promise.resolve();
				})
				.then(function() {
					 return db.query('SELECT * FROM stocks WHERE symbol = ?',['SPY']);
				})

				.then(function(result) {
					result = result[0];

					data.SMA10  = {value: result.SMA10,  change:(1 - (result.SMA10/data.SPY)) * 100};
					data.SMA50  = {value: result.SMA50,  change:(1 - (result.SMA50/data.SPY)) * 100};
					data.SMA200 = {value: result.SMA200, change:(1 - (result.SMA200/data.SPY)) * 100};
					/*
					data.SMA50 = result[0].SMA50;
					data.SMA200 = result[0].SMA200;
					data.SMA10_change = (1 - (result[0].SMA10/data.SPY)) * 100;
					data.SMA50_change = (1 - (result[0].SMA50/data.SPY)) * 100;
					data.SMA200_change = (1 - (result[0].SMA200/data.SPY)) * 100;
*/
					console.log(data);
					//throw new Error('UPPS');
					resolve();
				})
				.catch(function(error) {
					reject(error);
				});

			});

		}

		function work() {

			return new Promise(function(resolve, reject) {
				var mysql = new MySQL();

				mysql.connect().then(function(db) {

					process(db).then(function() {
						db.end();
						resolve();
					})

					.catch(function(error) {
						db.end();
						reject(error);
					})
				})

				.catch(function(error) {
					reject(error);
				});
			});
		}


		function loop() {

			work().then(function() {
			})

			.catch(function(error) {
				console.log(error);
			})

			.then(function() {
				setTimeout(loop, 10000);
			})
		}


		function run() {

			var socket = require('socket.io-client')('http://app-o.se');
			console.log(socket);

			socket.on('hello', function(data) {
				console.log('HELLO', data);
			});

			loop();
		}

		run();


	}
	catch(error) {
		console.log(error.stack);
	}

};
