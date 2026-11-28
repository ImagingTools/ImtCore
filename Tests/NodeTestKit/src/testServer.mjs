/**
	The test server of a Node.js test suite: its settings, its database, its sims and its process.

	The suite describes the server in its suite.json (name, ports, settings folder, readiness request, simulated
	remote services, database); see the README of the kit (Docs/Development/TestInfrastructure.md of ImtCore). A run
	drops the test database and the files a previous run left, starts the sims and the server (it creates and migrates
	the database), and stops them again.

	A suite may list services the server depends on (the "services" list of suite.json, for example the authorization
	server of the product). They are started in their order before the server, each with its own test database, and
	stopped in the reverse order after it. A service may have a setup step, a Node script that prepares what the
	following servers need and passes them environment variables.

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
	Replaces ${NAME} by the value of the environment variable. Returns null if a variable is not set, so that a
	search folder that depends on it (${PUMADIR}/Bin) is skipped on a machine without it.
*/
function ExpandEnvironment(text)
{
	let isResolved = true;
	const retVal = text.replace(/\$\{(\w+)\}/g, (match, name) => {
		if (!process.env[name]){
			isResolved = false;
		}

		return process.env[name] ?? '';
	});

	return isResolved ? retVal : null;
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
		(where the logs go), keepServer (leave the servers and the sims running when the runner ends), serviceBins
		(the executables of the services by their names, else the newest ones under their bin search folders).
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
		this.serviceBins = options.serviceBins ?? {};

		/** The services of the suite in the order of their start: { spec, binary, settingsFolder, databaseSettings, process, starts }. */
		this.services = [];

		/** The environment variables the setup steps of the services pass to the servers started after them. */
		this.serverEnvironment = {};

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

		this.services = [];
		for (const spec of this.suite.services ?? []){
			this.services.push(this.DescribeService(spec));
		}

		this.binary = this.FindProgram(this.suite.server.name, this.bin);
		this.settingsFolder = this.ResolveSettingsFolder(this.suite.server);
		this.databaseSettings = this.ResolveDatabaseSettings(this.suite.server, this.suite.database.name, this.settingsFolder);
		Log(`Server: ${this.binary}`);
		Log(`Database: ${this.databaseSettings.name} on ${this.databaseSettings.host}:${this.databaseSettings.port} as ${this.databaseSettings.user}`);
		Log(`Settings: ${this.settingsFolder}`);

		await this.EnsurePortsAreFree();
		for (const service of this.services){
			if (service.databaseSettings !== null){
				await this.DropDatabase(service.databaseSettings);
			}

			this.RemoveRunFiles(service.spec, service.settingsFolder);
		}

		await this.DropDatabase();
		this.RemoveRunFiles(this.suite.server, this.settingsFolder);
	}


	/**
		Finds the executable, the settings folder and the test database of a service of the suite, and removes the
		output files of its previous run.
	*/
	DescribeService(spec)
	{
		for (const name of this.GetOutputFiles(spec.name)){
			fs.rmSync(path.join(this.resultsDir, name), { force: true });
		}

		const binary = this.FindProgram(spec.name, this.serviceBins[spec.name], spec.binSearchFolders);
		const settingsFolder = spec.settings === undefined ? null : this.ResolveSettingsFolder(spec);
		const databaseName = spec.database?.name;
		const databaseSettings = databaseName === undefined ? null : this.ResolveDatabaseSettings(spec, databaseName, settingsFolder);
		Log(`${spec.name}: ${binary}`);
		if (databaseSettings !== null){
			Log(`Database: ${databaseSettings.name} on ${databaseSettings.host}:${databaseSettings.port} as ${databaseSettings.user}`);
		}

		const retVal = { spec, binary, settingsFolder, databaseSettings, process: null, starts: 0 };

		return retVal;
	}


	/** The files the output of a program goes to, in the results folder: standard output, then standard error. */
	GetOutputFiles(name)
	{
		return [`${name}-stdout.log`, `${name}-stderr.log`];
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

		for (const service of this.services){
			if (service.process === null){
				await this.StartService(service);
			}
		}

		this.StartServer();
		await this.WaitForServer();
		await this.VerifyServerDatabase();
		Log('Server is ready');
	}


	/**
		Starts a service and waits until it is ready and has migrated its database; then runs its setup step.
	*/
	async StartService(service)
	{
		service.starts += 1;
		const serviceProcess = this.LaunchProgram(service.spec, service.binary, service.spec.name, service.starts);
		serviceProcess.once('exit', (code, signal) => {
			Log(`${service.spec.name} exited (code ${code}, signal ${signal})`);
			if (service.process === serviceProcess){
				service.process = null;
			}
		});

		service.process = serviceProcess;

		await this.WaitForProgram(service.spec, () => service.process !== null);
		if (service.databaseSettings !== null){
			await this.VerifyServerDatabase(service.databaseSettings, service.spec.readiness.timeoutSeconds);
		}

		Log(`${service.spec.name} is ready`);
		if (service.spec.setup !== undefined){
			await this.RunServiceSetup(service.spec);
		}
	}


	/**
		Runs the setup step of a service once it is ready: a Node script that prepares what the servers started after it
		need, such as a token, and writes the environment variables they get to the file of --out as
		{ "environment": { <name>: <value> } }. The values are not logged, since they may be secrets.
	*/
	async RunServiceSetup(spec)
	{
		const setup = spec.setup;
		const outputPath = path.join(this.resultsDir, `${setup.name}.json`);
		fs.rmSync(outputPath, { force: true });

		const extraArguments = Object.entries(setup.arguments ?? {}).flatMap(([name, value]) => [`--${name}`, String(value)]);
		const args = [
			this.ResolveKitFile('setup', setup),
			'--http', `http://127.0.0.1:${spec.ports.http}`,
			'--env', path.resolve(this.suiteRoot, this.suite.newman.environment),
			'--out', outputPath,
			...extraArguments,
		];

		Log(`Running setup ${setup.name}`);
		const isSucceeded = await new Promise((resolve) => {
			const child = spawn(process.execPath, args, { stdio: 'inherit' });
			child.once('exit', (code) => resolve(code === 0));
		});
		if (!isSucceeded || !fs.existsSync(outputPath)){
			throw new Error(`Setup ${setup.name} of ${spec.name} failed`);
		}

		const output = JSON.parse(fs.readFileSync(outputPath, 'utf8'));
		Object.assign(this.serverEnvironment, output.environment ?? {});
		fs.rmSync(outputPath, { force: true });
		Log(`Setup ${setup.name} passes ${Object.keys(output.environment ?? {}).join(', ')}`);
	}


	/**
		Ends the run: stops the server and the sims, unless they are to be kept, and copies the server logs.
	*/
	async Finish()
	{
		if (this.keepServer && this.serverProcess !== null){
			const services = this.services.filter((service) => service.process !== null);
			const servicesText = services.map((service) => `${service.spec.name} (pid ${service.process.pid})`).join(', ');
			Log(`Server left running (pid ${this.serverProcess.pid}), services ${servicesText || 'none'}, sims too (pids ${this.simProcesses.map((sim) => sim.pid).join(', ')})`);
		}
		else {
			await this.Kill();
		}

		this.CollectServerLogs();
	}


	/**
		Stops the server first, so it does not lose the services it uses while it is shutting down, then the services
		in the reverse order of their start, then the sims.
	*/
	async Kill()
	{
		await this.StopServer();
		for (const service of [...this.services].reverse()){
			await this.StopProgram(service.process);
		}

		this.StopSims();
	}


	CreateDatabaseClient(databaseName = this.databaseSettings.name, databaseSettings = this.databaseSettings)
	{
		return new this.pg.Client({
			host: databaseSettings.host,
			port: databaseSettings.port,
			user: databaseSettings.user,
			password: databaseSettings.password,
			database: databaseName,
		});
	}


	/**
		Reads the database access parameter from the server's persisted settings: only the attributes of that one
		element, so the rest of the file may change freely. Returns an empty object before the server first saved it.
	*/
	ReadPersistedDatabaseSettings(spec, settingsFolder)
	{
		const persisted = spec.settings?.persistedDatabase;
		if (persisted === undefined){
			return {};
		}

		const filePath = path.join(settingsFolder, persisted.file);
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


	ResolveDatabaseSettings(spec, databaseName, settingsFolder)
	{
		const database = this.suite.database;
		const prefix = database.environmentPrefix;
		const persisted = this.ReadPersistedDatabaseSettings(spec, settingsFolder);
		const defaults = database.compositionDefaults;

		/** The runner drops the database and the server connects to the one of its persisted settings. */
		VerifyTestDatabaseName(databaseName);
		if (persisted.DatabaseName !== undefined){
			VerifyTestDatabaseName(persisted.DatabaseName);
		}

		const retVal = {
			host: process.env[`${prefix}_HOST`] || persisted.Host || defaults.host,
			port: Number(process.env[`${prefix}_PORT`] || persisted.Port || defaults.port),
			user: process.env[`${prefix}_USER`] || persisted.UserName || defaults.user,
			password: process.env[`${prefix}_PASSWORD`] ?? persisted.Password ?? defaults.password,
			name: databaseName,
			maintenanceName: database.maintenanceName,
		};

		return retVal;
	}


	/**
		The same folder ifile::CSystemLocationComp resolves for shared application data:
		<public folder>/<company>/<product>. The public folder is fixed per platform there.
	*/
	ResolveSettingsFolder(spec)
	{
		const publicFolder = process.platform === 'win32' ? 'C:/Users/Public' : '/Users/Shared';
		const settings = spec.settings;

		const retVal = path.join(publicFolder, settings.company, settings.product);

		return retVal;
	}


	/**
		A program of the build: the given path, else the newest one with that name under the bin search folders (those of
		the server by default, relative to the folder of suite.json; a folder with a ${ENV} variable that is not set is
		skipped).
	*/
	FindProgram(name, explicitPath, searchFolders = this.suite.server.binSearchFolders)
	{
		if (explicitPath){
			const retVal = path.resolve(explicitPath);
			if (!fs.existsSync(retVal)){
				throw new Error(`${retVal} not found: the executable of ${name}`);
			}

			return retVal;
		}

		const fileName = name + (process.platform === 'win32' ? '.exe' : '');
		const candidates = [];
		for (const searchFolder of searchFolders){
			const expandedFolder = ExpandEnvironment(searchFolder);
			if (expandedFolder === null){
				continue;
			}

			const root = path.resolve(this.suiteRoot, expandedFolder);
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
			throw new Error(`${fileName} not found under ${searchFolders.join(', ')}; build it or pass its path (--bin, --service-bin)`);
		}

		candidates.sort((first, second) => second.time - first.time);

		return candidates[0].path;
	}


	async EnsurePortsAreFree()
	{
		const ports = [
			...Object.values(this.suite.server.ports),
			...(this.suite.services ?? []).flatMap((service) => Object.values(service.ports)),
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
		return this.ResolveKitFile('sims', sim);
	}


	/**
		The script of a sim or of a setup step: one of the kit ("kit": "<name>.mjs" in suite.json, from the folder of that
		kind of the kit), else the file of the suite, relative to suite.json.
	*/
	ResolveKitFile(kitFolder, entry)
	{
		if (entry.kit){
			return path.join(path.dirname(fileURLToPath(import.meta.url)), '..', kitFolder, entry.kit);
		}

		return path.resolve(this.suiteRoot, entry.file);
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
	async DropDatabase(databaseSettings = this.databaseSettings)
	{
		const client = this.CreateDatabaseClient(databaseSettings.maintenanceName, databaseSettings);
		await client.connect();
		try {
			await client.query(
				'SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = $1 AND pid != pg_backend_pid()',
				[databaseSettings.name]
			);
			await client.query(`DROP DATABASE IF EXISTS ${client.escapeIdentifier(databaseSettings.name)}`);
		}
		finally {
			await client.end();
		}
	}


	/**
		Guards against a server that runs on another database than the test one, for example because its persisted
		settings name a different database: the tests would then change that database.
	*/
	async VerifyServerDatabase(databaseSettings = this.databaseSettings, timeoutSeconds = 0)
	{
		const deadline = Date.now() + timeoutSeconds * 1000;
		for (;;){
			try {
				await this.CheckServerDatabase(databaseSettings);

				return;
			}
			catch (error) {
				if (Date.now() >= deadline){
					throw error;
				}
			}

			await new Promise((resolve) => setTimeout(resolve, 1000));
		}
	}


	/**
		One check of the database. A service may open its port before it has created and migrated its database, so
		VerifyServerDatabase repeats it until the timeout; the server under test answers only after that.
	*/
	async CheckServerDatabase(databaseSettings)
	{
		const databaseName = databaseSettings.name;
		const client = this.CreateDatabaseClient(databaseName, databaseSettings);
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
	RemoveRunFiles(spec, settingsFolder)
	{
		for (const file of spec.settings?.runFiles ?? []){
			fs.rmSync(path.join(settingsFolder, file), { force: true });
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
		const serverProcess = this.LaunchProgram(this.suite.server, this.binary, 'server', this.serverStarts);
		serverProcess.once('exit', (code, signal) => {
			Log(`Server exited (code ${code}, signal ${signal})`);
			if (this.serverProcess === serverProcess){
				this.serverProcess = null;
			}
		});

		this.serverProcess = serverProcess;
	}


	/**
		Starts a program of the build, the server or a service, with its output in the results folder as <logName>-stdout.log
		and -stderr.log. The environment variables the setup steps passed on are added to its environment.
	*/
	LaunchProgram(spec, binary, logName, startNumber)
	{
		const [stdout, stderr] = this.GetOutputFiles(logName).map((name) => {
			const file = path.join(this.resultsDir, name);
			fs.appendFileSync(file, `[test-server] Server start ${startNumber}\n`);

			return fs.openSync(file, 'a');
		});

		/**
			Per platform, because the deployed Qt platform plugins differ: Windows has only qwindows, while Linux agents
			without a display need offscreen.
		*/
		const argumentsByPlatform = spec.arguments;
		const programArguments = argumentsByPlatform[process.platform] ?? argumentsByPlatform.default;

		/**
			A program kept for debugging must outlive the runner; on Windows a child that is not detached ends with it.
		*/
		const retVal = spawn(binary, programArguments, {
			cwd: path.dirname(binary),
			env: { ...process.env, ...this.serverEnvironment },
			stdio: ['ignore', stdout, stderr],
			detached: this.keepServer,
		});
		if (this.keepServer){
			retVal.unref();
		}

		return retVal;
	}


	/**
		Whether a program answers: its readiness request is answered with 200. A service without a request to ask is
		ready when it accepts connections.
	*/
	async IsServerAnswering(spec)
	{
		const readiness = spec.readiness;
		if (readiness.path === undefined){
			return await IsPortOpen(spec.ports.http);
		}

		try {
			const response = await fetch(`http://127.0.0.1:${spec.ports.http}${readiness.path}`, {
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
		await this.WaitForProgram(this.suite.server, () => this.serverProcess !== null);
	}


	async WaitForProgram(spec, isRunning)
	{
		const readiness = spec.readiness;
		const deadline = Date.now() + readiness.timeoutSeconds * 1000;
		while (Date.now() < deadline){
			if (!isRunning()){
				throw new Error(`${spec.name} exited before it became ready; see its logs in the results folder`);
			}

			if (await this.IsServerAnswering(spec)){
				return;
			}

			await new Promise((resolve) => setTimeout(resolve, 1000));
		}

		throw new Error(`${spec.name} did not answer within ${readiness.timeoutSeconds} s`);
	}


	async StopServer()
	{
		await this.StopProgram(this.serverProcess);
	}


	async StopProgram(program)
	{
		if (program === null){
			return;
		}

		const exited = new Promise((resolve) => program.once('exit', resolve));
		program.kill();

		const killTimer = setTimeout(() => program.kill('SIGKILL'), 10000);
		await exited;
		clearTimeout(killTimer);
	}


	CollectServerLogs()
	{
		const programs = [
			{ spec: this.suite.server, settingsFolder: this.settingsFolder },
			...this.services,
		];
		for (const program of programs){
			for (const logFile of program.spec.settings?.logFiles ?? []){
				const source = path.join(program.settingsFolder, logFile);
				if (fs.existsSync(source)){
					fs.copyFileSync(source, path.join(this.resultsDir, path.basename(logFile)));
				}
			}
		}
	}
}
