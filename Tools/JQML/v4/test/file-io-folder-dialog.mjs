// FileIO (imtqml::CFileIO: `source`, read(), write(data)) and FolderDialog (Qt.labs.platform), which the export / import dialogs, PathPicker
// and FilePickerElementView of ImtCore use. The browser has no file system, so:
//   FileIO.write(data)   saves the data as a download named after the last part of `source` and returns true; an empty source gives a default name
//   FileIO.read()        has no file to read: an empty string, no error
//   FolderDialog         opens the chooser of a folder (an <input type=file webkitdirectory>), like FileDialog does for a file: `visible = true` or open()
//                        shows it, choosing sets `folder` to the folder (file:///name) and emits accepted, cancelling emits rejected
// `node test/file-io-folder-dialog.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import Qt.labs.platform 1.1

Item {
	id: root
	width: 200
	height: 100
	property alias io: io
	property alias dialog: dialog
	property alias dialogBody: dialogBody
	property string chosen: ""
	property int rejectedCount: 0
	property int sourceChanges: 0

	FileIO {
		id: io
		onSourceChanged: root.sourceChanges++
	}
	FolderDialog {
		id: dialog
		onAccepted: root.chosen = dialog.folder.toString()
		onRejected: root.rejectedCount++
	}
	Item { id: dialogBody }
}
`,
};

class MemoryFS {
	async readText(path) {
		return path in FILES ? FILES[path] : null;
	}

	async readBytes() {
		return null;
	}
}

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(200, 100, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);

// A page that records the inputs that are clicked and the links that are made for a download.
const inputs = [];
const links = [];
const blobs = [];
globalThis.document = {
	createElement(tag) {
		const listeners = {};
		if (tag === 'a') {
			const link = { href: '', download: '', click: () => links.push({ href: link.href, download: link.download }), remove() {}, style: {} };
			return link;
		}
		const input = { type: '', accept: '', multiple: false, webkitdirectory: false, files: [], addEventListener: (name, fn) => (listeners[name] = fn), click: () => inputs.push({ input, listeners }) };
		return input;
	},
	body: { appendChild() {}, removeChild() {} },
};
globalThis.URL = Object.assign(globalThis.URL || {}, {
	createObjectURL: (blob) => {
		blobs.push(blob);
		return 'blob:test/' + blobs.length;
	},
	revokeObjectURL() {},
});

scene.root.$updateRequested = true;
scene.drawFrame(0.016);
const root = viewer.content.$children[0];
// a chooser that was never opened is a failed check, not the end of the test
const chooser = (i) => inputs[i] || { input: {}, listeners: { change() {}, cancel() {} } };
const safe = (fn) => {
	try {
		return fn();
	} catch {
		return undefined;
	}
};

// FileIO
check('FileIO is a known type', engine.unknownTypes.has('FileIO'), false);
check('FolderDialog is a known type', engine.unknownTypes.has('FolderDialog'), false);
check('source is empty at first', root.io.source, '');
root.io.source = 'export/report.xlsx';
check('source is kept', root.io.source, 'export/report.xlsx');
check('and sourceChanged is sent', root.sourceChanges, 1);
check('read has no file', safe(() => root.io.read()), '');
check('write returns true', safe(() => root.io.write('hello')), true);
check('and makes a download', links.length, 1);
check('named after the last part of the source', links[0] && links[0].download, 'report.xlsx');
check('with the data in it', blobs[0] && blobs[0].size, 5);
root.io.source = 'C:\data\other.json';
safe(() => root.io.write('{}'));
check('a path of Windows is cut the same way', links[1] && links[1].download, 'other.json');
root.io.source = '';
check('write with no source still saves', [safe(() => root.io.write('x')), links.length], [true, 3]);

// FolderDialog
check('hidden at first', safe(() => root.dialog.visible), false);
safe(() => root.dialog.open());
check('open() shows the chooser of a folder', inputs.length, 1);
check('it asks for a folder', inputs[0] && chooser(0).input.webkitdirectory, true);
check('the dialog is shown', safe(() => root.dialog.visible), true);
chooser(0).input.files = [{ name: 'a.txt', webkitRelativePath: 'Reports/2026/a.txt' }, { name: 'b.txt', webkitRelativePath: 'Reports/2026/b.txt' }];
chooser(0).listeners.change();
check('the chosen folder is the folder of the files', root.chosen, 'file:///Reports');
check('currentFolder follows it, as currentFile follows the file of FileDialog', safe(() => String(root.dialog.currentFolder)), 'file:///Reports');
check('the folder answers replace() with a path, as the QML of ImtCore uses it', safe(() => root.dialog.folder.toString().replace('file:///', '')), 'Reports');
check('the chooser takes no extensions and one choice', [chooser(0).input.accept, chooser(0).input.multiple], ['', false]);
check('the dialog is hidden again', safe(() => root.dialog.visible), false);
root.dialog.visible = true;
check('visible = true shows it again', inputs.length, 2);
chooser(1).listeners.cancel();
check('cancelling is a rejection', root.rejectedCount, 1);
check('and hides the dialog', safe(() => root.dialog.visible), false);
inputs.length = 0;
root.dialog.folder = 'file:///C:/Work';
check('a folder can be set from QML', String(root.dialog.folder), 'file:///C:/Work');

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
