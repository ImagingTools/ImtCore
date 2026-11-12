// Path recording and triangulation for the Canvas item. The 2D context records drawing commands
// (with the current transform baked into each), and `replayCommands` draws them through the
// renderer, so a Canvas is drawn directly by WebGL instead of through an intermediate bitmap.

import { fonts } from './fonts.js';
import { toColor, toFloat } from './util.js';

function cross(ox, oy, ax, ay, bx, by) {
	return (ax - ox) * (by - oy) - (ay - oy) * (bx - ox);
}

function insideTriangle(px, py, ax, ay, bx, by, cx, cy) {
	const d1 = cross(ax, ay, bx, by, px, py);
	const d2 = cross(bx, by, cx, cy, px, py);
	const d3 = cross(cx, cy, ax, ay, px, py);
	const hasNegative = d1 < 0 || d2 < 0 || d3 < 0;
	const hasPositive = d1 > 0 || d2 > 0 || d3 > 0;
	return !(hasNegative && hasPositive);
}

// Ear clipping triangulation of a simple (possibly concave) polygon given as [x0, y0, x1, y1, ...].
// Returns triangle vertex indices.
export function triangulate(p) {
	const count = p.length >> 1;
	const triangles = [];
	if (count < 3) {
		return triangles;
	}

	let area = 0;
	for (let i = 0; i < count; i++) {
		const j = (i + 1) % count;
		area += p[2 * i] * p[2 * j + 1] - p[2 * j] * p[2 * i + 1];
	}
	const orientation = area > 0 ? 1 : -1;

	const remaining = [];
	for (let i = 0; i < count; i++) remaining.push(i);

	while (remaining.length > 3) {
		let clipped = false;
		const m = remaining.length;
		for (let i = 0; i < m && !clipped; i++) {
			const prev = remaining[(i + m - 1) % m];
			const curr = remaining[i];
			const next = remaining[(i + 1) % m];
			const ax = p[2 * prev], ay = p[2 * prev + 1];
			const bx = p[2 * curr], by = p[2 * curr + 1];
			const cx = p[2 * next], cy = p[2 * next + 1];

			// Reflex or degenerate corners cannot be ears.
			if (cross(ax, ay, bx, by, cx, cy) * orientation <= 0) continue;

			let containsOther = false;
			for (let j = 0; j < m && !containsOther; j++) {
				const idx = remaining[j];
				if (idx === prev || idx === curr || idx === next) continue;
				const qx = p[2 * idx], qy = p[2 * idx + 1];
				if ((qx === ax && qy === ay) || (qx === bx && qy === by) || (qx === cx && qy === cy)) continue;
				containsOther = insideTriangle(qx, qy, ax, ay, bx, by, cx, cy);
			}
			if (containsOther) continue;

			triangles.push(prev, curr, next);
			remaining.splice(i, 1);
			clipped = true;
		}
		if (!clipped) {
			return triangles; // Self-intersecting or degenerate input.
		}
	}

	triangles.push(remaining[0], remaining[1], remaining[2]);
	return triangles;
}

// Do two segments share a point (they cross, touch or overlap)?
function segmentsTouch(ax, ay, bx, by, cx, cy, dx, dy) {
	const eps = 1e-7;
	const side = (px, py, qx, qy, rx, ry) => {
		const v = cross(px, py, qx, qy, rx, ry);
		return Math.abs(v) <= eps ? 0 : v > 0 ? 1 : -1;
	};
	const onSegment = (px, py, qx, qy, rx, ry) => Math.min(px, qx) - eps <= rx && rx <= Math.max(px, qx) + eps && Math.min(py, qy) - eps <= ry && ry <= Math.max(py, qy) + eps;
	const d1 = side(cx, cy, dx, dy, ax, ay);
	const d2 = side(cx, cy, dx, dy, bx, by);
	const d3 = side(ax, ay, bx, by, cx, cy);
	const d4 = side(ax, ay, bx, by, dx, dy);
	if (d1 !== d2 && d3 !== d4 && d1 * d2 <= 0 && d3 * d4 <= 0) return true;
	return (d1 === 0 && onSegment(cx, cy, dx, dy, ax, ay)) || (d2 === 0 && onSegment(cx, cy, dx, dy, bx, by)) || (d3 === 0 && onSegment(ax, ay, bx, by, cx, cy)) || (d4 === 0 && onSegment(ax, ay, bx, by, dx, dy));
}

// A polygon whose outline crosses or touches itself (a figure eight, `arc(...)` followed by lines back through the start)
// cannot be ear clipped. Long outlines are taken for simple: the test is quadratic.
function isSimplePolygon(p) {
	const n = p.length >> 1;
	if (n > 600) return true;
	for (let i = 0; i < n; i++) {
		const b = (i + 1) % n;
		for (let j = i + 1; j < n; j++) {
			const d = (j + 1) % n;
			if (b === j || d === i) continue;
			if (segmentsTouch(p[2 * i], p[2 * i + 1], p[2 * b], p[2 * b + 1], p[2 * j], p[2 * j + 1], p[2 * d], p[2 * d + 1])) return false;
		}
	}
	return true;
}

