#!/usr/bin/env node

var App = function() {


	function run() {
		try {
			var args = require('yargs');

			args.usage('Usage: $0 <command> [options]')

			args.command(require('./src/commands/off.js'));
			args.command(require('./src/commands/on.js'));
			args.command(require('./src/commands/blink.js'));
			args.command(require('./src/commands/pwm.js'));

			args.help();

			args.check(function(argv) {
				return true;
			});

			args.demand(1);

			args.argv;

		}
		catch(error) {
			console.log(error.stack);
			process.exit(-1);
		}

	};

	run();
};

module.exports = new App();
