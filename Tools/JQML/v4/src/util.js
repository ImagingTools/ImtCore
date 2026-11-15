// Shared helpers: colors, numbers and virtual paths.

import { QmlError } from './lexer.js';

// ----- Colors -----

const NAMED = {
	black: [0, 0, 0],
	white: [255, 255, 255],
	red: [255, 0, 0],
	green: [0, 128, 0],
	blue: [0, 0, 255],
	yellow: [255, 255, 0],
	gold: [255, 215, 0],
	orange: [255, 165, 0],
	gray: [128, 128, 128],
	grey: [128, 128, 128],
	cyan: [0, 255, 255],
	magenta: [255, 0, 255],
	purple: [128, 0, 128],
	pink: [255, 192, 203],
	transparent: [0, 0, 0, 0],
	aliceblue: [240, 248, 255],
	antiquewhite: [250, 235, 215],
	aquamarine: [127, 255, 212],
	azure: [240, 255, 255],
	beige: [245, 245, 220],
	bisque: [255, 228, 196],
	blanchedalmond: [255, 235, 205],
	blueviolet: [138, 43, 226],
	brown: [165, 42, 42],
	burlywood: [222, 184, 135],
	cadetblue: [95, 158, 160],
	chartreuse: [127, 255, 0],
	chocolate: [210, 105, 30],
	coral: [255, 127, 80],
	cornflowerblue: [100, 149, 237],
	cornsilk: [255, 248, 220],
	crimson: [220, 20, 60],
	darkblue: [0, 0, 139],
	darkcyan: [0, 139, 139],
	darkgoldenrod: [184, 134, 11],
	darkgray: [169, 169, 169],
	darkgrey: [169, 169, 169],
	darkgreen: [0, 100, 0],
	darkkhaki: [189, 183, 107],
	darkmagenta: [139, 0, 139],
	darkolivegreen: [85, 107, 47],
	darkorange: [255, 140, 0],
	darkorchid: [153, 50, 204],
	darkred: [139, 0, 0],
	darksalmon: [233, 150, 122],
	darkseagreen: [143, 188, 143],
	darkslateblue: [72, 61, 139],
	darkslategray: [47, 79, 79],
	darkslategrey: [47, 79, 79],
	darkturquoise: [0, 206, 209],
	darkviolet: [148, 0, 211],
	deeppink: [255, 20, 147],
	deepskyblue: [0, 191, 255],
	dimgray: [105, 105, 105],
	dimgrey: [105, 105, 105],
	dodgerblue: [30, 144, 255],
	firebrick: [178, 34, 34],
	floralwhite: [255, 250, 240],
	forestgreen: [34, 139, 34],
	gainsboro: [220, 220, 220],
	ghostwhite: [248, 248, 255],
	goldenrod: [218, 165, 32],
	greenyellow: [173, 255, 47],
	honeydew: [240, 255, 240],
	hotpink: [255, 105, 180],
	indianred: [205, 92, 92],
	indigo: [75, 0, 130],
	ivory: [255, 255, 240],
	khaki: [240, 230, 140],
	lavender: [230, 230, 250],
	lavenderblush: [255, 240, 245],
	lawngreen: [124, 252, 0],
	lemonchiffon: [255, 250, 205],
	lightblue: [173, 216, 230],
	lightcoral: [240, 128, 128],
	lightcyan: [224, 255, 255],
	lightgoldenrodyellow: [250, 250, 210],
	lightgray: [211, 211, 211],
	lightgrey: [211, 211, 211],
	lightgreen: [144, 238, 144],
	lightpink: [255, 182, 193],
	lightsalmon: [255, 160, 122],
	lightseagreen: [32, 178, 170],
	lightskyblue: [135, 206, 250],
	lightslategray: [119, 136, 153],
	lightslategrey: [119, 136, 153],
	lightsteelblue: [176, 196, 222],
	lightyellow: [255, 255, 224],
	lime: [0, 255, 0],
	limegreen: [50, 205, 50],
	linen: [250, 240, 230],
	maroon: [128, 0, 0],
	mediumaquamarine: [102, 205, 170],
	mediumblue: [0, 0, 205],
	mediumorchid: [186, 85, 211],
	mediumpurple: [147, 112, 219],
	mediumseagreen: [60, 179, 113],
	mediumslateblue: [123, 104, 238],
	mediumspringgreen: [0, 250, 154],
	mediumturquoise: [72, 209, 204],
	mediumvioletred: [199, 21, 133],
	midnightblue: [25, 25, 112],
	mintcream: [245, 255, 250],
	mistyrose: [255, 228, 225],
	moccasin: [255, 228, 181],
	navajowhite: [255, 222, 173],
	navy: [0, 0, 128],
	oldlace: [253, 245, 230],
	olive: [128, 128, 0],
	olivedrab: [107, 142, 35],
	orangered: [255, 69, 0],
	orchid: [218, 112, 214],
	palegoldenrod: [238, 232, 170],
	palegreen: [152, 251, 152],
	paleturquoise: [175, 238, 238],
	palevioletred: [219, 112, 147],
	papayawhip: [255, 239, 213],
	peachpuff: [255, 218, 185],
	peru: [205, 133, 63],
	plum: [221, 160, 221],
	powderblue: [176, 224, 230],
	rosybrown: [188, 143, 143],
	royalblue: [65, 105, 225],
	saddlebrown: [139, 69, 19],
	salmon: [250, 128, 114],
	sandybrown: [244, 164, 96],
	seagreen: [46, 139, 87],
	seashell: [255, 245, 238],
	sienna: [160, 82, 45],
	silver: [192, 192, 192],
	skyblue: [135, 206, 235],
	slateblue: [106, 90, 205],
	slategray: [112, 128, 144],
	slategrey: [112, 128, 144],
	snow: [255, 250, 250],
	springgreen: [0, 255, 127],
	steelblue: [70, 130, 180],
	tan: [210, 180, 140],
	teal: [0, 128, 128],
	thistle: [216, 191, 216],
	tomato: [255, 99, 71],
	turquoise: [64, 224, 208],
	violet: [238, 130, 238],
	wheat: [245, 222, 179],
	whitesmoke: [245, 245, 245],
	yellowgreen: [154, 205, 50],
	aqua: [0, 255, 255],
	fuchsia: [255, 0, 255],
	rebeccapurple: [102, 51, 153],
};