// The area covered by a self-crossing outline (non-zero winding, as for a canvas fill), as trapezoids between the
// y values where an edge starts, ends or crosses another one.
function windingMesh(p) {
	const n = p.length >> 1;
	const edges = [];
	for (let i = 0; i < n; i++) {
		const j = (i + 1) % n;
		const x0 = p[2 * i], y0 = p[2 * i + 1], x1 = p[2 * j], y1 = p[2 * j + 1];
		if (y0 === y1) continue;
		edges.push(y0 < y1 ? { x0, y0, x1, y1, dir: 1 } : { x0: x1, y0: y1, x1: x0, y1: y0, dir: -1 });
	}
	const xAt = (e, y) => e.x0 + ((e.x1 - e.x0) * (y - e.y0)) / (e.y1 - e.y0);
	const ys = new Set();
	for (const e of edges) {
		ys.add(e.y0);
		ys.add(e.y1);
	}
	for (let i = 0; i < edges.length; i++) {
		for (let j = i + 1; j < edges.length; j++) {
			const a = edges[i];
			const b = edges[j];
			const ax = a.x1 - a.x0, ay = a.y1 - a.y0, bx = b.x1 - b.x0, by = b.y1 - b.y0;
			const den = ax * by - ay * bx;
			if (Math.abs(den) < 1e-12) continue;
			const t = ((b.x0 - a.x0) * by - (b.y0 - a.y0) * bx) / den;
			const u = ((b.x0 - a.x0) * ay - (b.y0 - a.y0) * ax) / den;
			if (t > 0 && t < 1 && u > 0 && u < 1) ys.add(a.y0 + t * ay);
		}
	}
	const levels = [...ys].sort((a, b) => a - b);
	const points = [];
	const indices = [];
	for (let k = 0; k + 1 < levels.length; k++) {
		const top = levels[k];
		const bottom = levels[k + 1];
		if (bottom - top < 1e-9) continue;
		const mid = (top + bottom) / 2;
		const active = edges.filter((e) => e.y0 <= mid && e.y1 > mid).sort((a, b) => xAt(a, mid) - xAt(b, mid));
		let winding = 0;
		let left = null;
		for (const e of active) {
			const before = winding;
			winding += e.dir;
			if (before === 0 && winding !== 0) left = e;
			else if (before !== 0 && winding === 0) {
				const first = points.length >> 1;
				points.push(xAt(left, top), top, xAt(e, top), top, xAt(e, bottom), bottom, xAt(left, bottom), bottom);
				indices.push(first, first + 1, first + 2, first, first + 2, first + 3);
			}
		}
	}
	return { points, indices };
}

// What a fill of one sub-path draws: its own points when the outline is simple, else the area it winds around.
export function fillMesh(path) {
	// The arcs of a rounded rectangle end where the next one starts: a point twice in a row is one point.
	let p = path;
	for (let i = 2; i < p.length; i += 2) {
		if (Math.abs(p[i] - p[i - 2]) < 1e-9 && Math.abs(p[i + 1] - p[i - 1]) < 1e-9) {
			if (p === path) p = Array.from(path);
			p.splice(i, 2);
			i -= 2;
		}
	}
	const last = p.length - 2;
	if (last >= 2 && Math.abs(p[last] - p[0]) < 1e-9 && Math.abs(p[last + 1] - p[1]) < 1e-9) {
		if (p === path) p = Array.from(path);
		p.length = last;
	}
	if (isSimplePolygon(p)) return { points: p, indices: triangulate(p) };
	return windingMesh(p);
}

// A CSS font shorthand such as `italic bold 14px "Arial"` -> { family, size, bold, italic }.
function parseFont(css) {
	const m = /(?:^|\s)(\d*\.?\d+)(px|pt)\s+(.+)$/.exec(css);
	if (m === null) return null;
	const head = css.slice(0, m.index).toLowerCase();
	let size = parseFloat(m[1]);
	if (m[2] === 'pt') size = (size * 96) / 72;
	return {
		family: m[3].trim(),
		size,
		bold: /\bbold\b|\b[6-9]00\b/.test(head),
		italic: /\b(italic|oblique)\b/.test(head),
	};
}

// The color of a fillStyle/strokeStyle value, or null when it is not one.
function styleColor(value) {
	if (typeof value !== 'string') return null;
	try {
		return toColor(value);
	} catch {
		return null;
	}
}

function withAlpha(color, alpha) {
	return alpha === 1 ? color : { ...color, a: color.a * alpha };
}

function mixColors(a, b, f) {
	return { R: Math.round(a.R + (b.R - a.R) * f), G: Math.round(a.G + (b.G - a.G) * f), B: Math.round(a.B + (b.B - a.B) * f), a: a.a + (b.a - a.a) * f };
}

