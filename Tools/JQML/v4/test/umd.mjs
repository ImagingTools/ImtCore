// A JavaScript library of a qmldir that declares no top-level names (a UMD file such as moment.js) sets its own global:
// it runs when the application starts. And a TreeItemModel takes another model with setExternTreeModel.
// `node test/umd.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import iotjs 1.0

Item {
	property string fromGlobal: Provider.instance === undefined ? "none" : Provider.instance()
	property string stored: model.getTreeItemModel("Child", 0).getData("Name", 0)

	TreeItemModel { id: child }

	TreeItemModel {
		id: model
		Component.onCompleted: {
			child.setData("Name", "child", 0)
			setExternTreeModel("Child", child, 0)
		}
	}
}
`,
	'lib/qmldir': 'module iotjs\nLib 1.0 lib.js\nsingleton Provider 1.0 Provider.qml\n',
	'lib/Provider.qml': 'pragma Singleton\nimport QtQuick\nQtObject {\n\treadonly property var instance: typeof umdValue !== "undefined" ? umdValue : globalThis.umdValue\n}\n',
	'lib/lib.js': ';(function (global, factory) {\n\tglobal.umdValue = factory()\n}(this, (function () { return function () { return "umd loaded" } })))\n',
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
const project = new Project(new MemoryFS(), { modules: { iotjs: ['lib'] } });
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
const root = viewer.content.$children[0];

check('a library without top-level names sets its global', root.fromGlobal, 'umd loaded');
check('setExternTreeModel stores the other model', root.stored, 'child');

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
