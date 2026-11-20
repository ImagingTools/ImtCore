// A module imported with an alias (`import Qt.labs.platform 1.1 as Platform`): `Platform.FileDialog` is the FileDialog of the
// engine and `Platform.FileDialog.OpenFiles` its enumeration, as without the alias.
// `node test/module-alias.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import Qt.labs.platform 1.1 as Platform

Item {
	width: 200
	height: 100

	property alias dialog: dialog
	property int saveMode: Platform.FileDialog.SaveFile

	Platform.FileDialog {
		id: dialog
		fileMode: Platform.FileDialog.OpenFiles
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
scene.drawFrame(0.016);
const root = viewer.content.$children[0];

check('Platform.FileDialog is the FileDialog of the engine', root.dialog.$typeName, 'FileDialog');
check('it has the methods of a dialog', typeof root.dialog.open, 'function');
check('Platform.FileDialog.OpenFiles', root.dialog.fileMode, 1);
check('Platform.FileDialog.SaveFile', root.saveMode, 2);
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
