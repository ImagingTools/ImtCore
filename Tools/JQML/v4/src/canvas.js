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
		this.$strokeColor = toColor('#000000');
		this.$strokeText = '#000000';
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
		const color = styleColor(value);
		if (color === null) return;
		this.$fillColor = color;
		this.$fillText = value;
	}

	get strokeStyle() {
		return this.$strokeText;
	}

	set strokeStyle(value) {
		const color = styleColor(value);
		if (color === null) return;
		this.$strokeColor = color;
		this.$strokeText = value;
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
			strokeColor: this.$strokeColor,
			strokeText: this.$strokeText,
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
		this.$strokeColor = s.strokeColor;
		this.$strokeText = s.strokeText;
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

	fill() {
		if (this.$output === null) return;
		const color = withAlpha(this.$fillColor, this.$alpha);
		for (const subPath of this.$subPaths) {
			const indices = triangulate(subPath);
			if (indices.length === 0) continue;
			this.$emit({ k: 'poly', points: Float64Array.from(subPath), indices, color });
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

	arc(cx, cy, radius, start, end) {
		cx = toFloat(cx);
		cy = toFloat(cy);
		radius = toFloat(radius);
		start = toFloat(start);
		end = toFloat(end);
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
		this.$emit({ k: 'text', info, text, x, y: baseline, color: withAlpha(this.$fillColor, this.$alpha) });
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
		this.$emit({ k: 'rect', x, y, w, h, color: withAlpha(this.$fillColor, this.$alpha) });
	}
}

// Draws recorded commands through the renderer `r`, relative to its current matrix.
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
