// A handler that a use site's assignment runs while the component is built (the card is made narrower than its literal width) assigns a
// property that still has its binding: the assignment replaces the binding, so the binding's first run does not undo it.
// `node test/narrow-signal.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'Card.qml': `
import QtQuick

Rectangle {
	id: card
	width: 1000
	height: 50
	property bool narrow: (width - 650) < 600
	property string trace: ""
	onNarrowChanged: { card.trace += "n" + card.narrow + ","; if (card.narrow) list.model = tabsB; else list.model = tabsA; }
	ListModel { id: tabsA; ListElement { name: "a" } }
	ListModel { id: tabsB; ListElement { name: "b" } ListElement { name: "c" } }
	ListView { id: list; model: tabsA; width: 10; height: 10 }
	property alias tabs: list
	Component.onCompleted: card.trace += "c,"
}
`,
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 400
	height: 200
	property alias card: holder.item
	Component {
		id: comp
		Card {
			width: root.width
		}
	}
	Loader {
		id: holder
		sourceComponent: comp
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

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 200, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 6; i++) {
	scene.root.$updateRequested = true;
	scene.drawFrame(i * 0.016);
}
const card = viewer.content.$children[0].card;
let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}
check('the change is reported before the completion', card.trace, 'ntrue,c,');
check('what the handler assigned to a bound property stays', card.tabs.count, 2);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
