// Image.sourceSize with a picture that is loaded, as desktop Qt 6.11 gives it (the table was measured: test/qt-measure/image/Main.qml loads
// pic.png 40 x 20, tall.png 20 x 40 and pic.svg 40 x 20 with each request and prints what is read). What it shows:
//   - neither side asked for: sourceSize is the size of the picture; a side asked for is read back as it was asked (also 80 for a picture of 40)
//   - a picture made of pixels is only made smaller: one side keeps the proportions, both sides fit the picture into the box
//   - a vector (.svg) is made to the size asked for (one side keeps the proportions, both sides are exactly the box)
//   - with PreserveAspectFit or PreserveAspectCrop the picture covers the box (and may be made larger)
//   - the implicit size is that size; an explicit width or height wins for the item and the picture is painted into it
// `node test/image-source-size.mjs`
import { Engine } from '../src/engine.js';
import { hooks } from '../src/items.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const PICTURES = { 'pic.png': [40, 20], 'tall.png': [20, 40], 'pic.svg': [40, 20] };
// [name, source, the request [width, height], fillMode, the explicit size of the item, then what Qt read:
//  sourceSize, implicit size, size, painted size]; a request that is null was not written
const CASES = [
	["png plain", "pic.png", [null,null], 0, [null,null], [40,20], [40,20], [40,20], [40,20]],
	["png 20x10", "pic.png", [20,10], 0, [null,null], [20,10], [20,10], [20,10], [20,10]],
	["png w10", "pic.png", [10,null], 0, [null,null], [10,0], [10,5], [10,5], [10,5]],
	["png h10", "pic.png", [null,10], 0, [null,null], [0,10], [20,10], [20,10], [20,10]],
	["png 20x40 (ratio changed)", "pic.png", [20,40], 0, [null,null], [20,40], [20,10], [20,10], [20,10]],
	["png 100x10", "pic.png", [100,10], 0, [null,null], [100,10], [20,10], [20,10], [20,10]],
	["png 80x60 (bigger)", "pic.png", [80,60], 0, [null,null], [80,60], [40,20], [40,20], [40,20]],
	["png w80 (bigger)", "pic.png", [80,null], 0, [null,null], [80,0], [40,20], [40,20], [40,20]],
	["png h60 (bigger)", "pic.png", [null,60], 0, [null,null], [0,60], [40,20], [40,20], [40,20]],
	["png 30x30", "pic.png", [30,30], 0, [null,null], [30,30], [30,15], [30,15], [30,15]],
	["png 0x0", "pic.png", [0,0], 0, [null,null], [0,0], [40,20], [40,20], [40,20]],
	["png 20x10 fit", "pic.png", [20,10], 1, [null,null], [20,10], [20,10], [20,10], [20,10]],
	["png 30x30 fit", "pic.png", [30,30], 1, [null,null], [30,30], [60,30], [60,30], [60,30]],
	["png 30x30 crop", "pic.png", [30,30], 2, [null,null], [30,30], [60,30], [60,30], [60,30]],
	["png w10 fit", "pic.png", [10,null], 1, [null,null], [10,0], [10,5], [10,5], [10,5]],
	["tall plain", "tall.png", [null,null], 0, [null,null], [20,40], [20,40], [20,40], [20,40]],
	["tall w10", "tall.png", [10,null], 0, [null,null], [10,0], [10,20], [10,20], [10,20]],
	["tall 30x30", "tall.png", [30,30], 0, [null,null], [30,30], [15,30], [15,30], [15,30]],
	["svg plain", "pic.svg", [null,null], 0, [null,null], [40,20], [40,20], [40,20], [40,20]],
	["svg 20x10", "pic.svg", [20,10], 0, [null,null], [20,10], [20,10], [20,10], [20,10]],
	["svg 80x40 (bigger)", "pic.svg", [80,40], 0, [null,null], [80,40], [80,40], [80,40], [80,40]],
	["svg w80", "pic.svg", [80,null], 0, [null,null], [80,0], [80,40], [80,40], [80,40]],
	["svg h60", "pic.svg", [null,60], 0, [null,null], [0,60], [120,60], [120,60], [120,60]],
	["svg 30x30", "pic.svg", [30,30], 0, [null,null], [30,30], [30,30], [30,30], [30,30]],
	["svg 30x30 fit", "pic.svg", [30,30], 1, [null,null], [30,30], [60,30], [60,30], [60,30]],
	["svg 100x100 fit", "pic.svg", [100,100], 1, [null,null], [100,100], [200,100], [200,100], [200,100]],
	["svg width 50 only", "pic.svg", [null,null], 0, [50,null], [40,20], [40,20], [50,20], [50,20]],
	["svg width50 sourceSize 20x10", "pic.svg", [20,10], 0, [50,null], [20,10], [20,10], [50,10], [50,10]],
	["png explicit 100x100 + ss 20x10", "pic.png", [20,10], 0, [100,100], [20,10], [20,10], [100,100], [100,100]],
	["svg explicit 100x100 + ss 20x10 fit", "pic.svg", [20,10], 1, [100,100], [20,10], [20,10], [100,100], [100,50]],
];

const lines = [];
CASES.forEach(([, source, request, fill, explicit], index) => {
	const fills = ['', 'fillMode: Image.PreserveAspectFit', 'fillMode: Image.PreserveAspectCrop'];
	const props = [`source: "${source}"`];
	if (request[0] !== null) props.push(`sourceSize.width: ${request[0]}`);
	if (request[1] !== null) props.push(`sourceSize.height: ${request[1]}`);
	if (fill > 0) props.push(fills[fill]);
	if (explicit[0] !== null) props.push(`width: ${explicit[0]}`);
	if (explicit[1] !== null) props.push(`height: ${explicit[1]}`);
	lines.push(`\tImage { ${props.join('; ')} }`);
});
const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 400
	height: 400
${lines.join('\n')}
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
// the pictures that are asked for come with the sizes they have
hooks.loadImage = (image, source) => {
	const size = PICTURES[source];
	if (size === undefined) return;
	image.$setImage({ naturalWidth: size[0], naturalHeight: size[1] }, 1);
};
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 400, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 6; i++) scene.drawFrame(i * 0.016);
const images = viewer.content.$children[0].$children.filter((item) => item.$typeName === 'Image');
check('every picture was made', images.length, CASES.length);

CASES.forEach(([name, , , , , sourceSize, implicit, size, painted], index) => {
	const image = images[index];
	const got = [
		[image.sourceSize.width, image.sourceSize.height],
		[image.implicitWidth, image.implicitHeight],
		[image.width, image.height],
		[Math.round(image.paintedWidth), Math.round(image.paintedHeight)],
	];
	check(name + ': sourceSize, implicit, size, painted', got, [sourceSize, implicit, size, painted]);
});

// the request after the picture is loaded changes the implicit size, and the readers of sourceSize hear of the picture
const free = images[0];
free.sourceSize.width = 10;
for (let i = 0; i < 3; i++) scene.drawFrame((10 + i) * 0.016);
check('a request after the load: implicit size, size', [free.implicitWidth, free.implicitHeight, free.width, free.height], [10, 5, 10, 5]);
free.sourceSize = { width: 0, height: 0 };
for (let i = 0; i < 3; i++) scene.drawFrame((20 + i) * 0.016);
check('no request again: the size of the picture', [free.implicitWidth, free.implicitHeight], [40, 20]);

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
