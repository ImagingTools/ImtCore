/**
	The test server of a Node.js test suite: its settings, its database, its sims and its process.

	The suite describes the server in its suite.json (name, ports, settings folder, readiness request, simulated
	remote services, database); see the README of the kit (Docs/Development/TestInfrastructure.md of ImtCore). A run
	drops the test database and the files a previous run left, starts the sims and the server (it creates and migrates
	the database), and stops them again.

	The server's configuration comes from its composition and its own persisted settings; nothing here writes them.
	To drop the database it uses the same credentials as the server, field by field: the environment variables named
	in suite.json (TEST_PG_HOST, _PORT, _USER, _PASSWORD with the prefix of the suite), then the server's persisted
	settings, then the composition defaults repeated in suite.json.
*/

import { spawn } from 'node:child_process';
import fs from 'node:fs';
import { createRequire } from 'node:module';
import net from 'node:net';
import path from 'node:path';
import { fileURLToPath } from 'node:url';


export function Log(message)
{
	console.log(`[run-tests] ${message}`);
}


function UnescapeXml(text)
{
	return text
		.replaceAll('&quot;', '"')
		.replaceAll('&apos;', "'")
		.replaceAll('&lt;', '<')
		.replaceAll('&gt;', '>')
		.replaceAll('&amp;', '&');
}


/**
	Only a database whose name ends with _test is dropped or used, so a test never changes a production database.
*/
function VerifyTestDatabaseName(databaseName)
{
	if (!databaseName.endsWith('_test')){
		throw new Error(`${databaseName} is not a test database: its name has to end with _test`);
	}
}


export function IsPortOpen(port)
{
	return new Promise((resolve) => {
		const socket = net.connect({ host: '127.0.0.1', port });
		socket.setTimeout(1000);
		socket.once('connect', () => { socket.destroy(); resolve(true); });
		socket.once('timeout', () => { socket.destroy(); resolve(false); });
		socket.once('error', () => resolve(false));
	});
}


export class TestServer
{
	/** The files the output of the server goes to, in the results folder: standard output, then standard error. */
	static OUTPUT_FILES = ['server-stdout.log', 'server-stderr.log'];

	/**
		options: bin (the server executable, else the newest one under the suite's bin search folders), resultsDir
		(where the logs go), keepServer (leave the server and the sims running when the runner ends).
	*/
	constructor(suite, suiteRoot, options)
	{
		this.suite = suite;
		this.suiteRoot = suiteRoot;

		/** The database driver of the suite: the kit has no packages of its own, it uses those the suite installed. */
		this.pg = createRequire(path.join(suiteRoot, 'package.json'))('pg');
		this.bin = options.bin;
		this.resultsDir = options.resultsDir;
		this.keepServer = options.keepServer;

		this.serverProcess = null;
		this.serverStarts = 0;
		this.simProcesses = [];
		this.settingsFolder = null;
		this.databaseSettings = null;
	}


	get HttpUrl()
	{
		return `http://localhost:${this.suite.server.ports.http}`;
	}


	get WebSocketUrl()
	{
		return `ws://localhost:${this.suite.server.ports.webSocket}`;
	}


	/**
		Finds the server and its settings, checks that its ports are free, drops the test database and removes the
		files of the previous run.
	*/
	async Prepare()
	{
		fs.mkdirSync(this.resultsDir, { recursive: true });
		for (const name of TestServer.OUTPUT_FILES){
			fs.rmSync(path.join(this.resultsDir, name), { force: true });
		}

		this.binary = this.FindProgram(this.suite.server.name, this.bin);
		this.settingsFolder = this.ResolveSettingsFolder();
		this.databaseSettings = this.ResolveDatabaseSettings();
		Log(`Server: ${this.binary}`);
		Log(`Database: ${this.databaseSettings.name} on ${this.databaseSettings.host}:${this.databaseSettings.port} as ${this.databaseSettings.user}`);
		Log(`Settings: ${this.settingsFolder}`);

		await this.EnsurePortsAreFree();
		await this.DropDatabase();
		this.RemoveRunFiles();
	}


