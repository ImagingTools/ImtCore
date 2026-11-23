// `ctx.arcTo(x1, y1, x2, y2, radius)` against what desktop Qt paints (the samples are what Qt gave for the same path, read with getImageData):
// a rectangle whose four corners are made with arcTo is a rounded one (the corner pixel is empty, the pixel inside the arc is full), the
// path goes on from the second point of touch, a path with no current point starts at (x1, y1), and a zero radius or points on one line give
// a straight line.
// `node test/canvas-arc-to.mjs`
import { CanvasContext } from '../src/canvas.js';
import { rasterize } from './canvas-raster.mjs';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `\n      got      ${JSON.stringify(actual)}\n      expected ${JSON.stringify(expected)}`}`);
}
const painted = (commands, points) => {
	const image = rasterize(commands, 60, 60);
	return points.map(([x, y]) => (image.at(x, y)[3] > 128 ? 'F' : '.')).join(' ');
};
const newContext = () => {
	const commands = [];
	const ctx = new CanvasContext(60, 60, commands);
	ctx.fillStyle = '#000000';
	return { ctx, commands };
};

{
	// the rounded rectangle of 10..50 with the radius 15: Qt gave `. . . .`, `F F F F`, `F`, `. . . .`, `F F F F`
	const { ctx, commands } = newContext();
	ctx.beginPath();
	ctx.moveTo(30, 10);
	ctx.arcTo(50, 10, 50, 30, 15);
	ctx.arcTo(50, 50, 30, 50, 15);
	ctx.arcTo(10, 50, 10, 30, 15);
	ctx.arcTo(10, 10, 30, 10, 15);
	ctx.closePath();
	ctx.fill();
	check('the corners are cut', painted(commands, [[11, 11], [48, 11], [11, 48], [48, 48]]), '. . . .');
	check('inside the arcs it is filled', painted(commands, [[18, 18], [41, 18], [18, 41], [41, 41]]), 'F F F F');
	check('the centre is filled', painted(commands, [[30, 30]]), 'F');
	check('outside is empty', painted(commands, [[5, 30], [30, 5], [55, 30], [30, 55]]), '. . . .');
	check('the straight edges are filled', painted(commands, [[30, 11], [11, 30], [48, 30], [30, 48]]), 'F F F F');
}

{
	// a corner with a bigger radius than the sides allow still touches both lines
	const { ctx, commands } = newContext();
	ctx.beginPath();
	ctx.moveTo(10, 50);
	ctx.arcTo(10, 10, 50, 10, 20);
	ctx.lineTo(50, 50);
	ctx.closePath();
	ctx.fill();
	check('a corner of one arc: the cut corner is empty', painted(commands, [[11, 11]]), '.');
	check('and the rest is filled', painted(commands, [[30, 30], [45, 45]]), 'F F');
}

{
	// the current point after the call is the second point of touch: the next line starts there
	const { ctx } = newContext();
	ctx.beginPath();
	ctx.moveTo(0, 10);
	ctx.arcTo(30, 10, 30, 40, 10);
	check('the path goes on from the second point of touch', ctx.$currentPoint(), { x: 30, y: 20 });
}

{
	const { ctx } = newContext();
	ctx.beginPath();
	ctx.arcTo(20, 20, 40, 20, 5);
	check('with no current point the path starts at (x1, y1)', ctx.$currentPoint(), { x: 20, y: 20 });
	ctx.beginPath();
	ctx.moveTo(0, 0);
	ctx.arcTo(20, 0, 40, 0, 5);
	check('points on one line give a line to (x1, y1)', ctx.$currentPoint(), { x: 20, y: 0 });
	ctx.beginPath();
	ctx.moveTo(0, 0);
	ctx.arcTo(20, 0, 20, 20, 0);
	check('a zero radius gives a line to (x1, y1)', ctx.$currentPoint(), { x: 20, y: 0 });
	ctx.beginPath();
	ctx.moveTo(0, 0);
	ctx.arcTo(0, 0, 20, 20, 5);
	check('the same first point gives a line to it', ctx.$currentPoint(), { x: 0, y: 0 });
}

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
