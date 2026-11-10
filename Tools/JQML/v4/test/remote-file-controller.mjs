// RemoteFileController, the analog of the C++ type of com.imtcore.imtqml for the browser (a class of the engine, like TreeItemModel):
// a page that uses it gets a type with the methods and the signals of the C++ one, whatever the qmldir of the module lists.
// The address of a file is `../../<prefix>/<name>`; a download opens it in a new tab, a delete sends DELETE.
// `node test/remote-file-controller.mjs`
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
	property var deleted: []
	property bool got: false

	RemoteFileController {
		id: files
		prefix: "reports"
		onFileDeleted: function (url) { root.deleted.push(url) }
	}

	function download(path) { got = files.getFile(path, path) }
}
`,
	// the module lists the QML file under another name, as imtguigql does: the type of the engine is the one that a page gets
	'lib/qmldir': 'module gui\nRemoteFileController__ 1.0 RemoteFileController.qml\n',
	'lib/RemoteFileController.qml': 'import QtQuick\nQtObject {\n\tproperty string prefix\n}\n',
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

// The browser: window.open and XMLHttpRequest that answer at once.
const opened = [];
globalThis.window = { open: (url, target) => opened.push([url, target]) };
const requests = [];
globalThis.XMLHttpRequest = class {
	static DONE = 4;
	open(method, url) {
		this.method = method;
		this.url = url;
	}

	send(body) {
		requests.push([this.method, this.url, body]);
		this.readyState = 4;
		this.status = 200;
		this.responseText = '{"ok":true}';
		setTimeout(() => this.onreadystatechange && this.onreadystatechange(), 0);
	}
};

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

check('the page gets the type of the engine', root.files.$typeName, 'RemoteFileController');
check('the prefix is what the page sets', root.files.prefix, 'reports');
root.download('28_08_2026/{id}/report.xlsx');
check('getFile reports that it started', root.got, true);
check('a download opens the address in a new tab', opened, [['../../reports/28_08_2026/{id}/report.xlsx?FileId=28_08_2026/{id}/report.xlsx', '_blank']]);

root.files.deleteFile('abc', 'file:///abc');
check('a delete is loading until the server answers', root.files.state, 'Loading');
await new Promise((resolve) => setTimeout(resolve, 20));
check('deleteFile sends DELETE to the file', requests, [['DELETE', '../../reports/abc', 'abc']]);
check('and the controller is ready', [root.files.state, root.files.json], ['Ready', '{"ok":true}']);
check('fileDeleted is emitted with the url', root.deleted, ['file:///abc']);

root.files.prefix = '';
root.files.getFile('x', 'x.txt');
check('without a prefix the address is in files', [opened[1][0], root.files.prefix], ['../../files/x.txt?FileId=x', 'files']);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
