// A module that has a type in QML under `Name__` (RemoteFileController of imtguigql: the qmldir keeps it from taking the place of the C++
// type of the same name on a desktop) gives that type to a page that uses `Name`: the browser has no C++ type, so the QML one is the type.
// A type that the module has under its own name is not replaced by it.
// `node test/cpp-twin-type.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import gui 1.0

Item {
	id: root
	property alias files: files
	property alias both: both
	property bool hasGet: typeof files.getFile === "function"
	property string got: ""

	FileController { id: files; prefix: "reports" }
	Both { id: both }

	Component.onCompleted: got = files.getFile("a", "b")
}
`,
	'lib/qmldir': 'module gui\nFileController__ 1.0 FileController.qml\nBoth 1.0 Both.qml\nBoth__ 1.0 BothTwin.qml\n',
	'lib/FileController.qml': 'import QtQuick\nQtObject {\n\tproperty string prefix\n\tfunction getFile(id, name) { return prefix + "/" + name + "?FileId=" + id }\n}\n',
	'lib/Both.qml': 'import QtQuick\nItem {\n\tproperty string which: "own"\n}\n',
	'lib/BothTwin.qml': 'import QtQuick\nItem {\n\tproperty string which: "twin"\n}\n',
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
const project = new Project(new MemoryFS(), { modules: { gui: ['lib'] } });
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
const root = viewer.content.$children[0];
check('the QML twin of a type is the type', root.hasGet, true);
check('and it works', root.got, "reports/b?FileId=a");
check('a type under its own name wins over its twin', root.both.which, "own");
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