// The result of `ctx.createLinearGradient(x0, y0, x1, y1)`: colors along the line from (x0, y0) to (x1, y1), in the coordinates the
// context has when the gradient is used; before the first stop and after the last one the color of that stop goes on.
export class CanvasGradient {
	constructor(x0, y0, x1, y1) {
		this.$x0 = x0;
		this.$y0 = y0;
		this.$dx = x1 - x0;
		this.$dy = y1 - y0;
		this.$stops = [];
	}

	// A position outside of 0..1 or a value that is not a color is ignored.
	addColorStop(position, color) {
		const p = toFloat(position);
		const c = styleColor(color);
		if (c === null || !(p >= 0 && p <= 1)) return;
		let at = this.$stops.length;
		while (at > 0 && this.$stops[at - 1].p > p) at--;
		this.$stops.splice(at, 0, { p, color: c });
	}

	// Without stops Qt paints black to white.
	$list() {
		return this.$stops.length > 0 ? this.$stops : [{ p: 0, color: toColor('#000000') }, { p: 1, color: toColor('#ffffff') }];
	}

	// The position of a point along the line, where the line is 0 at its start and 1 at its end. A line of no length is 0 everywhere.
	$t(x, y) {
		const len2 = this.$dx * this.$dx + this.$dy * this.$dy;
		return len2 === 0 ? 0 : ((x - this.$x0) * this.$dx + (y - this.$y0) * this.$dy) / len2;
	}

	$colorAt(x, y) {
		const stops = this.$list();
		const t = this.$t(x, y);
		if (t <= stops[0].p) return stops[0].color;
		for (let i = 0; i + 1 < stops.length; i++) {
			if (t <= stops[i + 1].p) {
				const span = stops[i + 1].p - stops[i].p;
				return span > 0 ? mixColors(stops[i].color, stops[i + 1].color, (t - stops[i].p) / span) : stops[i + 1].color;
			}
		}
		return stops[stops.length - 1].color;
	}

	// A convex polygon (flat x, y list) as pieces that each lie between two stops, with a color for every corner: { points, colors }.
	$pieces(points) {
		const stops = this.$list();
		const slices = [{ lo: -Infinity, hi: stops[0].p, a: stops[0].color, b: stops[0].color }];
		for (let i = 0; i + 1 < stops.length; i++) {
			if (stops[i + 1].p > stops[i].p) slices.push({ lo: stops[i].p, hi: stops[i + 1].p, a: stops[i].color, b: stops[i + 1].color });
		}
		const last = stops[stops.length - 1];
		slices.push({ lo: last.p, hi: Infinity, a: last.color, b: last.color });
		const pieces = [];
		for (const slice of slices) {
			let piece = points;
			if (slice.lo > -Infinity) piece = this.$clip(piece, slice.lo, true);
			if (slice.hi < Infinity) piece = this.$clip(piece, slice.hi, false);
			const n = piece.length / 2;
			if (n < 3) continue;
			const span = slice.hi - slice.lo;
			const colors = [];
			for (let i = 0; i < n; i++) {
				const f = span > 0 && span < Infinity ? Math.min(Math.max((this.$t(piece[2 * i], piece[2 * i + 1]) - slice.lo) / span, 0), 1) : 0;
				colors.push(mixColors(slice.a, slice.b, f));
			}
			pieces.push({ points: piece, colors });
		}
		return pieces;
	}

	// The part of a convex polygon whose position is above (or below) `limit`.
	$clip(points, limit, keepAbove) {
		const out = [];
		const n = points.length / 2;
		for (let i = 0; i < n; i++) {
			const j = (i + 1) % n;
			const ax = points[2 * i];
			const ay = points[2 * i + 1];
			const bx = points[2 * j];
			const by = points[2 * j + 1];
			const ta = this.$t(ax, ay);
			const tb = this.$t(bx, by);
			const ina = keepAbove ? ta >= limit : ta <= limit;
			const inb = keepAbove ? tb >= limit : tb <= limit;
			if (ina) out.push(ax, ay);
			if (ina !== inb) {
				const k = (limit - ta) / (tb - ta);
				out.push(ax + (bx - ax) * k, ay + (by - ay) * k);
			}
		}
		return out;
	}
}

// The 2D context passed to onPaint (`getContext("2d")`). A subset of the HTML canvas context:
// fills, strokes (also dashed), text, transforms, rectangular clipping and the state stack. Every
// drawing call is recorded into `output` as a command; `output === null` makes a detached
// context that only measures text (getContext() outside of onPaint).
export class CanvasContext {
	constructor(width, height, output) {
		this.$output = output;
		this.$width = width;
		this.$height = height;
		this.$init();
	}

