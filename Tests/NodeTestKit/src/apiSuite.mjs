/**
	Runs the API test suite described by suite.json against a freshly started server.

	The steps are the same on every platform and for every product: drop the test databases, clear the servers' run
	files, start the sims, the services the server depends on (the "services" list of suite.json) and the server (they
	create and migrate their databases), check that they use the test databases, run the newman collection and the
	Node scripts, stop the servers in the reverse order. Nothing here is specific to one product; suite.json
	carries the names, ports and files. The server and its database are the TestServer of the kit.

	A suite folder keeps only a run-tests.mjs of two lines:

		import { RunApiSuite } from 'imtcore-node-testkit';
		RunApiSuite(import.meta.url);
*/

import fs from 'node:fs';
import { createRequire } from 'node:module';
import path from 'node:path';
import { spawn } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { parseArgs } from 'node:util';

import { Log, TestServer } from './testServer.mjs';


function SelectFolders(suite, options, collection)
{
	if (options.folder){
		return options.folder;
	}

	const optInFolders = suite.newman.optInFolders ?? [];
	const retVal = collection.item
		.map((item) => item.name)
		.filter((name) => options['include-opt-in'] || !optInFolders.includes(name));

	return retVal;
}


function RunNewman(suite, suiteRoot, resultsDir, options)
{
	/** newman is a package of the suite, not of the kit. */
	const newman = createRequire(path.join(suiteRoot, 'package.json'))('newman');

	const collectionPath = path.resolve(suiteRoot, suite.newman.collection);
	const collection = JSON.parse(fs.readFileSync(collectionPath, 'utf8'));
	const folders = SelectFolders(suite, options, collection);
	Log(`Running ${folders.length} folder(s): ${folders.join(', ')}`);

	const envVar = Object.entries(suite.newman.environmentOverrides).map(([key, value]) => ({
		key,
		value: value.replaceAll('${HTTP_PORT}', suite.server.ports.http).replaceAll('${WEBSOCKET_PORT}', suite.server.ports.webSocket),
	}));

	return new Promise((resolve) => {
		newman.run({
			collection,
			environment: path.resolve(suiteRoot, suite.newman.environment),
			envVar,
			folder: folders,
			reporters: ['cli', 'junit', 'json'],
			reporter: {
				junit: { export: path.join(resultsDir, `${suite.name}.junit.xml`) },
				json: { export: path.join(resultsDir, `${suite.name}.json`) },
			},
			exportEnvironment: path.join(resultsDir, 'environment.json'),
			timeoutRequest: suite.newman.requestTimeoutSeconds * 1000,
		}, (error, summary) => {
			if (error){
				Log(`newman failed to run: ${error.message}`);
				resolve(false);
				return;
			}

			resolve(!summary.error && summary.run.failures.length === 0);
		});
	});
}


function RunScript(suiteRoot, resultsDir, server, options, script)
{
	const args = [
		path.resolve(suiteRoot, script.file),
		'--http', `http://127.0.0.1:${server.suite.server.ports.http}`,
		'--ws', `ws://127.0.0.1:${server.suite.server.ports.webSocket}`,
		'--env', path.join(resultsDir, 'environment.json'),
		'--junit', path.join(resultsDir, `${script.name}.junit.xml`),
	];
	if (options['include-opt-in']){
		args.push('--include-opt-in');
	}

	Log(`Running script ${script.name}`);

	return new Promise((resolve) => {
		const child = spawn(process.execPath, args, { stdio: 'inherit' });
		child.once('exit', (code) => resolve(code === 0));
	});
}


/**
	The --service-bin <name>=<path> options as an object by the service names; a name has to be a service of the suite.
*/
function ParseServiceBinaries(suite, entries)
{
	const retVal = {};
	for (const entry of entries ?? []){
		const separator = entry.indexOf('=');
		const name = entry.slice(0, separator);
		if (separator <= 0 || !(suite.services ?? []).some((service) => service.name === name)){
			throw new Error(`--service-bin expects <name>=<path> with the name of a service of the suite, got ${entry}`);
		}

		retVal[name] = entry.slice(separator + 1);
	}

	return retVal;
}


/**
	Runs the suite of the folder of the given script (import.meta.url of its run-tests.mjs) and ends the process:
	0 when every test passed, 1 when some failed, 2 when the run itself failed.
*/
export function RunApiSuite(scriptUrl)
{
	const suiteDir = path.dirname(fileURLToPath(scriptUrl));

	const { values: options } = parseArgs({
		options: {
			'suite': { type: 'string', default: path.join(suiteDir, 'suite.json') },
			'bin': { type: 'string' },
			'service-bin': { type: 'string', multiple: true },
			'folder': { type: 'string', multiple: true },
			'include-opt-in': { type: 'boolean', default: false },
			'results': { type: 'string' },
			'keep-server': { type: 'boolean', default: false },
			'help': { type: 'boolean', short: 'h', default: false },
		},
	});

	if (options.help){
		console.log(`Usage: node run-tests.mjs [options]

  --bin <path>          Server executable (default: the newest one found under the suite's bin search folders)
  --service-bin <name>=<path>
                        Executable of a service of the suite; repeatable (default: the newest one found under
                        the service's bin search folders)
  --folder <name>       Run only this collection folder; repeatable
  --include-opt-in      Also run the opt-in folders and scripts (known findings, expected to fail)
  --results <dir>       Where reports and logs go (default: results next to suite.json)
  --keep-server         Leave the servers and the sims running after the tests, for debugging
  --suite <file>        Suite description (default: suite.json next to this script)`);
		process.exit(0);
	}

	const suitePath = path.resolve(options.suite);
	const suite = JSON.parse(fs.readFileSync(suitePath, 'utf8'));
	const suiteRoot = path.dirname(suitePath);
	const resultsDir = path.resolve(options.results ?? path.join(suiteRoot, 'results'));

	const server = new TestServer(suite, suiteRoot, {
		bin: options.bin,
		serviceBins: ParseServiceBinaries(suite, options['service-bin']),
		resultsDir,
		keepServer: options['keep-server'],
	});

	async function Main()
	{
		await server.Prepare();

		let succeeded = false;
		try {
			await server.Start();

			succeeded = await RunNewman(suite, suiteRoot, resultsDir, options);

			for (const script of suite.scripts ?? []){
				if (script.optIn && !options['include-opt-in']){
					continue;
				}

				succeeded = await RunScript(suiteRoot, resultsDir, server, options, script) && succeeded;
			}
		}
		finally {
			await server.Finish();
		}

		Log(succeeded ? 'All tests passed' : 'Some tests failed');

		return succeeded;
	}

	process.once('SIGINT', async () => {
		await server.Kill();
		process.exit(130);
	});

	Main().then(
		(succeeded) => process.exit(succeeded ? 0 : 1),
		async (error) => {
			Log(error.message);
			await server.Kill();
			process.exit(2);
		}
	);
}
