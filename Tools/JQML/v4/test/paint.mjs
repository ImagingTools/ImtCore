// How rectangles are painted: a border around a see-through fill must stay a ring.
// `node test/paint.mjs`
import { CanvasContext } from '../src/canvas.js';
import { DropShadow, drawRectangle, Rectangle } from '../src/items.js';
import { parseColor } from '../src/util.js';

const OPAQUE = 0xff0000ff;
const CLEAR = 0x00000000;
const HALF = 0x800000ff;
const BORDER = 0xff00ff00;

function paint(fill, border, radius, aa) {
	const shapes = [];
	const r = {
		fillQuad: (x, y, w, h, color) => shapes.push({ x, y, w, h, color }),
		fillQuadAA: (x, y, w, h, color) => shapes.push({ x, y, w, h, color }),
		fillPolygon: (points, color) => {
			const xs = points.filter((_, i) => i % 2 === 0);
			const ys = points.filter((_, i) => i % 2 === 1);
			shapes.push({ x: Math.min(...xs), y: Math.min(...ys), w: Math.max(...xs) - Math.min(...xs), h: Math.max(...ys) - Math.min(...ys), color });
		},
	};
	drawRectangle(r, fill, BORDER, border, radius, aa, 0, 0, 100, 40);
	return shapes;
}

const covers = (shapes, x, y, color) => shapes.some((s) => s.color === color && x >= s.x && x <= s.x + s.w && y >= s.y && y <= s.y + s.h);

let failures = 0;
function check(name, actual, expected) {
	const same = actual === expected;
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${actual}, expected ${expected}`}`);
}

for (const [label, radius, aa] of [['rounded', 2, true], ['square', 0, false], ['square antialiased', 0, true], ['large radius', 12, true]]) {
	const clear = paint(CLEAR, 1, radius, aa);
	check(`${label}: a see-through fill leaves the middle unpainted`, covers(clear, 50, 20, BORDER) || covers(clear, 50, 20, CLEAR), false);
	check(`${label}: and the border is drawn along the top edge`, covers(clear, 50, 0.5, BORDER), true);
	check(`${label}: and along the left edge`, covers(clear, 0.5, 20, BORDER), true);
	const half = paint(HALF, 1, radius, aa);
	check(`${label}: a half see-through fill is drawn inside the border only`, covers(half, 50, 20, HALF) && !covers(half, 50, 20, BORDER), true);
	const solid = paint(OPAQUE, 1, radius, aa);
	check(`${label}: an opaque fill is drawn`, covers(solid, 50, 20, OPAQUE), true);
}

// A border is drawn once a visible color or a width of 1 or more is set (Rectangle.border is a pen).
function borderQuads(setup) {
	const rect = new Rectangle();
	rect.$aw = 100;
	rect.$ah = 40;
	setup(rect);
	let quads = 0;
	const r = { pack: (color) => color, fillQuad: () => quads++, fillQuadAA: () => quads++, fillPolygon: () => quads++ };
	rect.$paint(r, 1);
	return quads;
}
check('a rectangle without a border property has none', borderQuads(() => {}), 1);
check('a border color alone draws a border', borderQuads((rect) => (rect.border.color = 'red')) > 1, true);
check('a border width alone draws a border', borderQuads((rect) => (rect.border.width = 3)) > 1, true);
check('a width of 0 draws none', borderQuads((rect) => {
	rect.border.color = 'red';
	rect.border.width = 0;
}), 1);
check('a transparent border color draws none', borderQuads((rect) => (rect.border.color = 'transparent')), 1);