	$init() {
		this.$m = [1, 0, 0, 1, 0, 0];
		this.$fillColor = toColor('#000000');
		this.$fillText = '#000000';
		this.$fillGradient = null;
		this.$strokeColor = toColor('#000000');
		this.$strokeText = '#000000';
		this.$strokeGradient = null;
		this.$lineWidth = 1;
		this.$alpha = 1;
		this.$dash = [];
		this.$fontCss = '10px sans-serif';
		this.$fontInfo = null;
		this.$textAlign = 'start';
		this.$textBaseline = 'alphabetic';
		this.$subPaths = [];
		this.$stack = [];
		this.$clipDepth = 0;
	}

	get width() {
		return this.$width;
	}

	get height() {
		return this.$height;
	}

	get fillStyle() {
		return this.$fillText;
	}

	// Like Qt's Context2D and the HTML canvas, a value that is not a color (`undefined`, a misspelt
	// property, "nonsense") leaves the style unchanged instead of failing the whole onPaint.
	set fillStyle(value) {
		if (value instanceof CanvasGradient) {
			this.$fillGradient = value;
			this.$fillText = value;
			return;
		}
		const color = styleColor(value);
		if (color === null) return;
		this.$fillColor = color;
		this.$fillText = value;
		this.$fillGradient = null;
	}

	get strokeStyle() {
		return this.$strokeText;
	}

	set strokeStyle(value) {
		if (value instanceof CanvasGradient) {
			this.$strokeGradient = value;
			this.$strokeText = value;
			return;
		}
		const color = styleColor(value);
		if (color === null) return;
		this.$strokeColor = color;
		this.$strokeText = value;
		this.$strokeGradient = null;
	}

	createLinearGradient(x0, y0, x1, y1) {
		return new CanvasGradient(toFloat(x0), toFloat(y0), toFloat(x1), toFloat(y1));
	}

	get lineWidth() {
		return this.$lineWidth;
	}

	set lineWidth(value) {
		const w = toFloat(value);
		if (w > 0) this.$lineWidth = w;
	}

	get globalAlpha() {
		return this.$alpha;
	}

	set globalAlpha(value) {
		const a = toFloat(value);
		if (a >= 0 && a <= 1) this.$alpha = a;
	}

	get font() {
		return this.$fontCss;
	}

	set font(value) {
		const css = String(value);
		const parsed = parseFont(css);
		if (parsed === null) return; // an invalid font is ignored, like in a browser
		this.$fontCss = css;
		this.$fontInfo = fonts.get(parsed.family, parsed.size, parsed.bold, parsed.italic);
	}

	get textAlign() {
		return this.$textAlign;
	}

	set textAlign(value) {
		this.$textAlign = String(value);
	}

	get textBaseline() {
		return this.$textBaseline;
	}

	set textBaseline(value) {
		this.$textBaseline = String(value);
	}

	setLineDash(segments) {
		const list = Array.from(segments ?? [], toFloat);
		this.$dash = list.length > 0 && list.every((v) => v >= 0) && list.some((v) => v > 0) ? (list.length % 2 ? list.concat(list) : list) : [];
	}

	getLineDash() {
		return this.$dash.slice();
	}

	// ----- recording -----

	$emit(command) {
		if (this.$output === null) return;
		command.m = this.$m.slice();
		this.$output.push(command);
	}

	// Closes the clips that are still open (as the end of onPaint does for unbalanced save()).
	$finish() {
		while (this.$clipDepth > 0) {
			this.$clipDepth--;
			this.$emit({ k: 'unclip' });
		}
	}

	// ----- state -----

	save() {
		this.$stack.push({
			m: this.$m.slice(),
			fillColor: this.$fillColor,
			fillText: this.$fillText,
			fillGradient: this.$fillGradient,
			strokeColor: this.$strokeColor,
			strokeText: this.$strokeText,
			strokeGradient: this.$strokeGradient,
			lineWidth: this.$lineWidth,
			alpha: this.$alpha,
			dash: this.$dash,
			fontCss: this.$fontCss,
			fontInfo: this.$fontInfo,
			textAlign: this.$textAlign,
			textBaseline: this.$textBaseline,
			clipDepth: this.$clipDepth,
		});
	}

	restore() {
		const s = this.$stack.pop();
		if (s === undefined) return;
		while (this.$clipDepth > s.clipDepth) {
			this.$clipDepth--;
			this.$emit({ k: 'unclip' });
		}
		this.$m = s.m;
		this.$fillColor = s.fillColor;
		this.$fillText = s.fillText;
		this.$fillGradient = s.fillGradient;
		this.$strokeColor = s.strokeColor;
		this.$strokeText = s.strokeText;
		this.$strokeGradient = s.strokeGradient;
		this.$lineWidth = s.lineWidth;
		this.$alpha = s.alpha;
		this.$dash = s.dash;
		this.$fontCss = s.fontCss;
		this.$fontInfo = s.fontInfo;
		this.$textAlign = s.textAlign;
		this.$textBaseline = s.textBaseline;
	}