const colorCache = new Map();
const MAX_COLOR_CACHE = 50000;

// A color: float channels, 8-bit channels and `rgb`, the channels packed as 0x00BBGGRR
// (the alpha byte is added when a vertex color is built).
function makeColor(r, g, b, a = 255) {
	return Object.freeze({
		r: r / 255, g: g / 255, b: b / 255, a: a / 255,
		R: r, G: g, B: b, A: a,
		rgb: (r | (g << 8) | (b << 16)) >>> 0,
	});
}

export function parseColor(text) {
	const cached = colorCache.get(text);
	if (cached !== undefined) {
		return cached;
	}
	let color = null;
	if (text.length > 0 && text[0] === '#') {
		if (/^#[0-9a-fA-F]+$/.test(text)) {
			const hex = (pos, len) => parseInt(text.substr(pos, len), 16);
			if (text.length === 4) {
				color = makeColor(hex(1, 1) * 17, hex(2, 1) * 17, hex(3, 1) * 17);
			} else if (text.length === 7) {
				color = makeColor(hex(1, 2), hex(3, 2), hex(5, 2));
			} else if (text.length === 9) {
				color = makeColor(hex(3, 2), hex(5, 2), hex(7, 2), hex(1, 2));
			}
		}
	} else if (/^rgba?\(/i.test(text)) {
		// CSS notation used by Canvas: rgb(r, g, b) and rgba(r, g, b, a) with a in 0..1.
		const m = /^rgba?\(\s*([\d.]+)\s*,\s*([\d.]+)\s*,\s*([\d.]+)\s*(?:,\s*([\d.]+)\s*)?\)$/i.exec(text);
		if (m !== null) {
			const ch = (v) => Math.max(0, Math.min(255, Math.round(Number(v))));
			color = makeColor(ch(m[1]), ch(m[2]), ch(m[3]), m[4] === undefined ? 255 : ch(Number(m[4]) * 255));
		}
	} else if (Object.prototype.hasOwnProperty.call(NAMED, text)) {
		const n = NAMED[text];
		color = makeColor(n[0], n[1], n[2], n.length > 3 ? n[3] : 255);
	}
	if (color === null) {
		throw new QmlError(`Invalid color '${text}'`);
	}
	if (colorCache.size >= MAX_COLOR_CACHE) {
		colorCache.clear();
	}
	colorCache.set(text, color);
	return color;
}

export function toColor(value) {
	if (typeof value !== 'string') {
		throw new QmlError('A color must be a string such as "#ff8800" or "red"');
	}
	return parseColor(value);
}

const hex2 = (v) => v.toString(16).toUpperCase().padStart(2, '0');

// "#AARRGGBB", the form in which colors are returned to scripts.
export function colorToString(c) {
	return `#${hex2(c.A)}${hex2(c.R)}${hex2(c.G)}${hex2(c.B)}`;
}

function hueToChannel(p, q, t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1 / 6) return p + (q - p) * 6 * t;
	if (t < 0.5) return q;
	if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
	return p;
}

