// `fileDialog.visible = true` opens the dialog (ExcelImportView of IotPlatform shows it so, not with open()). The browser file chooser
// is an <input type=file> that is clicked; choosing a file emits `accepted` and hides the dialog again, cancelling emits `rejected`.
// `node test/file-dialog-visible.mjs`
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
	property alias dialog: dialog
	property string chosen: ""
	property int rejectedCount: 0

	FileDialog {
		id: dialog
		fileMode: FileDialog.OpenFile
		nameFilters: ["Excel workbooks (*.xlsx)"]
		onAccepted: root.chosen = dialog.file.name
		onRejected: root.rejectedCount++
	}
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
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
scene.root.$updateRequested = true;
scene.drawFrame(0.016);
const root = viewer.content.$children[0];

// A page with one <input type=file> that records what is done with it.
const inputs = [];
// Installed after the scene exists: the renderer looks for a canvas in `document`.
globalThis.document = {
	createElement() {
		const listeners = {};
		const input = { type: '', accept: '', multiple: false, files: [], addEventListener: (name, fn) => (listeners[name] = fn), click: () => inputs.push({ input, listeners }) };
		return input;
	},
};

check('hidden at first', root.dialog.visible, false);
root.dialog.visible = true;
check('visible = true opens the chooser', inputs.length, 1);
check('the chooser takes the extensions of the name filters', inputs[0].input.accept, '.xlsx');
check('the dialog is shown', root.dialog.visible, true);
inputs[0].input.files = [{ name: 'book.xlsx' }];
inputs[0].listeners.change();
check('the chosen file is accepted', root.chosen, 'book.xlsx');
check('the dialog is hidden again', root.dialog.visible, false);
root.dialog.visible = true;
check('it can be shown again', inputs.length, 2);
inputs[1].listeners.cancel();
check('cancelling is a rejection', root.rejectedCount, 1);
check('and hides the dialog', root.dialog.visible, false);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
