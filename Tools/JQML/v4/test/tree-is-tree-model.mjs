// `isTreeModel` of TreeItemModel, as in the C++ model: InputView of imtcontrols asks it for every key of a row, and without it the
// elements of a dialog (a combo box) get no model.
// `node test/tree-is-tree-model.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 300
	height: 200
	property alias view: list
	property alias source: rows
	property var failures: []

	TreeItemModel { id: commands }
	TreeItemModel { id: rows }

	ListView {
		id: list
		width: 300
		height: 200
		model: rows
		delegate: Loader {
			source: model.Source || ""
			onLoaded: {
				let keys = rows.getKeys(model.index)
				for (let i = 0; i < keys.length; i++) {
					if (item[keys[i]] === undefined) continue
					if (rows.isTreeModel(keys[i], model.index)) item[keys[i]] = rows.getTreeItemModel(keys[i], model.index)
					else item[keys[i]] = rows.getData(keys[i], model.index)
				}
			}
		}
	}

	Component.onCompleted: {
		commands.setData("name", "valveControl", 0)
		commands.setData("name", "timeSync", 1)
		rows.setData("modelRole", "CommandList", 0)
		rows.setData("Source", "Elem.qml", 0)
		rows.setData("comboModel", commands, 0)
		rows.setData("plain", "text", 0)
	}
}
`,
	'Elem.qml': `
import QtQuick

Item {
	property string modelRole: ""
	property var comboModel: 0
	property string plain: ""
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

const errors = [];
const warn = console.warn;
console.warn = (...args) => errors.push(args.join(' '));
const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(300, 200, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 8; i++) scene.drawFrame(i * 0.016);
console.warn = warn;

const root = viewer.content.$children[0];
const rows = root.source;
check('a nested model is a tree model', rows.isTreeModel('comboModel', 0), true);
check('a plain value is not', rows.isTreeModel('plain', 0), false);
check('a missing key is not', rows.isTreeModel('nothing', 0), false);
check('a missing row is not', rows.isTreeModel('comboModel', 5), false);

const element = [...root.view.$delegates.values()][0].item;
check('the element got the combo model', element.comboModel.getItemsCount(), 2);
check('the element got the plain value', element.plain, 'text');
check('the handler of the loader did not fail', errors.filter((text) => text.includes('onLoaded')), []);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