const clamp01 = (v) => Math.min(1, Math.max(0, v));
const byte = (v) => Math.round(clamp01(v) * 255);

export function rgbaString(r, g, b, a = 1) {
	return `#${hex2(byte(a))}${hex2(byte(r))}${hex2(byte(g))}${hex2(byte(b))}`;
}

export function hslaString(h, s, l, a = 1) {
	h = h - Math.floor(h);
	s = clamp01(s);
	l = clamp01(l);
	if (s === 0) {
		return rgbaString(l, l, l, a);
	}
	const q = l < 0.5 ? l * (1 + s) : l + s - l * s;
	const p = 2 * l - q;
	return rgbaString(hueToChannel(p, q, h + 1 / 3), hueToChannel(p, q, h), hueToChannel(p, q, h - 1 / 3), a);
}

// ----- Numbers -----

export function toFloat(v) {
	const d = Number(v);
	return Number.isFinite(d) ? d : 0;
}

// ----- Virtual paths (always '/'-separated) -----

// `.import "Other.js" as Alias` inside a JavaScript file: [1] is the path, [2] the alias.
export const SCRIPT_IMPORT_RE = /^\s*\.import\s+"([^"]+\.js)"\s+as\s+(\w+)/gm;

export function normalizePath(path) {
	const absolute = path.startsWith('/');
	const out = [];
	for (const part of path.split('/')) {
		if (part === '' || part === '.') continue;
		if (part === '..') {
			if (out.length > 0 && out[out.length - 1] !== '..') out.pop();
			else if (!absolute) out.push('..');
			continue;
		}
		out.push(part);
	}
	return (absolute ? '/' : '') + out.join('/');
}

export function dirname(path) {
	const i = path.lastIndexOf('/');
	return i < 0 ? '' : path.slice(0, i);
}

export function basename(path) {
	return path.slice(path.lastIndexOf('/') + 1);
}

export function joinPath(directory, relative) {
	if (directory === '') return normalizePath(relative);
	return normalizePath(`${directory}/${relative}`);
}

// Accepts plain paths and file:/// URLs; relative paths are relative to `directory`.
export function resolvePath(path, directory) {
	let result = path;
	if (result.startsWith('file:///')) {
		result = result.slice(7);
	}
	if (/^[a-z][a-z0-9+.-]*:/i.test(result) || result.startsWith('/')) {
		return result;
	}
	return joinPath(directory || '', result);
}