// Qt's Context2D extensions roundedRect() and ellipse() add a closed sub-path that fill() paints.
function filledPolygon(draw) {
	const commands = [];
	const ctx = new CanvasContext(100, 50, commands);
	ctx.beginPath();
	draw(ctx);
	ctx.fill();
	return commands.filter((c) => c.k === 'poly').map((c) => Array.from(c.points));
}
// Even-odd ray cast against the recorded polygon.
function inside(points, x, y) {
	let hit = false;
	for (let i = 0, j = points.length - 2; i < points.length; j = i, i += 2) {
		const xi = points[i], yi = points[i + 1], xj = points[j], yj = points[j + 1];
		if (yi > y !== yj > y && x < ((xj - xi) * (y - yi)) / (yj - yi) + xi) hit = !hit;
	}
	return hit;
}
const rounded = filledPolygon((ctx) => ctx.roundedRect(10, 10, 80, 30, 8, 8));
check('roundedRect fills one sub-path', rounded.length, 1);
check('roundedRect covers the middle', inside(rounded[0], 50, 25), true);
check('roundedRect covers the straight edges', inside(rounded[0], 50, 10.5) && inside(rounded[0], 10.5, 25), true);
check('roundedRect leaves the corner unpainted', inside(rounded[0], 10.5, 10.5), false);
check('roundedRect stays inside its rectangle', Math.min(...rounded[0]) >= 10 && Math.max(...rounded[0].filter((_, i) => i % 2 === 0)) <= 90, true);
const clamped = filledPolygon((ctx) => ctx.roundedRect(10, 10, 80, 30, 100, 100));
check('roundedRect clamps the radii to half the sides', inside(clamped[0], 50, 25) && inside(clamped[0], 50, 10.5), true);
const square = filledPolygon((ctx) => ctx.roundedRect(10, 10, 80, 30, 0, 0));
check('roundedRect with a zero radius is a rectangle', inside(square[0], 10.5, 10.5), true);
const ellipse = filledPolygon((ctx) => ctx.ellipse(10, 10, 80, 30));
check('ellipse covers the middle but not the corner', inside(ellipse[0], 50, 25) && !inside(ellipse[0], 12, 12), true);

// Like Qt, a fillStyle/strokeStyle that is not a color leaves the style as it was.
const styled = new CanvasContext(10, 10, []);
styled.fillStyle = '#ff0000';
styled.strokeStyle = '#00ff00';
for (const bad of [undefined, null, 'nonsense', 42, {}]) {
	styled.fillStyle = bad;
	styled.strokeStyle = bad;
}
check('an invalid fillStyle is ignored', styled.fillStyle, '#ff0000');
check('an invalid strokeStyle is ignored', styled.strokeStyle, '#00ff00');
styled.fillStyle = 'blue';
check('a valid fillStyle still applies', styled.fillStyle, 'blue');

// A DropShadow: the outline of its source, moved by the offsets, softer towards the outside, and nothing beyond blur + offset.
{
	const source = new Rectangle();
	source.$radius = 6;
	const shadow = new DropShadow();
	shadow.$aw = 100;
	shadow.$ah = 40;
	shadow.$source = source;
	shadow.$color = parseColor('#80000000');
	shadow.$hOffset = 2;
	shadow.$vOffset = 2;
	shadow.$blur = 6;
	const shapes = [];
	const r = {
		pack: (color, opacity) => Math.round(color.a * opacity * 255),
		fillQuad: (x, y, w, h, alpha) => shapes.push({ x, y, w, h, alpha }),
		fillQuadAA: (x, y, w, h, alpha) => shapes.push({ x, y, w, h, alpha }),
		fillPolygon: (points, alpha) => {
			const xs = points.filter((_, i) => i % 2 === 0);
			const ys = points.filter((_, i) => i % 2 === 1);
			shapes.push({ x: Math.min(...xs), y: Math.min(...ys), w: Math.max(...xs) - Math.min(...xs), h: Math.max(...ys) - Math.min(...ys), alpha });
		},
	};
	source.$aw = 100;
	source.$ah = 40;
	shadow.$paint(r, 1);
	const over = shapes.pop();
	check('the source is painted over the shadow, in its box', [over.x, over.y, over.w, over.h, over.alpha].join(','), '0,0,100,40,255');
	const right = Math.max(...shapes.map((s) => s.x + s.w));
	const bottom = Math.max(...shapes.map((s) => s.y + s.h));
	const left = Math.min(...shapes.map((s) => s.x));
	check('the shadow reaches blur + offset beyond the right edge', Math.round(right), 108);
	check('and beyond the bottom edge', Math.round(bottom), 48);
	check('and not further to the left than blur - offset', Math.round(left), -4);
	check('and is the more opaque the nearer to the middle', shapes[shapes.length - 1].w < shapes[0].w, true);
	shadow.$color = parseColor('#00000000');
	const none = [];
	shadow.$paint({ pack: () => 0, fillQuad: () => none.push(1), fillPolygon: () => none.push(1) }, 1);
	check('a see-through colour paints nothing', none.length, 0);
}

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