	// Back to the initial state with nothing drawn.
	reset() {
		if (this.$output !== null) this.$output.length = 0;
		this.$init();
	}

	// ----- transform -----

	transform(a, b, c, d, e, f) {
		a = toFloat(a);
		b = toFloat(b);
		c = toFloat(c);
		d = toFloat(d);
		e = toFloat(e);
		f = toFloat(f);
		const m = this.$m;
		this.$m = [
			m[0] * a + m[2] * b,
			m[1] * a + m[3] * b,
			m[0] * c + m[2] * d,
			m[1] * c + m[3] * d,
			m[0] * e + m[2] * f + m[4],
			m[1] * e + m[3] * f + m[5],
		];
	}

	setTransform(a, b, c, d, e, f) {
		this.$m = [1, 0, 0, 1, 0, 0];
		this.transform(a, b, c, d, e, f);
	}

	resetTransform() {
		this.$m = [1, 0, 0, 1, 0, 0];
	}

	translate(x, y) {
		this.transform(1, 0, 0, 1, x, y);
	}

	scale(x, y) {
		this.transform(x, 0, 0, y, 0, 0);
	}

	rotate(angle) {
		const cos = Math.cos(toFloat(angle));
		const sin = Math.sin(toFloat(angle));
		this.transform(cos, sin, -sin, cos, 0, 0);
	}

	// ----- path -----

	beginPath() {
		this.$subPaths = [];
	}

	moveTo(x, y) {
		this.$subPaths.push([toFloat(x), toFloat(y)]);
	}

	lineTo(x, y) {
		if (this.$subPaths.length === 0) {
			this.moveTo(x, y);
			return;
		}
		this.$subPaths[this.$subPaths.length - 1].push(toFloat(x), toFloat(y));
	}

	// Last point of the current subpath, or null when there is none.
	$currentPoint() {
		const path = this.$subPaths[this.$subPaths.length - 1];
		if (path === undefined || path.length < 2) return null;
		return { x: path[path.length - 2], y: path[path.length - 1] };
	}

	// Approximates a quadratic Bézier from the current point through (cpx, cpy) to (x, y).
	quadraticCurveTo(cpx, cpy, x, y) {
		cpx = toFloat(cpx);
		cpy = toFloat(cpy);
		x = toFloat(x);
		y = toFloat(y);
		let from = this.$currentPoint();
		if (from === null) {
			this.moveTo(cpx, cpy);
			from = { x: cpx, y: cpy };
		}
		const steps = Math.max(8, Math.ceil(Math.hypot(x - from.x, y - from.y) / 4));
		for (let i = 1; i <= steps; i++) {
			const t = i / steps;
			const u = 1 - t;
			this.lineTo(u * u * from.x + 2 * u * t * cpx + t * t * x, u * u * from.y + 2 * u * t * cpy + t * t * y);
		}
	}

