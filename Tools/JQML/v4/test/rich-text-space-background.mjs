// The spaces of rich text and their background, as desktop Qt 6.11 gives them (measured with
// test/qt-measure/rich-space-background/Main.qml: the blue columns of a span with background-color and the content width).
//   - the background covers a space inside the text, at the end of a line of the block and of a span
//   - one space at the end of a block that no <br/> or wrap ended is kept: it is part of the content width (the block cursor of the
//     terminal of Agentino is a span with a background that holds one space at the end of the text)
//   - a space before a <br/> or at the start of a block is dropped, a span of only spaces gives nothing, several spaces are one
// `node test/rich-text-space-background.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const BG = 'style="background-color:#0000ff"';
// [name, text, content width in letters, the quads of the background as [first letter, letters]]
const CASES = [
	['a space at the end of a span after a word', `AB<span ${BG}> </span>`, 3, [[2, 1]]],
	['two spaces at the end are one', `AB<span ${BG}>  </span>`, 3, [[2, 1]]],
	['a space in the middle', `A<span ${BG}> </span>B`, 3, [[1, 1]]],
	['a span over a word with a space', `<span ${BG}>A B</span>`, 3, [[0, 1], [1, 1], [2, 1]]],
	['a span over a word and a space', `<span ${BG}>AB </span>`, 3, [[0, 2], [2, 1]]],
	['a space at the start is dropped', `<span ${BG}> </span>AB`, 2, []],
	['a span of a space only gives nothing', `<span ${BG}> </span>`, 0, []],
	['a no-break space at the end', `AB<span ${BG}>&nbsp;</span>`, 3, [[2, 1]]],
	['white-space: pre', `<span style="white-space:pre">AB<span ${BG}> </span></span>`, 3, [[2, 1]]],
	['a space after a coloured span', `<span style="color:#ff0000">AB</span><span ${BG.slice(0, -1)};color:#ffffff"> </span>`, 3, [[2, 1]]],
	['a plain space at the end', 'AB ', 3, []],
	['a space before a break is dropped', 'AB <br/>CD', 2, []],
	['a space at the end of the first block is kept', '<p>AB </p><p>C</p>', 3, []],
	['a space at the end of a span before a break', `AB<span ${BG}> </span><br/>CD`, 2, []],
];

const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 300
	height: 300
${CASES.map(([, text]) => `\tText { textFormat: Text.RichText; font.pixelSize: 16; text: ${JSON.stringify(text)} }`).join('\n')}
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
scene.resize(300, 300, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 6; i++) scene.drawFrame(i * 0.016);
const texts = viewer.content.$children[0].$children.filter((item) => item.$typeName === 'Text');
check('every text was made', texts.length, CASES.length);

// the font of the test is the box font: every letter is as wide as the pixel size
const LETTER = 16;
CASES.forEach(([name, , width, quads], index) => {
	const text = texts[index];
	const found = [];
	const renderer = { pack: (color) => color, fillQuad: (x, y, w) => found.push([Math.round(x / LETTER), Math.round(w / LETTER)]), glyph() {}, translate() {} };
	text.$paintRich(renderer, 1);
	check(`${name}: content width`, Math.round(text.contentWidth / LETTER), width);
	check(`${name}: background`, found, quads);
});

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
