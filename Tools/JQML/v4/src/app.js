// Browser glue: canvas, animation loop, mouse input, opening files, sample list.

import { Engine } from './engine.js';
import { attachCanvas } from './host.js';
import { HttpFS, MapFS, Project } from './project.js';
import { Renderer } from './renderer.js';
import { Scene } from './scene.js';
import { Viewer } from './viewer.js';

const config = window.qmlWebConfig || {};

// Relative paths (qml/...) are resolved against the page URL: `/gallery` without the slash would break them.
if (!location.pathname.endsWith('/') && !/\.\w+$/.test(location.pathname)) {
	location.replace(`${location.pathname}/${location.search}${location.hash}`);
}
const projectOptions = { importPaths: config.importPaths || [] };
const SAMPLES = config.samples || [
	'basic.qml', 'layout.qml', 'anchors_chain.qml', 'transforms.qml', 'fonts.qml', 'imports.qml',
	'js_functions.qml', 'js_advanced.qml', 'stars_1000.qml', 'many_items.qml', 'list_100k.qml', 'list_reuse.qml', 'text_100k.qml',
	'error.qml', 'import_error.qml',
].map((name) => `qml/${name}`);

const canvas = document.getElementById('view');
const renderer = new Renderer(canvas);
const engine = new Engine();
engine.lenient = Boolean(config.lenient);
engine.incremental = config.incremental !== false;
if (config.maxErrors) engine.maxErrors = config.maxErrors;
const scene = new Scene(renderer, engine);
const fileInput = document.getElementById('file');
const sampleSelect = document.getElementById('samples');

let loadToken = 0;

async function openProject(fs, mainPath) {
	const token = ++loadToken;
	clearLog();
	const project = new Project(fs, projectOptions);
	try {
		await project.load(mainPath);
	} catch (error) {
		if (token === loadToken) {
			viewer.showPath(mainPath);
			viewer.showError(error);
		}
		return;
	}
	if (token === loadToken) viewer.load(project);
}

const viewer = new Viewer(scene, engine, () => fileInput.click());

// ----- error log: a selectable text panel (the canvas text cannot be copied) -----

const logPanel = document.createElement('div');
logPanel.style.cssText = 'display:none;flex:none;max-height:35%;flex-direction:column;background:#101014;border-top:1px solid #3a3a46';
const logBar = document.createElement('div');
logBar.style.cssText = 'display:flex;gap:10px;align-items:center;padding:4px 10px;font:12px system-ui,sans-serif';
const logTitle = document.createElement('span');
logTitle.style.flex = '1';
const copyButton = document.createElement('button');
copyButton.textContent = 'Copy';
const clearButton = document.createElement('button');
clearButton.textContent = 'Clear';
const logText = document.createElement('pre');
logText.style.cssText = 'margin:0;padding:4px 10px 8px;overflow:auto;user-select:text;-webkit-user-select:text;cursor:text;color:#ff8c8c;font:12px ui-monospace,Menlo,monospace;white-space:pre-wrap;word-break:break-word';
logBar.append(logTitle, copyButton, clearButton);
logPanel.append(logBar, logText);
canvas.after(logPanel);

const logLines = [];
let loggedErrors = 0;

function renderLog() {
	logPanel.style.display = logLines.length > 0 ? 'flex' : 'none';
	logTitle.textContent = `Errors: ${logLines.length}`;
	logText.textContent = logLines.join('\n');
}

function addLog(text) {
	if (logLines.length < 1000) logLines.push(text);
	renderLog();
}

function clearLog() {
	logLines.length = 0;
	loggedErrors = engine.errors.length;
	renderLog();
}

copyButton.addEventListener('click', async () => {
	const text = logLines.join('\n');
	try {
		await navigator.clipboard.writeText(text);
	} catch {
		const range = document.createRange();
		range.selectNodeContents(logText);
		const selection = getSelection();
		selection.removeAllRanges();
		selection.addRange(range);
		document.execCommand('copy');
	}
	copyButton.textContent = 'Copied';
	setTimeout(() => (copyButton.textContent = 'Copy'), 1200);
});
clearButton.addEventListener('click', clearLog);

const showError = viewer.showError.bind(viewer);
viewer.showError = (error) => {
	showError(error);
	addLog(error instanceof Error ? error.message : String(error));
};

// The viewer shows only one error at a time; every error of the engine goes to the log.
function syncLog() {
	if (engine.errors.length < loggedErrors) loggedErrors = 0;
	while (loggedErrors < engine.errors.length) addLog(String(engine.errors[loggedErrors++]));
}

// ----- files -----

function pickMain(paths) {
	const qml = paths.filter((p) => p.toLowerCase().endsWith('.qml'));
	if (qml.length === 0) return null;
	const named = qml.find((p) => p.split('/').pop().toLowerCase() === 'main.qml');
	if (named) return named;
	return qml.sort((a, b) => a.split('/').length - b.split('/').length || a.localeCompare(b))[0];
}

function openFiles(files) {
	if (files.size === 0) return;
	const main = pickMain([...files.keys()]);
	if (main === null) {
		viewer.showError(new Error('No .qml file selected'));
		return;
	}
	openProject(new MapFS(files), main);
}

fileInput.addEventListener('change', () => {
	const files = new Map();
	for (const file of fileInput.files) files.set(file.webkitRelativePath || file.name, file);
	fileInput.value = '';
	openFiles(files);
});

function readEntries(reader) {
	return new Promise((resolve, reject) => reader.readEntries(resolve, reject));
}

async function collect(entry, out) {
	if (entry.isFile) {
		const file = await new Promise((resolve, reject) => entry.file(resolve, reject));
		out.set(entry.fullPath.replace(/^\//, ''), file);
	} else if (entry.isDirectory) {
		const reader = entry.createReader();
		for (;;) {
			const batch = await readEntries(reader);
			if (batch.length === 0) break;
			for (const child of batch) await collect(child, out);
		}
	}
}

window.addEventListener('dragover', (e) => e.preventDefault());
window.addEventListener('drop', async (e) => {
	e.preventDefault();
	const files = new Map();
	const entries = [...e.dataTransfer.items].map((item) => item.webkitGetAsEntry && item.webkitGetAsEntry()).filter(Boolean);
	if (entries.length > 0) {
		for (const entry of entries) await collect(entry, files);
	} else {
		for (const file of e.dataTransfer.files) files.set(file.name, file);
	}
	openFiles(files);
});

// ----- samples -----

function openSample(path) {
	sampleSelect.value = path;
	history.replaceState(null, '', `?file=${encodeURIComponent(path)}`);
	openProject(new HttpFS(), path);
}

for (const path of SAMPLES) {
	const option = document.createElement('option');
	option.value = path;
	option.textContent = path;
	sampleSelect.append(option);
}
sampleSelect.addEventListener('change', () => openSample(sampleSelect.value));

// ----- size, input, loop -----

attachCanvas(canvas, scene, engine, { afterFrame: syncLog });

const requested = new URLSearchParams(location.search).get('file');
openSample(requested || SAMPLES[0]);

window.qmlWeb = { engine, scene, viewer, renderer, openSample };
