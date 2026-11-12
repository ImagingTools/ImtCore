// The colors of `ctx.createLinearGradient()` fills against what desktop Qt gives (read with getImageData on a 100 x 100 canvas, pixel
// centres): stops, clamping before the first and after the last stop, no stops (black to white), a line of no length, a diagonal line,
// the transform at the time of the fill, strokes, and a stop with alpha.
// `node test/canvas-gradient.mjs`
import { CanvasContext, CanvasGradient } from '../src/canvas.js';

let failures = 0;
function check(name, actual, expected, tolerance = 3) {
	const same = actual.length === expected.length && actual.every((v, i) => Math.abs(v - expected[i]) <= tolerance);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

// The color [r, g, b, a] of the drawn commands at the centre of pixel (x, y): the last piece that covers it, interpolated over its triangles.
function pixel(commands, x, y) {
	const px = x + 0.5;
	const py = y + 0.5;
	let found = null;
	for (const c of commands) {
		if (c.k !== 'gpoly') continue;
		const m = c.m;
		const det = m[0] * m[3] - m[1] * m[2];
		// back from the canvas to the local space of the command
		const lx = (m[3] * (px - m[4]) - m[2] * (py - m[5])) / det;
		const ly = (-m[1] * (px - m[4]) + m[0] * (py - m[5])) / det;
		const n = c.points.length / 2;
		for (let i = 1; i + 1 < n; i++) {
			const [ax, ay, bx, by, cx, cy] = [c.points[0], c.points[1], c.points[2 * i], c.points[2 * i + 1], c.points[2 * i + 2], c.points[2 * i + 3]];
			const d = (by - cy) * (ax - cx) + (cx - bx) * (ay - cy);
			if (d === 0) continue;
			const u = ((by - cy) * (lx - cx) + (cx - bx) * (ly - cy)) / d;
			const v = ((cy - ay) * (lx - cx) + (ax - cx) * (ly - cy)) / d;
			const w = 1 - u - v;
			if (u < -1e-9 || v < -1e-9 || w < -1e-9) continue;
			const [ca, cb, cc] = [c.colors[0], c.colors[i], c.colors[i + 1]];
			found = ['R', 'G', 'B'].map((k) => u * ca[k] + v * cb[k] + w * cc[k]).concat(Math.round((u * ca.a + v * cb.a + w * cc.a) * 255));
		}
	}
	return found === null ? [0, 0, 0, 0] : found;
}

const stops = (g, list) => list.forEach(([p, color]) => g.addColorStop(p, color));
const fresh = () => {
	const commands = [];
	return { ctx: new CanvasContext(100, 100, commands), commands };
};

{
	const { ctx, commands } = fresh();
	const g = ctx.createLinearGradient(20, 0, 80, 0);
	stops(g, [[0, '#ff0000'], [0.5, '#00ff00'], [1, '#0000ff']]);
	ctx.fillStyle = g;
	ctx.fillRect(0, 0, 100, 10);
	check('x = 0, before the first stop', pixel(commands, 0, 5), [255, 0, 0, 255]);
	check('x = 19', pixel(commands, 19, 5), [255, 0, 0, 255]);
	check('x = 20', pixel(commands, 20, 5), [251, 4, 0, 255]);
	check('x = 25', pixel(commands, 25, 5), [208, 47, 0, 255]);
	check('x = 35', pixel(commands, 35, 5), [124, 131, 0, 255]);
	check('x = 50, the middle stop', pixel(commands, 50, 5), [0, 251, 4, 255]);
	check('x = 65', pixel(commands, 65, 5), [0, 124, 131, 255]);
	check('x = 80', pixel(commands, 80, 5), [0, 0, 255, 255]);
	check('x = 99, after the last stop', pixel(commands, 99, 5), [0, 0, 255, 255]);
}
{
	const { ctx, commands } = fresh();
	const g = ctx.createLinearGradient(0, 20, 0, 40);
	stops(g, [[0, 'rgba(255,0,0,0.0)'], [1, 'rgba(255,0,0,1.0)']]);
	ctx.fillStyle = g;
	ctx.fillRect(0, 20, 10, 20);
	check('alpha at y = 25', pixel(commands, 5, 25), [255, 0, 0, 70]);
	check('alpha at y = 35', pixel(commands, 5, 35), [255, 0, 0, 198]);
}
{
	const { ctx, commands } = fresh();
	ctx.fillStyle = ctx.createLinearGradient(0, 0, 100, 0);
	ctx.fillRect(0, 50, 100, 5);
	check('no stops: black to white', pixel(commands, 50, 52), [129, 129, 129, 255]);
}
{
	const { ctx, commands } = fresh();
	const g = ctx.createLinearGradient(0, 0, 100, 0);
	stops(g, [[0.4, '#ff0000'], [0.6, '#0000ff']]);
	ctx.fillStyle = g;
	ctx.fillRect(0, 60, 100, 5);
	check('stops that do not start at 0', [pixel(commands, 10, 62), pixel(commands, 50, 62), pixel(commands, 90, 62)].flat(), [255, 0, 0, 255, 122, 0, 133, 255, 0, 0, 255, 255]);
}
{
	const { ctx, commands } = fresh();
	const g = ctx.createLinearGradient(50, 0, 50, 0);
	stops(g, [[0, '#ff0000'], [1, '#0000ff']]);
	ctx.fillStyle = g;
	ctx.fillRect(0, 70, 100, 5);
	check('a line of no length is the first stop', [pixel(commands, 10, 72), pixel(commands, 90, 72)].flat(), [255, 0, 0, 255, 255, 0, 0, 255]);
}
{
	const { ctx, commands } = fresh();
	const g = ctx.createLinearGradient(0, 80, 20, 100);
	stops(g, [[0, '#000000'], [1, '#ffffff']]);
	ctx.fillStyle = g;
	ctx.fillRect(0, 80, 20, 20);
	check('a diagonal line', [pixel(commands, 1, 82), pixel(commands, 10, 90), pixel(commands, 18, 98), pixel(commands, 1, 98), pixel(commands, 18, 82)].flat(), [25, 25, 25, 255, 134, 134, 134, 255, 236, 236, 236, 255, 127, 127, 127, 255, 134, 134, 134, 255]);
}
{
	const { ctx, commands } = fresh();
	ctx.save();
	ctx.translate(50, 80);
	const g = ctx.createLinearGradient(0, 0, 20, 0);
	stops(g, [[0, '#000000'], [1, '#ffffff']]);
	ctx.fillStyle = g;
	ctx.fillRect(0, 0, 20, 10);
	ctx.restore();
	check('the coordinates of the context at the fill', [pixel(commands, 51, 82), pixel(commands, 60, 82), pixel(commands, 69, 82)].flat(), [19, 19, 19, 255, 134, 134, 134, 255, 249, 249, 249, 255]);
}
{
	const { ctx, commands } = fresh();
	const g = ctx.createLinearGradient(0, 0, 100, 0);
	stops(g, [[0, '#ff0000'], [1, '#0000ff']]);
	ctx.strokeStyle = g;
	ctx.lineWidth = 4;
	ctx.beginPath();
	ctx.moveTo(60, 50);
	ctx.lineTo(100, 50);
	ctx.stroke();
	check('a stroke', [pixel(commands, 65, 50), pixel(commands, 95, 50)].flat(), [88, 0, 167, 255, 11, 0, 244, 255]);
}
{
	const { ctx } = fresh();
	const g = ctx.createLinearGradient(0, 0, 1, 0);
	ctx.fillStyle = '#123456';
	ctx.save();
	ctx.fillStyle = g;
	check('the style is the gradient', [ctx.fillStyle === g && g instanceof CanvasGradient ? 1 : 0], [1], 0);
	ctx.restore();
	check('restore() brings the color back', [ctx.fillStyle === '#123456' ? 1 : 0], [1], 0);
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
