// A function or an arrow as the whole handler (`Keys.onPressed: function(event) {...}`) is the handler for every kind of handler,
// not only for signals: Keys, Component.onCompleted and onDestruction, a change handler, Connections.
// `node test/handler-functions.mjs`
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
	width: 200
	height: 100

	property var log: []
	property int value: 0
	property alias loader: loader

	signal said(string word)

	function add(entry) {
		log = log.concat([entry])
	}

	Component.onCompleted: function() { root.add("completed") }
	onValueChanged: function() { root.add("value " + root.value) }

	Item {
		id: field
		focus: true
		Keys.onPressed: function(event) {
			if (event.text === "a") {
				root.add("pressed " + event.text)
				event.accepted = true
			}
		}
		Keys.onEscapePressed: (event) => root.add("escape")
	}

	Connections {
		target: root
		function onSaid(word) { root.add("function " + word) }
		onSaid: function(word) { root.add("handler " + word) }
	}

	Loader {
		id: loader
		sourceComponent: Item {
			Component.onDestruction: function() { root.add("destroyed") }
		}
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
let time = 0;
const frame = () => {
	for (let i = 0; i < 4; i++) scene.drawFrame((time += 0.016));
};
frame();
const root = viewer.content.$children[0];

check('Component.onCompleted: function', root.log.includes('completed'), true);
root.log = [];

check('Keys.onPressed: function(event) takes the key', scene.keyDown({ key: 'a', code: 'KeyA' }), true);
scene.keyDown({ key: 'Escape', code: 'Escape' });
check('Keys.onPressed and Keys.onEscapePressed run as functions', root.log, ['pressed a', 'escape']);
root.log = [];

root.value = 5;
frame();
check('a change handler as a function', root.log, ['value 5']);
root.log = [];

root.said('hi');
frame();
check('Connections: a function and a handler expression', root.log.slice().sort(), ['function hi', 'handler hi']);
root.log = [];

root.loader.active = false;
frame();
check('Component.onDestruction: function', root.log, ['destroyed']);
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
