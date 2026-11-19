// The easing curves of animations (`easing.type`, 14 uses in our QML) as QEasingCurve gives them: the numbers of Easing.*, the ends of every
// curve, the known middle values, the curves that overshoot (Back, Elastic) and the ones that do not, and an animation that runs along a curve.
// `node test/easing.mjs`
import { Engine } from '../src/engine.js';
import { easingValue } from '../src/items.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}
const near = (a, b) => Math.abs(a - b) < 1e-9;

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 100
	height: 100
	property alias box: box
	property alias eased: eased
	property alias plain: plain
	property var numbers: [Easing.Linear, Easing.InQuad, Easing.OutCubic, Easing.OutElastic, Easing.OutBack, Easing.OutBounce, Easing.BezierSpline]
	Item { id: box; x: 0; y: 0 }
	NumberAnimation { id: eased; target: box; property: "x"; from: 0; to: 100; duration: 1000; easing.type: Easing.OutCubic }
	NumberAnimation { id: plain; target: box; property: "y"; from: 0; to: 100; duration: 1000 }
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

// the ends of every curve: 0 at the start, 1 at the end
let ends = true;
for (let type = 0; type <= 45; type++) if (easingValue(type, 0) !== 0 || easingValue(type, 1) !== 1) ends = false;
check('every curve starts at 0 and ends at 1', ends, true);
check('the curve below 0 and above 1 is clamped', [easingValue(1, -1), easingValue(1, 2)], [0, 1]);

// the numbers of the middle value of the formulas of Qt
check('Linear', easingValue(0, 0.5), 0.5);
check('InQuad', easingValue(1, 0.5), 0.25);
check('OutQuad', easingValue(2, 0.5), 0.75);
check('InOutQuad at the middle and quarters', [easingValue(3, 0.5), easingValue(3, 0.25), easingValue(3, 0.75)], [0.5, 0.125, 0.875]);
check('OutCubic', easingValue(6, 0.5), 0.875);
check('InQuart', easingValue(9, 0.5), 0.0625);
check('OutQuint', near(easingValue(14, 0.5), 0.96875), true);
check('InSine', near(easingValue(17, 0.5), 1 - Math.cos(Math.PI / 4)), true);
check('InExpo', near(easingValue(21, 0.5), 2 ** -5 - 0.001), true);
check('OutCirc', near(easingValue(26, 0.5), Math.sqrt(0.75)), true);
check('OutBounce', near(easingValue(38, 0.5), 7.5625 * (0.5 - 1.5 / 2.75) ** 2 + 0.75), true);
check('InOutBounce is symmetric', near(easingValue(39, 0.5) + 0, 0.5), true);
check('OutInQuad meets in the middle', near(easingValue(4, 0.5), 0.5), true);

// overshoot
check('the Back curves leave the range', [easingValue(33, 0.2) < 0, easingValue(34, 0.8) > 1], [true, true]);
check('a bigger overshoot goes farther', easingValue(34, 0.8, 1, 3) > easingValue(34, 0.8, 1, 1.70158), true);
check('the Elastic curves leave the range', [easingValue(30, 0.2) > 1 || easingValue(30, 0.1) > 1, easingValue(29, 0.8) < 0 || easingValue(29, 0.9) < 0], [true, true]);
let inside = true;
for (const type of [1, 2, 3, 6, 10, 17, 18, 22, 26, 38]) for (let i = 1; i < 20; i++) if (easingValue(type, i / 20) < 0 || easingValue(type, i / 20) > 1) inside = false;
check('the other curves stay inside 0..1', inside, true);
check('the curves that are no function of the time are linear', [easingValue(41, 0.3), easingValue(45, 0.3)], [0.3, 0.3]);

// an animation runs along its curve
const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(100, 100, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let time = 0;
const advance = (seconds) => {
	const end = time + seconds;
	while (time < end) scene.drawFrame((time += 0.016));
};
advance(0.1);
const root = viewer.content.$children[0];
check('the numbers of Easing are those of Qt', root.numbers, [0, 1, 6, 30, 34, 38, 45]);
root.eased.start();
root.plain.start();
advance(0.5);
check('the animation with OutCubic is ahead of the linear one at the middle', root.box.x > root.box.y + 20, true);
check('and the linear one is at the middle', Math.abs(root.box.y - 50) < 5, true);
advance(1);
check('both end at the end value', [root.box.x, root.box.y], [100, 100]);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
