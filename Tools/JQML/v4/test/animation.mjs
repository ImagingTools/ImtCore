// An animation of a hidden item must not force a redraw of the scene on every frame.
// `node test/animation.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const SOURCE = `
import QtQuick 2.15

Item {
	width: 400
	height: 300

	Item {
		id: page
		objectName: "page"
		visible: false

		Rectangle {
			id: bar
			objectName: "bar"
			width: 10
			height: 10

			NumberAnimation {
				id: slide
				objectName: "slide"
				target: bar
				property: "x"
				from: 0
				to: 100
				duration: 1000
			}
		}
	}
}
`;

class MemoryFS {
	async readText(path) {
		return path === 'main.qml' ? SOURCE : null;
	}

	async readBytes() {
		return null;
	}
}

const engine = new Engine();
engine.lenient = true; // animations exist only in this mode
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 300, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);

let time = 0;
const frame = (step = 0.016) => scene.drawFrame((time += step));
for (let i = 0; i < 4; i++) frame();

const find = (name, item = viewer.content) => {
	if (item.$objectName === name) return item;
	for (const child of item.$children) {
		const hit = find(name, child);
		if (hit !== null) return hit;
	}
	return null;
};
const page = find('page');
const bar = find('bar');
const slide = find('slide');

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

check('engine errors', engine.errors, []);

slide.start();
frame();
check('the animation runs', slide.running, true);
check('an animation of a hidden item does not request frames', engine.isAnimated, false);
frame(0.3);
check('the hidden item is not touched while the animation runs', bar.x, 0);
check('a frame of a hidden animation paints nothing', frame(0.1), false);

page.visible = true;
frame();
check('an animation of a visible item requests frames', engine.isAnimated, true);
frame(0.2);
check('the visible item is animated', bar.x > 0 && bar.x < 100, true);

page.visible = false;
check('hiding the item stops the frame requests', engine.isAnimated, false);

frame(2);
check('the animation finishes while hidden', slide.running, false);
check('the final state is written', bar.x, 100);


// An indicator on another page restarts itself for ever: its ticks must not repaint the whole scene.
const BUSY_SOURCE = `
import QtQuick 2.15

Item {
	width: 400
	height: 300

	Item {
		id: page
		visible: false
		property int cycles: 0

		Rectangle {
			id: spinner
			width: 10
			height: 10

			NumberAnimation {
				id: spin
				objectName: "spin"
				target: spinner
				property: "x"
				from: 0
				to: 10
				duration: 50
				onFinished: { page.cycles++; spin.start() }
			}
		}

		Timer {
			id: tick
			objectName: "tick"
			interval: 20
			repeat: true
			onTriggered: page.cycles++
		}

		PauseAnimation {
			id: hiddenPause
			objectName: "hiddenPause"
			duration: 100
			onFinished: tip.visible = false
		}
	}

	Text {
		id: label
		objectName: "label"
		text: "busy " + (page.cycles > 5)
	}

	Rectangle {
		id: tip
		objectName: "tip"
		width: 10
		height: 10
	}
}
`;

const busy = {};
busy.engine = new Engine();
busy.engine.lenient = true;
busy.scene = new Scene(new Renderer(null), busy.engine);
busy.viewer = new Viewer(busy.scene, busy.engine, () => {});
busy.scene.resize(400, 300, 1);
const busyProject = new Project({ readText: async (path) => (path === 'main.qml' ? BUSY_SOURCE : null), readBytes: async () => null });
await busyProject.load('main.qml');
busy.viewer.load(busyProject);
let busyTime = 0;
const busyFrame = (step = 0.016) => busy.scene.drawFrame((busyTime += step));
for (let i = 0; i < 4; i++) busyFrame();
const busyFind = (name, item = busy.viewer.content) => {
	if (item.$objectName === name) return item;
	for (const child of item.$children) {
		const hit = busyFind(name, child);
		if (hit !== null) return hit;
	}
	return null;
};
const label = busyFind('label');
const tip = busyFind('tip');

busyFind('spin').start();
busyFind('tick').start();
let painted = 0;
for (let i = 0; i < 120; i++) if (busyFrame()) painted++;
check('hidden animation and timer: engine errors', busy.engine.errors, []);
check('a hidden animation and a timer that keep ticking do not repaint every tick', painted <= 6, true);
check('what they changed still reaches a binding of a visible item', label.text === 'busy true', true);

busyFind('hiddenPause').start();
let hidTip = false;
for (let i = 0; i < 20 && !hidTip; i++) {
	const drawn = busyFrame();
	if (tip.visible === false) hidTip = drawn;
}
check('a visible item hidden by a handler of a hidden animation is repainted at once', hidTip, true);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
