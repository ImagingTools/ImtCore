// `ctx.arc()` against what desktop Qt draws (the painted samples of a filled pie in each quarter of the circle, read with getImageData):
// the direction, a sweep of a whole turn in either direction (a circle, not an arc of no length as in the HTML canvas), sweeps of more
// than a turn, and the nodes of a graph (DesignScheme.drawNode: `moveTo(x, y); arc(x, y, r, 0, 2 * Math.PI, true)` for every point of one
// path, one fill) that must be circles.
// `node test/canvas-arc.mjs`
import { CanvasContext } from '../src/canvas.js';
import { rasterize } from './canvas-raster.mjs';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `\n      got      ${JSON.stringify(actual)}\n      expected ${JSON.stringify(expected)}`}`);
}

const PI = Math.PI;
// [start, end, anticlockwise, painted samples of the top left, top right, bottom left, bottom right quarter] as Qt gives them: a quarter
// is full (about 75 to 83 samples), partly painted or empty.
const CASES = [
	[0, 2 * PI, true, 'F F F F'], [0, 2 * PI, false, 'F F F F'], [2 * PI, 0, true, 'F F F F'], [2 * PI, 0, false, 'F F F F'],
	[0, PI, true, 'F F . .'], [0, PI, false, '. . F F'], [PI, 0, true, '. . F F'], [PI, 0, false, 'F F . .'],
	[0, 3 * PI, true, '. . F F'], [0, 3 * PI, false, 'F F F F'], [0, -2 * PI, true, 'F F F F'], [0, -2 * PI, false, 'F F F F'],
	[0, PI / 2, true, 'F F F .'], [0, PI / 2, false, '. . . F'], [PI / 2, 0, true, '. . . F'], [PI / 2, 0, false, 'F F F .'],
	[0, 1.5 * PI, true, '. F . .'], [0, 0, true, '. . . .'], [0, 0, false, '. . . .'],
	[-PI, PI, true, 'F F F F'], [-PI, PI, false, 'F F F F'], [0, 4 * PI, true, 'F F F F'], [0, 4 * PI, false, 'F F F F'],
	[1, 1 + 2 * PI, true, 'F F F F'], [1, 1 + 2 * PI, false, 'F F F F'],
	[0, 2 * PI + 0.5, true, '. . . p'], [0, 2 * PI - 0.5, true, '. p . .'], [0, 2 * PI - 0.5, false, 'F p F F'],
];

// the painted samples of a quarter at the steps of 2 pixels, as the probe counted them: F full, p a part of it, . none
function quarters(commands) {
	const image = rasterize(commands, 100, 100);
	const counts = [0, 0, 0, 0];
	for (let y = 30; y < 70; y += 2) {
		for (let x = 30; x < 70; x += 2) {
			if (image.at(x, y)[3] > 128) counts[(y >= 50 ? 2 : 0) + (x >= 50 ? 1 : 0)]++;
		}
	}
	return counts.map((n) => (n >= 60 ? 'F' : n === 0 ? '.' : 'p')).join(' ');
}

for (const [start, end, anticlockwise, expected] of CASES) {
	const commands = [];
	const ctx = new CanvasContext(100, 100, commands);
	ctx.fillStyle = '#000000';
	ctx.beginPath();
	ctx.moveTo(50, 50);
	ctx.arc(50, 50, 20, start, end, anticlockwise);
	ctx.fill();
	check(`arc(${(start / PI).toFixed(2)}pi, ${(end / PI).toFixed(2)}pi, ${anticlockwise}): TL TR BL BR`, quarters(commands), expected);
}

{
	// the nodes of a graph: one path, a circle at every point, one fill
	const commands = [];
	const ctx = new CanvasContext(300, 100, commands);
	ctx.fillStyle = '#0066bb';
	ctx.beginPath();
	for (let i = 0; i < 12; i++) {
		const x = 20 + i * 22;
		ctx.moveTo(x, 50);
		ctx.arc(x, 50, 5, 0, 2 * PI, true);
	}
	ctx.stroke();
	ctx.fill();
	const image = rasterize(commands, 300, 100);
	let circles = 0;
	for (let i = 0; i < 12; i++) {
		const x = 20 + i * 22;
		// the centre, and the points at 4 pixels from it in the four directions, are inside of a circle of the radius 5
		const inside = [[0, 0], [4, 0], [-4, 0], [0, 4], [0, -4]].every(([dx, dy]) => image.at(x + dx, 50 + dy)[3] > 128);
		// and a point beyond its edge is not
		const outside = image.at(x + 7, 50)[3] === 0 && image.at(x, 50 + 7)[3] === 0;
		if (inside && outside) circles++;
	}
	check('twelve nodes of a graph are twelve filled circles', circles, 12);
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
