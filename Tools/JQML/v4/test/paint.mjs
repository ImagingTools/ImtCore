// How rectangles are painted: a border around a see-through fill must stay a ring.
// `node test/paint.mjs`
import { CanvasContext } from '../src/canvas.js';
import { Canvas, DropShadow, drawRectangle, Rectangle } from '../src/items.js';
import { Renderer } from '../src/renderer.js';
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

// arc() goes clockwise unless `anticlockwise` is set. PaintedCorner of ImtCore fills the corner outside of a rounded corner:
// `arc(w, 0, h, 0, Math.PI / 2, true)`, then two lines; the quarter of the circle must stay empty.
function covered(draw, x, y) {
	const commands = [];
	const ctx = new CanvasContext(40, 40, commands);
	ctx.beginPath();
	draw(ctx);
	ctx.fill();
	return commands.some((c) => {
		if (c.k !== 'poly') return false;
		for (let i = 0; i + 2 < c.indices.length + 1; i += 3) {
			const [a, b, d] = [c.indices[i], c.indices[i + 1], c.indices[i + 2]].map((n) => [c.points[2 * n], c.points[2 * n + 1]]);
			const side = (p, q) => (q[0] - p[0]) * (y - p[1]) - (q[1] - p[1]) * (x - p[0]);
			const s = [side(a, b), side(b, d), side(d, a)];
			if (s.every((v) => v >= 0) || s.every((v) => v <= 0)) return true;
		}
		return false;
	});
}
const corner = (ctx) => {
	ctx.arc(40, 0, 40, 0, Math.PI / 2, true);
	ctx.lineTo(0, 40);
	ctx.lineTo(0, 0);
};
check('the corner outside of the rounded corner is filled', covered(corner, 2, 38) && covered(corner, 15, 38), true);
check('the quarter of the circle stays empty', covered(corner, 20, 20) || covered(corner, 5, 5) || covered(corner, 38, 2), false);
const clockwise = filledPolygon((ctx) => ctx.arc(20, 20, 10, 0, Math.PI / 2));
check('a clockwise arc is a quarter of the circle', inside([...clockwise[0], 20, 20], 26, 26) && !inside([...clockwise[0], 20, 20], 12, 12), true);
const backwards = filledPolygon((ctx) => ctx.arc(20, 20, 10, Math.PI / 2, 0));
check('a clockwise arc from a larger angle to a smaller one goes round three quarters', Math.max(...backwards[0].filter((_, i) => i % 2 === 0)) > 29 && Math.min(...backwards[0].filter((_, i) => i % 2 === 0)) < 11, true);

// What a canvas drew beyond its size is not shown (the corner above goes round half a disc above the canvas), and a canvas that drew
// inside of its size is not clipped (a clip flushes the drawing).
function canvasClips(draw) {
	const canvas = new Canvas();
	canvas.$aw = 24;
	canvas.$ah = 24;
	canvas.$painted = true;
	canvas.$paintedWidth = 24;
	canvas.$paintedHeight = 24;
	const ctx = new CanvasContext(24, 24, canvas.$cmds);
	ctx.beginPath();
	draw(ctx);
	ctx.fill();
	let clips = 0;
	let drawn = 0;
	canvas.$paint(
		{
			ma: 1, mb: 0, mc: 0, md: 1, me: 0, mf: 0,
			setMatrix() {},
			translate() {},
			pack: () => 0,
			beginClip: () => ++clips,
			endClip: () => {},
			fillTriangles: () => drawn++,
			fillQuad: () => drawn++,
			fillQuadAA: () => drawn++,
		},
		1,
	);
	return `${clips},${drawn > 0}`;
}
check('a canvas that drew beyond its size is clipped', canvasClips((ctx) => {
	ctx.arc(24, 0, 24, 0, Math.PI / 2, true);
	ctx.lineTo(0, 24);
	ctx.lineTo(0, 0);
}), '1,true');
check('a canvas that drew inside of its size is not clipped', canvasClips((ctx) => ctx.rect(2, 2, 10, 10)), '0,true');

// A clip of an item turned by 180 degrees (PaintedCorner at the top right) is the same rectangle of the screen as without the turn.
{
	const r = new Renderer(null);
	r.flush = () => {};
	let scissor = null;
	r.setScissor = (...rect) => (scissor = rect.join(','));
	r.clipX0 = 0;
	r.clipY0 = 0;
	r.clipX1 = 1000;
	r.clipY1 = 1000;
	r.dpr = 1;
	r.setMatrix(1, 0, 0, 1, 100, 50);
	r.beginClip(24, 24);
	const plain = scissor;
	r.setMatrix(-1, 1.2e-16, -1.2e-16, -1, 124, 74);
	const turned = r.beginClip(24, 24);
	check('a clip of an item turned by 180 degrees is not empty', turned !== null, true);
	check('and is the rectangle of the item on the screen', scissor, plain);
}

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