	/**
		Starts the sims (unless they already run) and the server, and waits until the server answers on the
		test database.
	*/
	async Start()
	{
		if (this.simProcesses.length === 0){
			await this.StartSims();
		}

		this.StartServer();
		await this.WaitForServer();
		await this.VerifyServerDatabase();
		Log('Server is ready');
	}


	/**
		Ends the run: stops the server and the sims, unless they are to be kept, and copies the server logs.
	*/
	async Finish()
	{
		if (this.keepServer && this.serverProcess !== null){
			Log(`Server left running (pid ${this.serverProcess.pid}), sims too (pids ${this.simProcesses.map((sim) => sim.pid).join(', ')})`);
		}
		else {
			await this.Kill();
		}

		this.CollectServerLogs();
	}


	async Kill()
	{
		await this.StopServer();
		this.StopSims();
	}


	CreateDatabaseClient(databaseName = this.databaseSettings.name)
	{
		return new this.pg.Client({
			host: this.databaseSettings.host,
			port: this.databaseSettings.port,
			user: this.databaseSettings.user,
			password: this.databaseSettings.password,
			database: databaseName,
		});
	}


	/**
		Reads the database access parameter from the server's persisted settings: only the attributes of that one
		element, so the rest of the file may change freely. Returns an empty object before the server first saved it.
	*/
	ReadPersistedDatabaseSettings()
	{
		const persisted = this.suite.server.settings.persistedDatabase;
		const filePath = path.join(this.settingsFolder, persisted.file);
		if (!fs.existsSync(filePath)){
			return {};
		}

		const content = fs.readFileSync(filePath, 'utf8');
		const element = content.match(new RegExp(`<Parameter\\s+Id="${persisted.parameterId}"([^>]*)/?>`));
		if (element === null){
			return {};
		}

		const retVal = {};
		for (const [, name, value] of element[1].matchAll(/(\w+)="([^"]*)"/g)){
			retVal[name] = UnescapeXml(value);
		}

