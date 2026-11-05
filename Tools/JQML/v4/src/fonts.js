// Fonts and the glyph atlas. Text is measured with the 2D canvas context of the browser and its
// glyphs are rasterized into atlas pages (canvases) that the renderer uploads as textures.
//
// Without a font family the 8x8 look of SDL's debug font is emulated: every character occupies
// a square cell of `pixelSize` and is drawn with the generic monospace font of the browser.

const PAGE_SIZE = 1024;
const MAX_PAGES = 4;
const GLYPH_PAD = 1;
const MAX_RASTER_PX = 256;
const GENERIC_FAMILIES = new Set(['serif', 'sans-serif', 'monospace', 'cursive', 'fantasy', 'system-ui', 'ui-monospace']);

function makeCanvas(width, height) {
	if (typeof OffscreenCanvas !== 'undefined') {
		return new OffscreenCanvas(width, height);
	}
	if (typeof document !== 'undefined') {
		const canvas = document.createElement('canvas');
		canvas.width = width;
		canvas.height = height;
		return canvas;
	}
	return null;
}

function splitFamilies(list) {
	return list
		.split(',')
		.map((name) => name.trim().replace(/^["']+|["']+$/g, '').trim())
		.filter((name) => name.length > 0);
}

// The family name stored in the 'name' table of a TrueType/OpenType file.
export function parseFontFamily(buffer) {
	try {
		const view = new DataView(buffer);
		let base = 0;
		if (view.getUint32(0) === 0x74746366) {
			base = view.getUint32(12); // 'ttcf': the first font of a collection
		}
		const tableCount = view.getUint16(base + 4);
		let nameOffset = 0;
		for (let t = 0; t < tableCount; t++) {
			const record = base + 12 + 16 * t;
			if (view.getUint32(record) === 0x6e616d65) {
				nameOffset = view.getUint32(record + 8);
				break;
			}
		}
		if (nameOffset === 0) return null;

		const count = view.getUint16(nameOffset + 2);
		const stringsOffset = nameOffset + view.getUint16(nameOffset + 4);
		let best = null;
		let bestScore = -1;
		for (let k = 0; k < count; k++) {
			const record = nameOffset + 6 + 12 * k;
			const platform = view.getUint16(record);
			const nameId = view.getUint16(record + 6);
			if (nameId !== 1 && nameId !== 16) continue;
			const length = view.getUint16(record + 8);
			const offset = stringsOffset + view.getUint16(record + 10);
			let text = '';
			if (platform === 1) {
				for (let i = 0; i < length; i++) text += String.fromCharCode(view.getUint8(offset + i));
			} else {
				for (let i = 0; i + 1 < length; i += 2) text += String.fromCharCode(view.getUint16(offset + i));
			}
			const score = (nameId === 1 ? 2 : 0) + (platform === 3 ? 1 : 0);
			if (text.length > 0 && score > bestScore) {
				best = text;
				bestScore = score;
			}
		}
		return best;
	} catch {
		return null;
	}
}

// Texture pages the glyphs are packed into (shelf packing).
export class GlyphAtlas {
	constructor() {
		this.pages = [];
		this.generation = 0;
		this.newPage();
	}

	newPage() {
		const canvas = makeCanvas(PAGE_SIZE, PAGE_SIZE);
		const context = canvas ? canvas.getContext('2d', { willReadFrequently: false }) : null;
		const page = { canvas, context, texture: null, dirty: true, cursorX: 0, cursorY: 4, rowHeight: 0, size: PAGE_SIZE };
		this.clearPage(page);
		this.pages.push(page);
		return page;
	}

	clearPage(page) {
		page.cursorX = 0;
		page.cursorY = 4;
		page.rowHeight = 0;
		page.dirty = true;
		const c = page.context;
		if (c) {
			c.clearRect(0, 0, page.size, page.size);
			c.fillStyle = '#fff';
			c.fillRect(0, 0, 4, 4); // an opaque white block for untextured geometry
			c.textBaseline = 'alphabetic';
			c.textAlign = 'left';
		}
	}

	// Reserves a w x h rectangle; returns null if it does not fit on a single page, or { page, x, y }.
	// When all pages are full the atlas is emptied (see `generation`).
	alloc(w, h) {
		if (w > PAGE_SIZE || h > PAGE_SIZE - 4) return null;
		for (let attempt = 0; attempt < 2; attempt++) {
			const page = this.pages[this.pages.length - 1];
			if (page.cursorX + w > page.size) {
				page.cursorY += page.rowHeight;
				page.cursorX = 0;
				page.rowHeight = 0;
			}
			if (page.cursorY + h <= page.size) {
				const slot = { page, x: page.cursorX, y: page.cursorY };
				page.cursorX += w;
				page.rowHeight = Math.max(page.rowHeight, h);
				page.dirty = true;
				return slot;
			}
			if (this.pages.length < MAX_PAGES) {
				this.newPage();
			} else {
				this.reset();
			}
		}
		return null;
	}

	// Empties every page; all glyph entries become invalid (see `generation`).
	reset() {
		this.generation++;
		for (const page of this.pages) this.clearPage(page);
		this.pages.length = 1;
	}
}

class FontStyle {
	constructor(manager, family, bold, italic) {
		this.manager = manager;
		this.family = family;
		this.bold = bold;
		this.italic = italic;
		this.advances = new Map();
		this.metrics = null;
	}

	css(px) {
		const family = /[\s,]/.test(this.family) && !GENERIC_FAMILIES.has(this.family) ? `"${this.family}"` : this.family;
		return `${this.italic ? 'italic ' : ''}${this.bold ? 'bold ' : ''}${px}px ${family}`;
	}

	// Advance width of a character at a font size of 100 pixels.
	advance100(ch) {
		let value = this.advances.get(ch);
		if (value === undefined) {
			const context = this.manager.measureContext();
			if (context === null) {
				value = 55;
			} else {
				context.font = this.css(100);
				value = context.measureText(ch).width;
			}
			this.advances.set(ch, value);
		}
		return value;
	}

	// Ascent, descent at a font size of 100 pixels.
	metrics100() {
		if (this.metrics === null) {
			const context = this.manager.measureContext();
			let ascent = 90;
			let descent = 25;
			if (context !== null) {
				context.font = this.css(100);
				const m = context.measureText('Hg');
				ascent = m.fontBoundingBoxAscent ?? m.actualBoundingBoxAscent ?? ascent;
				descent = m.fontBoundingBoxDescent ?? m.actualBoundingBoxDescent ?? descent;
			}
			this.metrics = { ascent, descent };
		}
		return this.metrics;
	}
}

// A font at a size: metrics and the glyphs of the atlas.
export class FontInfo {
	constructor(manager, style, size) {
		this.manager = manager;
		this.style = style; // null: the built-in look
		this.size = size;
		this.glyphs = new Map();
		this.generation = manager.atlas ? manager.atlas.generation : 0;

		if (style === null) {
			this.ascent = 0;
			this.lineHeight = size;
			// Qt's built-in box font (used when no fonts are installed): line spacing is 7/6 of the pixel size.
			this.lineSpacing = Math.round((size * 7) / 6);
			this.underlineY = size - Math.max(1, size * 0.08);
			this.strikeY = size * 0.5;
			this.lineWidth = Math.max(1, size * 0.08);
		} else {
			const m = style.metrics100();
			this.ascent = (m.ascent * size) / 100;
			this.lineHeight = Math.round(((m.ascent + m.descent) * size) / 100);
			this.lineSpacing = this.lineHeight;
			this.underlineY = this.ascent + Math.max(1, size * 0.1);
			this.strikeY = this.ascent - size * 0.3;
			this.lineWidth = Math.max(1, size * 0.07);
		}
	}

	advance(ch) {
		return this.style === null ? this.size : (this.style.advance100(ch) * this.size) / 100;
	}

	// The atlas entry of a character, or null if there is nothing to draw.
	glyph(ch) {
		if (this.generation !== (this.manager.atlas ? this.manager.atlas.generation : 0)) {
			this.glyphs.clear();
			this.generation = this.manager.atlas.generation;
		}
		let glyph = this.glyphs.get(ch);
		if (glyph === undefined) {
			glyph = this.manager.rasterize(this, ch);
			this.glyphs.set(ch, glyph);
		}
		return glyph;
	}
}

export class FontManager {
	constructor() {
		this.dpr = 1;
		this.atlas = null;
		this.version = 0; // incremented when fonts get registered
		this.registered = new Set();
		this.styles = new Map();
		this.infos = new Map();
		this.resolved = new Map();
		this.measure = undefined;
		this.warned = new Set();
		this.onWarning = null;
		// () => string: the family of text that names none (the main font of the style); '' selects the built-in look.
		this.defaultFamily = () => '';
	}

	setDevicePixelRatio(dpr) {
		if (dpr !== this.dpr) {
			this.dpr = dpr;
			this.infos.clear();
			if (this.atlas) this.atlas.reset();
		}
	}

	measureContext() {
		if (this.measure === undefined) {
			const canvas = makeCanvas(4, 4);
			this.measure = canvas ? canvas.getContext('2d') : null;
		}
		return this.measure;
	}

	ensureAtlas() {
		if (this.atlas === null && this.measureContext() !== null) {
			this.atlas = new GlyphAtlas();
		}
		return this.atlas;
	}

	// Registers font data (a TrueType/OpenType file). Returns the family name or '' on failure.
	async registerFont(buffer, fallbackName) {
		const family = parseFontFamily(buffer) || fallbackName;
		if (!family) return '';
		if (typeof FontFace !== 'undefined' && typeof document !== 'undefined') {
			try {
				const face = new FontFace(family, buffer);
				await face.load();
				document.fonts.add(face);
			} catch {
				return '';
			}
		}
		this.registered.add(family.toLowerCase());
		this.version++;
		this.resolved.clear();
		this.infos.clear();
		return family;
	}

	isAvailable(name) {
		const lower = name.toLowerCase();
		if (GENERIC_FAMILIES.has(lower) || this.registered.has(lower)) return true;
		const context = this.measureContext();
		if (context === null) return true;
		// A family is installed if the text is measured differently than with two unrelated fallbacks.
		const sample = 'mmmmmmmmmmlli WwQ@';
		const widths = [];
		for (const fallback of ['monospace', 'serif']) {
			context.font = `72px "${name}", ${fallback}`;
			widths.push(context.measureText(sample).width);
			context.font = `72px ${fallback}`;
			widths.push(context.measureText(sample).width);
		}
		return widths[0] !== widths[1] || widths[2] !== widths[3];
	}

	// The first available family of a comma-separated list, or null.
	resolveFamily(list) {
		if (this.resolved.has(list)) return this.resolved.get(list);
		let result = null;
		for (const name of splitFamilies(list)) {
			if (this.isAvailable(name)) {
				result = name;
				break;
			}
		}
		if (result === null && !this.warned.has(list)) {
			this.warned.add(list);
			if (this.onWarning) this.onWarning(`Font family '${list}' not found, using the built-in font`);
		}
		this.resolved.set(list, result);
		return result;
	}

	get(family, pixelSize, bold, italic) {
		const size = pixelSize > 0 ? pixelSize : 16;
		const resolved = family ? this.resolveFamily(family) : null;
		const key = `${resolved ?? ''}|${size}|${bold ? 1 : 0}${italic ? 1 : 0}`;
		let info = this.infos.get(key);
		if (info === undefined) {
			let style = null;
			if (resolved !== null) {
				const styleKey = `${resolved}|${bold ? 1 : 0}${italic ? 1 : 0}`;
				style = this.styles.get(styleKey);
				if (style === undefined) {
					style = new FontStyle(this, resolved, bold, italic);
					this.styles.set(styleKey, style);
				}
			}
			info = new FontInfo(this, style, size);
			this.infos.set(key, info);
		}
		return info;
	}

	// Draws a character into the atlas. Entry fields (in CSS pixels relative to the pen position on
	// the baseline): x0, y0, w, h; texture coordinates u0, v0, u1, v1; page.
	rasterize(info, ch) {
		if (ch === ' ' || ch === '\t' || ch === '\u00a0') return null;
		const atlas = this.ensureAtlas();
		if (atlas === null) return null;

		let px = Math.max(1, Math.round(info.size * this.dpr));
		if (px > MAX_RASTER_PX) px = MAX_RASTER_PX;
		const scale = px / info.size;
		let boxW;
		let boxH;
		let originX;
		let originY;
		let draw;

		if (info.style === null) {
			// A square cell: the character is scaled to fit it horizontally.
			const fontPx = Math.max(1, Math.round(px * 0.9));
			const measure = this.measureContext();
			measure.font = `${fontPx}px monospace`;
			const mWidth = measure.measureText('M').width || fontPx * 0.6;
			const sx = (px * 0.875) / mWidth;
			boxW = px + 2 * GLYPH_PAD;
			boxH = px + 2 * GLYPH_PAD;
			originX = GLYPH_PAD;
			originY = GLYPH_PAD;
			draw = (c, x, y) => {
				c.save();
				c.font = `${fontPx}px monospace`;
				c.translate(x + GLYPH_PAD + px * 0.0625, y + GLYPH_PAD + Math.round(px * 0.8));
				c.scale(sx, 1);
				c.fillText(ch, 0, 0);
				c.restore();
			};
		} else {
			const measure = this.measureContext();
			const css = info.style.css(px);
			measure.font = css;
			const m = measure.measureText(ch);
			const left = Math.ceil(m.actualBoundingBoxLeft || 0);
			const right = Math.ceil(m.actualBoundingBoxRight || m.width);
			const up = Math.ceil(m.actualBoundingBoxAscent || 0);
			const down = Math.ceil(m.actualBoundingBoxDescent || 0);
			boxW = left + right + 2 * GLYPH_PAD;
			boxH = up + down + 2 * GLYPH_PAD;
			if (right + left <= 0 || up + down <= 0) return null; // blank
			originX = GLYPH_PAD + left;
			originY = GLYPH_PAD + up;
			draw = (c, x, y) => {
				c.font = css;
				c.fillText(ch, x + originX, y + originY);
			};
		}

		const slot = atlas.alloc(boxW, boxH);
		if (slot === null) return null;
		slot.page.context.fillStyle = '#fff';
		draw(slot.page.context, slot.x, slot.y);

		const size = slot.page.size;
		let y0;
		if (info.style === null) {
			y0 = -GLYPH_PAD / scale;
		} else {
			y0 = -originY / scale;
		}
		return {
			page: slot.page,
			x0: -originX / scale,
			y0,
			w: boxW / scale,
			h: boxH / scale,
			u0: slot.x / size,
			v0: slot.y / size,
			u1: (slot.x + boxW) / size,
			v1: (slot.y + boxH) / size,
		};
	}
}

export const fonts = new FontManager();
