// `required property` of a delegate takes the row, as Qt sets it: `modelData` and `index` of a Repeater over an array, a role of a
// ListModel, also when a ListView gives the delegate another row. Outside of a delegate it is an ordinary property. A compiled
// application keeps `required` in the packed form of its documents.
// `node test/required-properties.mjs`
import { Engine } from '../src/engine.js';
import { packDocument, unpackDocument } from '../src/packed.js';
import { parseQmlDocument } from '../src/parser.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 200
	height: 100

	property alias arrayColumn: arrayColumn
	property alias list: list
	property var made: null

	Column {
		id: arrayColumn
		Repeater {
			model: [{name: "a"}, {name: "b"}]
			delegate: Item {
				id: arrayDelegate
				required property var modelData
				required property int index
				property string label: arrayDelegate.modelData.name + index
			}
		}
	}

	Column {
		Repeater {
			id: fileRepeater
			model: [{name: "x"}]
			delegate: Label {}
		}
	}

	ListModel {
		id: people
		ListElement { name: "Ann" }
		ListElement { name: "Bob" }
		ListElement { name: "Cid" }
		ListElement { name: "Dan" }
	}

	ListView {
		id: list
		width: 100
		height: 40
		reuseItems: true
		model: people
		delegate: Item {
			required property string name
			required property int index
			property string label: name + index
			width: 100
			height: 20
		}
	}

	Component {
		id: plainComponent
		Item {
			required property var modelData
		}
	}

	property alias fileRepeater: fileRepeater

	property alias people: people

	Component.onCompleted: {
		for (let i = 4; i < 40; i++) people.append({name: "P" + i})
		made = plainComponent.createObject(root, {modelData: "given"})
	}
}
`,
	'Label.qml': `
import QtQuick

Item {
	required property var modelData
	property string label: modelData.name
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
let time = 0;
const frame = () => {
	for (let i = 0; i < 4; i++) scene.drawFrame((time += 0.016));
};
frame();
const root = viewer.content.$children[0];

const labels = (column) => column.$children.filter((child) => child.label !== undefined).map((child) => child.label);
check('a Repeater over an array gives modelData and index', labels(root.arrayColumn), ['a0', 'b1']);
check('a delegate of a component file gets modelData too', root.fileRepeater.itemAt(0).label, 'x');

// Every delegate shows its own row; the ListView makes a few more than it shows (cacheBuffer)
const listDelegates = () => root.list.contentItem.$children.filter((child) => child.label !== undefined && child.visible);
const rowsMatch = () => listDelegates().every((child) => child.label === root.people.get(child.index).name + child.index);
const shown = () => listDelegates().map((child) => child.label);
const firstDelegates = new Set(listDelegates());
check('a ListView gives the roles of its ListModel', rowsMatch() && shown().includes('Ann0'), true);
root.list.contentY = 600;
frame();
check('the ListView reuses its delegates', listDelegates().some((child) => firstDelegates.has(child)), true);
check('a reused delegate takes the role of its new row', rowsMatch() && shown().includes('P3030'), true);

check('outside of a delegate the property is set by createObject', root.made.modelData, 'given');
check('no errors', engine.errors, []);

const parsed = parseQmlDocument(FILES['Label.qml']);
const unpacked = unpackDocument(JSON.parse(JSON.stringify(packDocument(parsed))));
const declarations = (def) => def.properties.filter((prop) => prop.isDeclaration).map((prop) => [prop.name, prop.isRequired]);
check('the packed document keeps required', declarations(unpacked), [['modelData', true], ['label', false]]);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