		return retVal;
	}


	ResolveDatabaseSettings()
	{
		const database = this.suite.database;
		const prefix = database.environmentPrefix;
		const persisted = this.ReadPersistedDatabaseSettings();
		const defaults = database.compositionDefaults;

		/** The runner drops the database and the server connects to the one of its persisted settings. */
		VerifyTestDatabaseName(database.name);
		if (persisted.DatabaseName !== undefined){
			VerifyTestDatabaseName(persisted.DatabaseName);
		}

		const retVal = {
			host: process.env[`${prefix}_HOST`] || persisted.Host || defaults.host,
			port: Number(process.env[`${prefix}_PORT`] || persisted.Port || defaults.port),
			user: process.env[`${prefix}_USER`] || persisted.UserName || defaults.user,
			password: process.env[`${prefix}_PASSWORD`] ?? persisted.Password ?? defaults.password,
			name: database.name,
			maintenanceName: database.maintenanceName,
		};

		return retVal;
	}


	/**
		The same folder ifile::CSystemLocationComp resolves for shared application data:
		<public folder>/<company>/<product>. The public folder is fixed per platform there.
	*/
	ResolveSettingsFolder()
	{
		const publicFolder = process.platform === 'win32' ? 'C:/Users/Public' : '/Users/Shared';
		const settings = this.suite.server.settings;

		const retVal = path.join(publicFolder, settings.company, settings.product);

		return retVal;
	}


	/**
		A program of the build: the given path, else the newest one with that name under the bin search folders (those of
		the server by default, relative to the folder of suite.json).
	*/
	FindProgram(name, explicitPath, searchFolders = this.suite.server.binSearchFolders)
	{
		if (explicitPath){
			return path.resolve(explicitPath);
		}

		const fileName = name + (process.platform === 'win32' ? '.exe' : '');
		const candidates = [];
		for (const searchFolder of searchFolders){
			const root = path.resolve(this.suiteRoot, searchFolder);
			if (!fs.existsSync(root)){
				continue;
			}

			for (const entry of fs.readdirSync(root, { withFileTypes: true })){
				const candidate = path.join(root, entry.name, fileName);
				if (entry.isDirectory() && fs.existsSync(candidate)){
					candidates.push({ path: candidate, time: fs.statSync(candidate).mtimeMs });
				}
			}
		}

		if (candidates.length === 0){
			throw new Error(`${fileName} not found under ${searchFolders.join(', ')}; build it or pass its path`);
		}

		candidates.sort((first, second) => second.time - first.time);

		return candidates[0].path;
	}


	async EnsurePortsAreFree()
	{
		const ports = [
			...Object.values(this.suite.server.ports),
			...(this.suite.sims ?? []).flatMap((sim) => [sim.port, ...Object.values(sim.otherPorts ?? {})]),
		];
		for (const port of ports){
			if (await IsPortOpen(port)){
				throw new Error(`Port ${port} is already in use; stop the process that holds it (a server left by --keep-server?)`);
			}
		}
	}


	/**
		The script of a sim: a sim of the kit ("kit": "sim-smtp.mjs" in suite.json, from the sims folder of the kit),
		else the file of the suite, relative to suite.json.
	*/
	ResolveSimFile(sim)
	{
		if (sim.kit){
			return path.join(path.dirname(fileURLToPath(import.meta.url)), '..', 'sims', sim.kit);
		}

		return path.resolve(this.suiteRoot, sim.file);
	}


	/**
		Starts the simulated remote services the server talks to, each a Node script listening on its port. A sim that
		listens on more ports lists them under otherPorts, and gets each one as --<name>-port.
	*/
	async StartSims()
	{
		for (const sim of this.suite.sims ?? []){
			const log = fs.openSync(path.join(this.resultsDir, `${sim.name}.log`), 'w');
			const otherPortArguments = Object.entries(sim.otherPorts ?? {}).flatMap(([name, port]) => [`--${name}-port`, String(port)]);
			const simProcess = spawn(process.execPath, [this.ResolveSimFile(sim), '--port', String(sim.port), ...otherPortArguments], {
				stdio: ['ignore', log, log],
				detached: this.keepServer,
			});
			if (this.keepServer){
				simProcess.unref();
			}
			this.simProcesses.push(simProcess);

			const deadline = Date.now() + 10000;
			while (!(await IsPortOpen(sim.port))){
				if (Date.now() > deadline){
					throw new Error(`${sim.name} did not listen on ${sim.port}; see results/${sim.name}.log`);
				}

				await new Promise((resolve) => setTimeout(resolve, 200));
			}
		}
	}


	StopSims()
	{
		for (const simProcess of this.simProcesses.splice(0)){
			simProcess.kill();
		}
	}


	/**
		Drops the test database; the server creates it again and applies the migrations on startup.
	*/
	async DropDatabase()
	{
		const client = this.CreateDatabaseClient(this.databaseSettings.maintenanceName);
		await client.connect();
		try {
			await client.query(
				'SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = $1 AND pid != pg_backend_pid()',
				[this.databaseSettings.name]
			);
			await client.query(`DROP DATABASE IF EXISTS ${client.escapeIdentifier(this.databaseSettings.name)}`);
		}
		finally {
			await client.end();
		}
	}


	/**
		Guards against a server that runs on another database than the test one, for example because its persisted
		settings name a different database: the tests would then change that database.
	*/
	async VerifyServerDatabase()
	{
		const databaseName = this.databaseSettings.name;
		const client = this.CreateDatabaseClient();
		try {
			await client.connect();
		}
		catch (error) {
			throw new Error(`The server did not create ${databaseName} (${error.message}); check the database name in its settings`);
		}

		try {
			const result = await client.query("SELECT count(*)::int AS count FROM information_schema.tables WHERE table_schema = 'public'");
			if (result.rows[0].count === 0){
				throw new Error(`${databaseName} has no tables: the server did not migrate it`);
			}
		}
		finally {
			await client.end();
		}
	}


	/**
		Removes the files a previous run left in the server's settings folder that would change this run, such as the
		cache database. The persisted settings stay: they are the developer's configuration of the test server.
	*/
	RemoveRunFiles()
	{
		for (const file of this.suite.server.settings.runFiles){
			fs.rmSync(path.join(this.settingsFolder, file), { force: true });
		}
	}


	/**
		Every start of a run appends its output to the same files, under a line that numbers the start. The first start
		migrates the test database, and its output is the only record of the migration: the log file of the server is
		one of the run files, removed before the server starts on the test data.
	*/
	StartServer()
	{
		this.serverStarts += 1;
		const [stdout, stderr] = TestServer.OUTPUT_FILES.map((name) => {
			const file = path.join(this.resultsDir, name);
			fs.appendFileSync(file, `[test-server] Server start ${this.serverStarts}\n`);

			return fs.openSync(file, 'a');
		});

		/**
			Per platform, because the deployed Qt platform plugins differ: Windows has only qwindows, while Linux agents
			without a display need offscreen.
		*/
		const argumentsByPlatform = this.suite.server.arguments;
		const serverArguments = argumentsByPlatform[process.platform] ?? argumentsByPlatform.default;

		/**
			A server kept for debugging must outlive the runner; on Windows a child that is not detached ends with it.
		*/
		const serverProcess = spawn(this.binary, serverArguments, {
			cwd: path.dirname(this.binary),
			stdio: ['ignore', stdout, stderr],
			detached: this.keepServer,
		});
		if (this.keepServer){
			serverProcess.unref();
		}

		serverProcess.once('exit', (code, signal) => {
			Log(`Server exited (code ${code}, signal ${signal})`);
			if (this.serverProcess === serverProcess){
				this.serverProcess = null;
			}
		});

		this.serverProcess = serverProcess;
	}


	async IsServerAnswering(readiness)
	{
		try {
			const response = await fetch(`http://127.0.0.1:${this.suite.server.ports.http}${readiness.path}`, {
				method: 'POST',
				headers: { 'Content-Type': 'application/json' },
				body: JSON.stringify(readiness.body),
				signal: AbortSignal.timeout(5000),
			});

			return response.status === 200;
		}
		catch {
			return false;
		}
	}


	async WaitForServer()
	{
		const readiness = this.suite.server.readiness;
		const deadline = Date.now() + readiness.timeoutSeconds * 1000;
		while (Date.now() < deadline){
			if (this.serverProcess === null){
				throw new Error('Server exited before it became ready; see the server logs in the results folder');
			}

			if (await this.IsServerAnswering(readiness)){
				return;
			}

			await new Promise((resolve) => setTimeout(resolve, 1000));
		}

		throw new Error(`Server did not answer within ${readiness.timeoutSeconds} s`);
	}


	async StopServer()
	{
		const serverProcess = this.serverProcess;
		if (serverProcess === null){
			return;
		}

		const exited = new Promise((resolve) => serverProcess.once('exit', resolve));
		serverProcess.kill();

		const killTimer = setTimeout(() => serverProcess.kill('SIGKILL'), 10000);
		await exited;
		clearTimeout(killTimer);
	}


	CollectServerLogs()
	{
		for (const logFile of this.suite.server.settings.logFiles){
			const source = path.join(this.settingsFolder, logFile);
			if (fs.existsSync(source)){
				fs.copyFileSync(source, path.join(this.resultsDir, path.basename(logFile)));
			}
		}
	}
}
