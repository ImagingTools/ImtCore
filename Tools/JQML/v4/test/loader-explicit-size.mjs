// A Loader that has a size of its own gives it to the item it loads, as in Qt: the item is resized after its bindings have run, so a
// `height` that the item binds to its content (the top panel of IotPlatform: 80 px in a Loader of 70) does not win over the Loader.
// `node test/loader-explicit-size.mjs`
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
	width: 400
	height: 300
	property alias bound: boundLoader
	property alias plain: plainLoader
	property alias again: againLoader
	property bool on: true
	property Component deco: Component {
		Item {
			height: 80
			width: 400
		}
	}

	Loader {
		id: boundLoader
		width: 400
		height: 70
		sourceComponent: Panel { decorator: root.deco }
	}
	Loader {
		id: plainLoader
		y: 100
		sourceComponent: Panel { decorator: root.deco }
	}
	// A Loader that is switched off and on again: its height is 0 while it is off and 70 when it is on (the top panel of the application).
	Loader {
		id: againLoader
		y: 200
		width: 400
		active: root.on
		height: !active ? 0 : 70
		sourceComponent: Panel { decorator: root.deco }
	}
}
`,
	'Panel.qml': `
import QtQuick

// Like ControlBase: the size is the size of a decorator, which is made when decorator is given (by a binding of the use), and
// a Binding gives the size of the control to the decorator.
FocusScope {
	id: panel
	property var decorator_: null
	property Component decorator
	width: decorator_ ? decorator_.width : 0
	height: decorator_ ? decorator_.height : 0

	onDecoratorChanged: {
		decorator_ = decorator.createObject(panel)
		bindHeight.target = decorator_
	}

	Binding {
		id: bindHeight
		property: "height"
		value: panel.height
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
scene.resize(400, 300, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n = 6) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame(t++ * 0.016);
	}
};
frames(20);
const root = viewer.content.$children[0];

check('a Loader of 70 px makes the item 70 px, not the 80 that it binds', root.bound.item.height, 70);
check('the Loader keeps its height', root.bound.height, 70);
check('a Loader without a size follows its item', root.plain.height, 80);
check('a Loader of 70 px makes its first item 70 px too', root.again.item.height, 70);
root.on = false;
frames(10);
root.on = true;
frames(20);
check('and the item that it loads again', root.again.item.height, 70);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