	// Approximates a cubic Bézier from the current point through (cp1) and (cp2) to (x, y).
	bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y) {
		cp1x = toFloat(cp1x);
		cp1y = toFloat(cp1y);
		cp2x = toFloat(cp2x);
		cp2y = toFloat(cp2y);
		x = toFloat(x);
		y = toFloat(y);
		let from = this.$currentPoint();
		if (from === null) {
			this.moveTo(cp1x, cp1y);
			from = { x: cp1x, y: cp1y };
		}
		const steps = Math.max(8, Math.ceil(Math.hypot(x - from.x, y - from.y) / 4));
		for (let i = 1; i <= steps; i++) {
			const t = i / steps;
			const u = 1 - t;
			const uu = u * u;
			const tt = t * t;
			this.lineTo(
				uu * u * from.x + 3 * uu * t * cp1x + 3 * u * tt * cp2x + tt * t * x,
				uu * u * from.y + 3 * uu * t * cp1y + 3 * u * tt * cp2y + tt * t * y,
			);
		}
	}

	// Fills treat every sub-path as closed; the flag only matters for strokes.
	closePath() {
		if (this.$subPaths.length > 0) this.$subPaths[this.$subPaths.length - 1].closed = true;
	}

	// Draws triangles (a flat point list and indices) with a gradient: one piece for each part between two stops.
	$emitGradient(gradient, points, indices) {
		for (let i = 0; i + 2 < indices.length; i += 3) {
			const triangle = [];
			for (let k = 0; k < 3; k++) triangle.push(points[2 * indices[i + k]], points[2 * indices[i + k] + 1]);
			for (const piece of gradient.$pieces(triangle)) {
				this.$emit({ k: 'gpoly', points: Float64Array.from(piece.points), colors: piece.colors.map((c) => withAlpha(c, this.$alpha)) });
			}
		}
	}

	fill() {
		if (this.$output === null) return;
		const color = withAlpha(this.$fillColor, this.$alpha);
		for (const subPath of this.$subPaths) {
			const { points, indices } = fillMesh(subPath);
			if (indices.length === 0) continue;
			if (this.$fillGradient !== null) this.$emitGradient(this.$fillGradient, points, indices);
			else this.$emit({ k: 'poly', points: Float64Array.from(points), indices, color });
		}
	}

	// Strokes the current path with the current line width and dash pattern. Segments are drawn
	// as quads with butt caps; joints of a polyline are covered by extending the segments.
	stroke() {
		this.$strokePaths(this.$subPaths);
	}

	strokeRect(x, y, w, h) {
		x = toFloat(x);
		y = toFloat(y);
		w = toFloat(w);
		h = toFloat(h);
		const path = [x, y, x + w, y, x + w, y + h, x, y + h];
		path.closed = true;
		this.$strokePaths([path]);
	}

	$strokePaths(paths) {
		if (this.$output === null) return;
		const color = withAlpha(this.$strokeColor, this.$alpha);
		const half = this.$lineWidth / 2;
		const dash = this.$dash;
		for (const path of paths) {
			const count = path.length >> 1;
			if (count < 2) continue;
			const segments = path.closed && count > 2 ? count : count - 1;
			let dashIndex = 0;
			let dashLeft = dash.length > 0 ? dash[0] : 0;
			let dashOn = true;
			for (let i = 0; i < segments; i++) {
				const j = (i + 1) % count;
				let x0 = path[2 * i];
				let y0 = path[2 * i + 1];
				let x1 = path[2 * j];
				let y1 = path[2 * j + 1];
				const len = Math.hypot(x1 - x0, y1 - y0);
				if (len === 0) continue;
				const dx = (x1 - x0) / len;
				const dy = (y1 - y0) / len;
				if (dash.length === 0) {
					const extendStart = i > 0 || path.closed ? half : 0;
					const extendEnd = i < segments - 1 || path.closed ? half : 0;
					this.$strokeSegment(x0 - dx * extendStart, y0 - dy * extendStart, x1 + dx * extendEnd, y1 + dy * extendEnd, half, color);
					continue;
				}
				let pos = 0;
				while (pos < len) {
					const step = Math.min(dashLeft, len - pos);
					if (dashOn && step > 0) {
						this.$strokeSegment(x0 + dx * pos, y0 + dy * pos, x0 + dx * (pos + step), y0 + dy * (pos + step), half, color);
					}
					pos += step;
					dashLeft -= step;
					if (dashLeft <= 1e-9) {
						dashIndex = (dashIndex + 1) % dash.length;
						dashLeft = dash[dashIndex];
						dashOn = !dashOn;
						if (dashLeft <= 0 && dash.every((v) => v <= 0)) return;
					}
				}
			}
		}
	}

	$strokeSegment(x0, y0, x1, y1, half, color) {
		const len = Math.hypot(x1 - x0, y1 - y0);
		if (len === 0) return;
		const nx = (-(y1 - y0) / len) * half;
		const ny = ((x1 - x0) / len) * half;
		if (this.$strokeGradient !== null) {
			this.$emitGradient(this.$strokeGradient, [x0 + nx, y0 + ny, x1 + nx, y1 + ny, x1 - nx, y1 - ny, x0 - nx, y0 - ny], [0, 1, 2, 0, 2, 3]);
			return;
		}
		this.$emit({
			k: 'poly',
			points: Float64Array.of(x0 + nx, y0 + ny, x1 + nx, y1 + ny, x1 - nx, y1 - ny, x0 - nx, y0 - ny),
			indices: [0, 1, 2, 0, 2, 3],
			color,
			// An axis-aligned segment needs no anti-aliasing fringe (it would blur the line).
			aa: x0 !== x1 && y0 !== y1 ? undefined : false,
		});
	}

	// Clips the following drawing (until the matching restore()) to the bounding box of the path.
	clip() {
		let x0 = Infinity;
		let y0 = Infinity;
		let x1 = -Infinity;
		let y1 = -Infinity;
		for (const p of this.$subPaths) {
			for (let i = 0; i + 1 < p.length; i += 2) {
				x0 = Math.min(x0, p[i]);
				y0 = Math.min(y0, p[i + 1]);
				x1 = Math.max(x1, p[i]);
				y1 = Math.max(y1, p[i + 1]);
			}
		}
		if (x0 > x1) return;
		this.$clipDepth++;
		this.$emit({ k: 'clip', x: x0, y: y0, w: x1 - x0, h: y1 - y0 });
	}

	// Clearing the whole canvas drops what was drawn so far; a partial clear is not supported.
	clearRect(x, y, w, h) {
		if (this.$output !== null && toFloat(x) <= 0 && toFloat(y) <= 0 && toFloat(w) >= this.$width && toFloat(h) >= this.$height) {
			this.$output.length = 0;
			this.$clipDepth = 0;
			for (const s of this.$stack) s.clipDepth = 0;
		}
	}

	rect(x, y, w, h) {
		x = toFloat(x);
		y = toFloat(y);
		w = toFloat(w);
		h = toFloat(h);
		this.moveTo(x, y);
		this.lineTo(x + w, y);
		this.lineTo(x + w, y + h);
		this.lineTo(x, y + h);
		this.closePath();
	}

	// The arc goes clockwise from start to end, or counter-clockwise when `anticlockwise` is set (`arc(w, 0, r, 0, Math.PI / 2, true)`
	// is three quarters of the circle); one full circle at most.
	arc(cx, cy, radius, start, end, anticlockwise = false) {
		cx = toFloat(cx);
		cy = toFloat(cy);
		radius = toFloat(radius);
		start = toFloat(start);
		end = toFloat(end);
		const turn = 2 * Math.PI;
		if (anticlockwise) {
			if (start - end >= turn) end = start - turn;
			else while (end > start) end -= turn;
		} else if (end - start >= turn) end = start + turn;
		else while (end < start) end += turn;
		this.$ellipticArc(cx, cy, radius, radius, start, end);
	}

	$ellipticArc(cx, cy, rx, ry, start, end) {
		const steps = Math.max(8, Math.ceil(Math.abs(end - start) * Math.max(rx, ry, 1) / 4));
		for (let i = 0; i <= steps; i++) {
			const angle = start + ((end - start) * i) / steps;
			this.lineTo(cx + rx * Math.cos(angle), cy + ry * Math.sin(angle));
		}
	}

	// Qt Context2D extension: a closed rounded rectangle sub-path, like QPainterPath::addRoundedRect with
	// Qt::AbsoluteSize (the radii are clamped to half the sides; a zero radius gives a plain rectangle).
	roundedRect(x, y, w, h, xRadius, yRadius) {
		x = toFloat(x);
		y = toFloat(y);
		w = toFloat(w);
		h = toFloat(h);
		if (w === 0 && h === 0) {
			this.moveTo(x, y);
			return;
		}
		const rx = Math.min(Math.abs(toFloat(xRadius)), Math.abs(w) / 2);
		const ry = Math.min(Math.abs(toFloat(yRadius)), Math.abs(h) / 2);
		if (rx <= 0 || ry <= 0) {
			this.rect(x, y, w, h);
			return;
		}
		const half = Math.PI / 2;
		this.moveTo(x, y + ry);
		this.$ellipticArc(x + rx, y + ry, rx, ry, Math.PI, Math.PI + half);
		this.$ellipticArc(x + w - rx, y + ry, rx, ry, Math.PI + half, 2 * Math.PI);
		this.$ellipticArc(x + w - rx, y + h - ry, rx, ry, 0, half);
		this.$ellipticArc(x + rx, y + h - ry, rx, ry, half, Math.PI);
		this.closePath();
	}

	// Qt Context2D extension: a closed ellipse sub-path inscribed in the rectangle.
	ellipse(x, y, w, h) {
		x = toFloat(x);
		y = toFloat(y);
		w = toFloat(w);
		h = toFloat(h);
		const rx = w / 2;
		const ry = h / 2;
		this.moveTo(x + w, y + ry);
		this.$ellipticArc(x + rx, y + ry, rx, ry, 0, 2 * Math.PI);
		this.closePath();
	}

	// ----- text -----

	// Outlined text is not drawn.
	strokeText() {}

	measureText(text) {
		const info = this.$fontInfo ?? fonts.get('sans-serif', 10, false, false);
		let width = 0;
		for (const ch of String(text)) width += info.advance(ch);
		return {
			width,
			actualBoundingBoxLeft: 0,
			actualBoundingBoxRight: width,
			fontBoundingBoxAscent: info.ascent,
			fontBoundingBoxDescent: info.lineHeight - info.ascent,
			actualBoundingBoxAscent: info.ascent,
			actualBoundingBoxDescent: info.lineHeight - info.ascent,
		};
	}

	fillText(text, x, y) {
		if (this.$output === null) return;
		text = String(text);
		if (text.length === 0) return;
		const info = this.$fontInfo ?? fonts.get('sans-serif', 10, false, false);
		x = toFloat(x);
		y = toFloat(y);
		const width = this.measureText(text).width;
		switch (this.$textAlign) {
			case 'center':
				x -= width / 2;
				break;
			case 'right':
			case 'end':
				x -= width;
				break;
		}
		const descent = info.lineHeight - info.ascent;
		let baseline = y;
		switch (this.$textBaseline) {
			case 'top':
			case 'hanging':
				baseline = y + info.ascent;
				break;
			case 'middle':
				baseline = y + info.ascent - info.lineHeight / 2;
				break;
			case 'bottom':
			case 'ideographic':
				baseline = y - descent;
				break;
		}
		// Text is filled with the color of the gradient where it starts.
		const textColor = this.$fillGradient !== null ? this.$fillGradient.$colorAt(x, baseline) : this.$fillColor;
		this.$emit({ k: 'text', info, text, x, y: baseline, color: withAlpha(textColor, this.$alpha) });
	}

	// ----- images -----

	// Images are not supported yet: nothing is loaded and drawImage() draws nothing.
	loadImage() {}

	drawImage() {}

	// ----- rectangles -----

	fillRect(x, y, w, h) {
		x = toFloat(x);
		y = toFloat(y);
		w = toFloat(w);
		h = toFloat(h);
		if (w < 0) {
			x += w;
			w = -w;
		}
		if (h < 0) {
			y += h;
			h = -h;
		}
		if (w === 0 || h === 0) return;
		if (this.$fillGradient !== null) {
			this.$emitGradient(this.$fillGradient, [x, y, x + w, y, x + w, y + h, x, y + h], [0, 1, 2, 0, 2, 3]);
			return;
		}
		this.$emit({ k: 'rect', x, y, w, h, color: withAlpha(this.$fillColor, this.$alpha) });
	}
}

