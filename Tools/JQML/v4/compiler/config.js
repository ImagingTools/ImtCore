// The JSON config of a compilation, the same format as the config of the JQML v3 compiler of ImtCore:
//
//   { "dirs": [module directories with a qmldir], "includes": [other config files], "translation_dir": "..." }
//
// `${VAR}` is replaced by the environment variable before the JSON is parsed. Included configs are
// visited first; their `dirs` are put in front of the own ones. Paths are relative to the config that
// declares them.

import { existsSync, readdirSync, readFileSync, statSync } from 'node:fs';
import path from 'node:path';

export function envFill(source, env) {
	let result = source;
	if (result.indexOf('${') < 0) return result;
	const keys = Object.keys(env).sort((a, b) => b.length - a.length);
	for (const key of keys) {
		if (env[key] == null) continue;
		const token = '${' + key + '}';
		if (result.indexOf(token) < 0) continue;
		result = result.replaceAll(token, String(env[key]).replaceAll('\\', '\\\\').trim());
	}
	return result;
}

function envFillPath(filePath, env) {
	let result = String(filePath || '');
	if (result.indexOf('${') < 0) return result;
	const keys = Object.keys(env).sort((a, b) => b.length - a.length);
	for (const key of keys) {
		if (env[key] == null) continue;
		const token = '${' + key + '}';
		if (result.indexOf(token) < 0) continue;
		result = result.replaceAll(token, String(env[key]).trim());
	}
	return result;
}

function isAbsolutePath(filePath) {
	if (!filePath) return false;
	if (path.isAbsolute(filePath)) return true;
	if (/^[A-Za-z]:[\\/]/.test(filePath)) return true;
	return filePath.slice(0, 2) === '\\\\';
}

export function toPosix(filePath) {
	return String(filePath).replaceAll('\\', '/');
}

// A path of the config resolved against the directory of the config file.
export function resolveConfigRef(baseDirPath, filePath, env) {
	const filled = envFillPath(filePath, env);
	if (isAbsolutePath(filled)) return toPosix(path.normalize(filled));
	if (filled.indexOf('${') >= 0) return toPosix(path.normalize(filled));
	return toPosix(path.resolve(baseDirPath, filled));
}

function readJson(filePath, env) {
	let text;
	try {
		text = readFileSync(filePath, 'utf8');
	} catch (error) {
		throw new Error(`cannot read config '${filePath}': ${error.message}`);
	}
	try {
		return JSON.parse(envFill(text, env));
	} catch (error) {
		throw new Error(`config '${filePath}' is not valid JSON: ${error.message}`);
	}
}

// Returns { config, configDirPath, translationDirs }; config.dirs holds the directories of every included
// config followed by the own ones, resolved to absolute paths.
export function loadCompilerConfig(configFilePath, env = process.env) {
	const normalizedConfigPath = path.normalize(String(configFilePath || '').trim());
	const configDirPath = path.dirname(path.resolve(normalizedConfigPath));
	const translationDirs = [];

	const appendTranslationDir = (baseDirPath, translationDir) => {
		if (typeof translationDir !== 'string' || !translationDir.trim()) return;
		translationDirs.push(resolveConfigRef(baseDirPath, translationDir, env));
	};

	const includeFiles = (sourceFile, baseDirPath) => {
		if (!Array.isArray(sourceFile.includes)) return;
		for (const filePath of sourceFile.includes) {
			const absoluteConfigPath = resolveConfigRef(baseDirPath, filePath, env);
			const includeConfigDirPath = path.dirname(absoluteConfigPath);
			const file = readJson(absoluteConfigPath, env);
			includeFiles(file, includeConfigDirPath);
			for (const dirPath of file.dirs || []) {
				sourceFile.dirs.unshift(resolveConfigRef(includeConfigDirPath, dirPath, env));
			}
			appendTranslationDir(includeConfigDirPath, file.translation_dir);
		}
	};

	const config = readJson(normalizedConfigPath, env);
	if (!Array.isArray(config.dirs)) config.dirs = [];
	includeFiles(config, configDirPath);
	config.dirs = config.dirs.map((dirPath) => resolveConfigRef(configDirPath, dirPath, env));
	appendTranslationDir(configDirPath, config.translation_dir);
	return { config, configDirPath, translationDirs };
}

// Lines of a qmldir file: { module } | { className, version, file, singleton } | null.
export function parseQmldirLine(line) {
	const params = line.trim().split(' ');
	if (params.length === 2 && params[0] === 'module') return { module: params[1] };
	if (params.length === 3) return { className: params[0], version: parseFloat(params[1]), file: params[2], singleton: false };
	if (params.length === 4 && params[0] === 'singleton') return { className: params[1], version: parseFloat(params[2]), file: params[3], singleton: true };
	return null;
}

// The qmldir of a module directory: { dir, moduleName, entries: [{ className, version, file, singleton, path }] }.
export function readQmldir(dirPath) {
	const dir = toPosix(dirPath).replace(/\/+$/, '');
	const qmldirPath = `${dir}/qmldir`;
	let text;
	try {
		text = readFileSync(qmldirPath, 'utf8');
	} catch (error) {
		throw new Error(`cannot read '${qmldirPath}': ${error.message}`);
	}
	const lines = text.replaceAll('\r', '').replaceAll(/[ \t]+/g, ' ').split('\n');
	let moduleName = '';
	const entries = [];
	for (const line of lines) {
		const parsed = parseQmldirLine(line);
		if (parsed === null) continue;
		if (parsed.module !== undefined) {
			moduleName = parsed.module;
			continue;
		}
		const extension = parsed.file.split('.').pop();
		if (extension !== 'qml' && extension !== 'js') continue;
		entries.push({ ...parsed, path: toPosix(path.resolve(dir, parsed.file)) });
	}
	return { dir, moduleName, entries, text };
}

// The directory of the entry file given on the command line, relative to the config directory.
export function entryDirectory(configDirPath, entryPath) {
	return toPosix(path.dirname(path.resolve(configDirPath, String(entryPath))));
}

function rememberFile(files, seen, filePath) {
	const absolute = toPosix(path.resolve(filePath));
	const key = process.platform === 'win32' ? absolute.toLowerCase() : absolute;
	if (seen.has(key)) return;
	seen.add(key);
	files.push(absolute);
}

// The .qml files of every qmldir of the config and its includes; the .qml files of the entry directory
// are added when that directory is not a module itself (the same list as the JQML v3 compiler prints).
export function listQmldirQmlFiles(configPath, env = process.env, entryPath = '') {
	const { config, configDirPath } = loadCompilerConfig(configPath, env);
	const files = [];
	const seen = new Set();
	const entryDir = entryPath ? entryDirectory(configDirPath, entryPath) : '';
	let entryIsModule = false;

	for (const dirPath of config.dirs) {
		if (entryDir && dirPath === entryDir) entryIsModule = true;
		for (const entry of readQmldir(dirPath).entries) {
			if (entry.path.endsWith('.qml')) rememberFile(files, seen, entry.path);
		}
	}

	if (entryDir && !entryIsModule) {
		for (const name of readdirSync(entryDir)) {
			const filePath = `${entryDir}/${name}`;
			if (path.extname(name) !== '.qml' || !statSync(filePath).isFile()) continue;
			rememberFile(files, seen, filePath);
		}
	}
	return files;
}

export function directoryExists(dirPath) {
	return existsSync(dirPath) && statSync(dirPath).isDirectory();
}
