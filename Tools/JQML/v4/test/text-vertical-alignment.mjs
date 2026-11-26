// Text.verticalAlignment, as desktop Qt 6.11 gives it (measured with test/qt-measure/text-valign/Main.qml: the rows of ink of a text in
// boxes that are higher, equal and lower than the text). What it shows:
//   - AlignTop (the default) keeps the text at the top padding
//   - AlignBottom moves the text down by all the room that is left under it, AlignVCenter by half of it, half rounded up
//   - the room is the height without topPadding and bottomPadding, less the height of the text; it is negative for a text higher than
//     the item, and the text is moved up then (no clamping)
//   - plain, wrapped, elided and rich text are aligned the same way
// `node test/text-vertical-alignment.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const TOP = 0x20;
const BOTTOM = 0x40;
const CENTER = 0x80;
// [name, properties, alignment]; the same boxes as in the measurement of Qt
const CASES = [
	['top, high box', 'height: 60', TOP],
	['center, high box', 'height: 60', CENTER],
	['bottom, high box', 'height: 60', BOTTOM],
	['default, high box', 'height: 60', null],
	['center, box as high as the text + 3', 'height: HEIGHT + 3', CENTER],
	['bottom, box lower than the text', 'height: 8', BOTTOM],
	['center, with paddings', 'height: 60; topPadding: 10; bottomPadding: 2', CENTER],
	['bottom, with paddings', 'height: 60; topPadding: 10; bottomPadding: 2', BOTTOM],
	['center, two lines', 'height: 60; text: "AAAA\\nAAAA"', CENTER],
	['center, elided', 'height: 60; width: 40; elide: Text.ElideMiddle', CENTER],
	['center, rich text', 'height: 60; textFormat: Text.RichText; text: "<b>AAAA</b>"', CENTER],
	['center, wrapped text higher than the box', 'height: 20; width: 30; wrapMode: Text.WordWrap; text: "AAAA AAAA AAAA"', CENTER],
];

const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 400
	height: 400
${CASES.map(([, props, align], index) => `\tText { id: t${index}; width: 200; text: "AAAA"; font.pixelSize: 16; ${props.replace('HEIGHT', '20')}${align === null ? '' : `; verticalAlignment: ${align}`} }`).join('\n')}
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
scene.resize(400, 400, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 6; i++) scene.drawFrame(i * 0.016);
const texts = viewer.content.$children[0].$children.filter((item) => item.$typeName === 'Text');
check('every text was made', texts.length, CASES.length);

// the rule of the measurement, with the height of the text of the engine's own font
CASES.forEach(([name, , align], index) => {
	const text = texts[index];
	const content = text.contentHeight;
	const padTop = text.topPadding;
	const padBottom = text.bottomPadding;
	const free = text.height - padTop - padBottom - content;
	const expected = align === BOTTOM ? free : align === CENTER ? Math.floor(free / 2 + 0.5) : 0;
	check(`${name}: shift`, text.$verticalShift(), expected);
	check(`${name}: verticalAlignment reads back`, text.verticalAlignment, align === null ? TOP : align);
});

// verticalAlignment by name and a change of it
const first = texts[0];
first.verticalAlignment = 'AlignBottom';
for (let i = 0; i < 3; i++) scene.drawFrame((10 + i) * 0.016);
check('AlignBottom by name', [first.verticalAlignment, first.$verticalShift() > 0], [BOTTOM, true]);
first.verticalAlignment = CENTER;
check('AlignVCenter by number', first.verticalAlignment, CENTER);

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