// Draws recorded commands through the renderer `r`, relative to its current matrix.
// Does anything that was drawn lie outside of the canvas (0, 0, w, h)? Such a canvas is clipped when it is painted, as in
// Qt, where it is a texture of its size (PaintedCorner fills a path that goes round the corner above the canvas).
export function commandsExceed(commands, w, h) {
	const eps = 0.5;
	for (const c of commands) {
		if (c.k !== 'poly' && c.k !== 'gpoly' && c.k !== 'rect') continue;
		const m = c.m;
		const outside = (x, y) => {
			const tx = m[0] * x + m[2] * y + m[4];
			const ty = m[1] * x + m[3] * y + m[5];
			return tx < -eps || ty < -eps || tx > w + eps || ty > h + eps;
		};
		if (c.k === 'rect') {
			if (outside(c.x, c.y) || outside(c.x + c.w, c.y) || outside(c.x + c.w, c.y + c.h) || outside(c.x, c.y + c.h)) return true;
			continue;
		}
		for (let i = 0; i + 1 < c.points.length; i += 2) {
			if (outside(c.points[i], c.points[i + 1])) return true;
		}
	}
	return false;
}

export function replayCommands(r, commands, opacity, aa) {
	const { ma, mb, mc, md, me, mf } = r;
	const clips = []; // open clips: { skip } or { token, m, w, h }
	let hidden = 0; // number of open clips that hide everything
	for (const c of commands) {
		const m = c.m;
		r.setMatrix(
			ma * m[0] + mc * m[1],
			mb * m[0] + md * m[1],
			ma * m[2] + mc * m[3],
			mb * m[2] + md * m[3],
			ma * m[4] + mc * m[5] + me,
			mb * m[4] + md * m[5] + mf,
		);
		switch (c.k) {
			case 'clip': {
				if (hidden > 0) {
					clips.push({ skip: true });
					hidden++;
					break;
				}
				r.translate(c.x, c.y);
				const saved = [r.ma, r.mb, r.mc, r.md, r.me, r.mf];
				const token = r.beginClip(c.w, c.h);
				if (token === null) {
					clips.push({ skip: true });
					hidden++;
				} else {
					clips.push({ token, m: saved });
				}
				break;
			}
			case 'unclip': {
				const clip = clips.pop();
				if (clip === undefined) break;
				if (clip.skip) {
					hidden--;
				} else {
					// A stencil clip is removed by drawing it again, so its matrix is needed.
					r.setMatrix(...clip.m);
					r.endClip(clip.token);
				}
				break;
			}
			case 'rect': {
				if (hidden > 0) break;
				const color = r.pack(c.color, opacity);
				if (Math.abs(r.mb) < 1e-9 && Math.abs(r.mc) < 1e-9) r.fillQuad(c.x, c.y, c.w, c.h, color);
				else r.fillQuadAA(c.x, c.y, c.w, c.h, color);
				break;
			}
			case 'poly':
				if (hidden > 0) break;
				r.fillTriangles(c.points, c.indices, r.pack(c.color, opacity), c.aa ?? aa);
				break;
			case 'gpoly':
				if (hidden > 0) break;
				r.fillFan(c.points, c.colors.map((color) => r.pack(color, opacity)));
				break;
			case 'text': {
				if (hidden > 0) break;
				const color = r.pack(c.color, opacity);
				const info = c.info;
				let pen = c.x;
				for (const ch of c.text) {
					const glyph = info.glyph(ch);
					if (glyph !== null) r.glyph(glyph, pen, c.y, color);
					pen += info.advance(ch);
				}
				break;
			}
		}
	}
	while (clips.length > 0) {
		const clip = clips.pop();
		if (!clip.skip) {
			r.setMatrix(...clip.m);
			r.endClip(clip.token);
		}
	}
	r.setMatrix(ma, mb, mc, md, me, mf);
}
