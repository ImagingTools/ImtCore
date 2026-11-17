// The rich text of a Text item: RichText (the HTML of Qt's QTextDocument) and StyledText (the small tag set of Qt Quick). The text is
// parsed into blocks of inline runs, the blocks are laid out in lines (wrapping across the runs of different fonts), and the result
// is what Text measures, paints and answers `linkAt` from. The rules are the ones of Qt, found by measuring its output (see
// test/desktop/RichText_*): the sizes of `<font size>` and of the headings, the margins that collapse between blocks (the first
// and the last are dropped), the indent of lists and quotes, a `<br>` that leaves an empty line, a `</div>` that does not end its block.

const NBSP = ' ';

// The size of the text of `<font size=N>` against the base size: the HTML sizes 1 to 7 as Qt maps them.
const SIZE_FACTORS = [0, 0.7, 0.8, 1, 1.2, 1.5, 2, 2.4];

// A heading: the size (an HTML size) and the margins of its block.
const HEADINGS = {
	h1: { level: 6, top: 18, bottom: 12 },
	h2: { level: 5, top: 16, bottom: 12 },
	h3: { level: 4, top: 14, bottom: 12 },
	h4: { level: 3, top: 12, bottom: 12 },
	h5: { level: 2, top: 12, bottom: 12 },
	h6: { level: 3, top: 0, bottom: 0 },
};

const CSS_SIZE_LEVELS = { small: 2, medium: 3, large: 4, 'x-large': 5, 'xx-large': 6 };

const NAMED_COLORS = {
	black: '#000000', silver: '#c0c0c0', gray: '#808080', grey: '#808080', white: '#ffffff', maroon: '#800000', red: '#ff0000',
	purple: '#800080', fuchsia: '#ff00ff', green: '#008000', lime: '#00ff00', olive: '#808000', yellow: '#ffff00', navy: '#000080',
	blue: '#0000ff', teal: '#008080', aqua: '#00ffff', orange: '#ffa500',
};

const ENTITIES = {
	nbsp: NBSP, lt: '<', gt: '>', amp: '&', quot: '"', apos: "'", copy: '©', reg: '®', laquo: '«', raquo: '»',
	mdash: '—', ndash: '–', hellip: '…', euro: '€', bull: '•', middot: '·', deg: '°', times: '×',
	trade: '™',
};

const VOID_TAGS = new Set(['br', 'hr', 'wbr', 'img', 'meta', 'link', 'input']);

// What StyledText reads of the tags (the rest is dropped, its text stays).
const STYLED_TAGS = new Set(['b', 'strong', 'i', 'em', 'u', 's', 'font', 'a', 'br', 'p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6']);

function decodeEntities(text) {
	return text.replace(/&(#x[0-9a-f]+|#[0-9]+|[a-z]+);/gi, (whole, name) => {
		if (name[0] === '#') {
			const code = name[1] === 'x' || name[1] === 'X' ? parseInt(name.slice(2), 16) : parseInt(name.slice(1), 10);
			return Number.isFinite(code) && code > 0 && code < 0x110000 ? String.fromCodePoint(code) : whole;
		}
		const value = ENTITIES[name.toLowerCase()];
		return value === undefined ? whole : value;
	});
}

// Whether a text that was set with Text.AutoText is taken for markup: a tag name after a '<' (Qt::mightBeRichText).
export function mightBeRichText(text) {
	return /<\s*\/?\s*[A-Za-z!][^<>]*>/.test(text);
}

// ----- tokens -----

function parseAttributes(source) {
	const attributes = {};
	const re = /([^\s=/>]+)(?:\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+)))?/g;
	let m;
	while ((m = re.exec(source)) !== null) {
		const value = m[2] !== undefined ? m[2] : m[3] !== undefined ? m[3] : m[4] !== undefined ? m[4] : '';
		attributes[m[1].toLowerCase()] = decodeEntities(value);
	}
	return attributes;
}

const SKIPPED = new Set(['head', 'title', 'style', 'script']);

// The tags and the text of the source, in order: { tag, close, attributes } or { text }. A '<' that does not close swallows the rest.
function tokenize(source) {
	const tokens = [];
	let i = 0;
	while (i < source.length) {
		const open = source.indexOf('<', i);
		if (open < 0) {
			tokens.push({ text: source.slice(i) });
			break;
		}
		if (open > i) tokens.push({ text: source.slice(i, open) });
		if (source.startsWith('<!--', open)) {
			const end = source.indexOf('-->', open + 4);
			i = end < 0 ? source.length : end + 3;
			continue;
		}
		const end = source.indexOf('>', open + 1);
		if (end < 0) break;
		const inner = source.slice(open + 1, end);
		i = end + 1;
		if (inner[0] === '!' || inner[0] === '?') continue;
		const m = /^\s*(\/)?\s*([A-Za-z][A-Za-z0-9:-]*)([\s\S]*)$/.exec(inner);
		if (m === null) continue;
		const tag = m[2].toLowerCase();
		const close = m[1] === '/';
		if (!close && SKIPPED.has(tag) && !/\/\s*$/.test(m[3])) {
			const found = new RegExp(`<\\s*/\\s*${tag}\\s*>`, 'i').exec(source.slice(i));
			i = found === null ? source.length : i + found.index + found[0].length;
			continue;
		}
		tokens.push({ tag, close, attributes: close ? {} : parseAttributes(m[3].replace(/\/\s*$/, '')) });
	}
	return tokens;
}

// ----- the document -----

function baseStyle() {
	return { bold: false, italic: false, underline: false, strike: false, color: null, background: null, level: 3, sizePx: null, family: null, script: 0, href: null, nowrap: false, pre: false };
}

function parseColor(value) {
	const v = String(value).trim().toLowerCase();
	if (/^#[0-9a-f]{3}$/.test(v)) return `#${v[1]}${v[1]}${v[2]}${v[2]}${v[3]}${v[3]}`;
	if (/^#[0-9a-f]{6}$/.test(v)) return v;
	if (NAMED_COLORS[v] !== undefined) return NAMED_COLORS[v];
	const rgb = /^rgb\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)$/.exec(v);
	if (rgb !== null) return `#${[rgb[1], rgb[2], rgb[3]].map((n) => Math.min(255, Number(n)).toString(16).padStart(2, '0')).join('')}`;
	return null;
}

function parseCss(text) {
	const out = {};
	for (const declaration of String(text).split(';')) {
		const at = declaration.indexOf(':');
		if (at < 0) continue;
		out[declaration.slice(0, at).trim().toLowerCase()] = declaration.slice(at + 1).trim();
	}
	return out;
}

// Applies the CSS of a `style` attribute (the part of it that Qt reads for the characters) to an inline style.
function applyCharacterCss(style, css) {
	for (const [name, value] of Object.entries(css)) {
		const v = value.toLowerCase();
		if (name === 'color') {
			const color = parseColor(v);
			if (color !== null) style.color = color;
		} else if (name === 'background-color') {
			const color = parseColor(v);
			if (color !== null) style.background = color;
		} else if (name === 'font-weight') {
			style.bold = v === 'bold' || v === 'bolder' || Number(v) >= 600;
		} else if (name === 'font-style') {
			style.italic = v === 'italic' || v === 'oblique';
		} else if (name === 'font-family') {
			style.family = value.split(',')[0].trim().replace(/^["']|["']$/g, '');
		} else if (name === 'font-size') {
			const px = /^([0-9.]+)px$/.exec(v);
			const pt = /^([0-9.]+)pt$/.exec(v);
			if (px !== null) style.sizePx = Math.round(Number(px[1]));
			else if (pt !== null) style.sizePx = Math.round((Number(pt[1]) * 4) / 3);
			else if (CSS_SIZE_LEVELS[v] !== undefined) {
				style.level = CSS_SIZE_LEVELS[v];
				style.sizePx = null;
			}
		} else if (name === 'text-decoration') {
			style.underline = v.includes('underline');
			style.strike = v.includes('line-through');
		} else if (name === 'white-space') {
			style.pre = v === 'pre' || v === 'pre-wrap';
			style.nowrap = v === 'nowrap' || v === 'pre';
		} else if (name === 'vertical-align') {
			style.script = v === 'sub' ? -1 : v === 'super' ? 1 : 0;
		}
	}
}

function newBlock(indent) {
	return {
		runs: [],
		top: 0,
		bottom: 0,
		left: indent.left,
		right: indent.right,
		align: null,
		textIndent: 0,
		lineHeight: null, // { px } or { factor }
		marker: null, // { ordered, number, type } on the first block of a list item
		rule: false,
		fresh: true, // nothing was added after it was made by a list item or a quote
	};
}

function applyBlockCss(block, css) {
	const px = (value) => {
		const m = /^(-?[0-9.]+)(px)?$/.exec(String(value).trim());
		return m === null ? null : Number(m[1]);
	};
	for (const [name, value] of Object.entries(css)) {
		if (name === 'margin-top') block.top = px(value) ?? block.top;
		else if (name === 'margin-bottom') block.bottom = px(value) ?? block.bottom;
		else if (name === 'margin-left') block.left += px(value) ?? 0;
		else if (name === 'margin-right') block.right += px(value) ?? 0;
		else if (name === 'text-indent') block.textIndent = px(value) ?? 0;
		else if (name === 'text-align') block.align = value.toLowerCase();
		else if (name === 'line-height') {
			const v = value.toLowerCase();
			if (/%$/.test(v)) block.lineHeight = { factor: parseFloat(v) / 100 };
			else if (/px$/.test(v)) block.lineHeight = { px: parseFloat(v) };
			else if (Number.isFinite(Number(v)) && Number(v) > 0) block.lineHeight = { factor: Number(v) };
		}
	}
}

// Parses `source` into { blocks, base }; `base` is what the <body> tag set (font-size, font-family, color). `styled` is StyledText: the
// small tag set, blocks are only line breaks (no margins, no indent), the break that ends a paragraph or a heading is `synthetic`.
export function parseRichText(source, styled) {
	const tokens = tokenize(source);
	const blocks = [];
	const base = {};
	const styles = [baseStyle()];
	const style = () => styles[styles.length - 1];
	const lists = []; // { ordered, number, marginBottom, type }
	let indent = { left: 0, right: 0 };
	const indents = [];
	let current = null;
	let pendingTop = 0; // the margin that a quote asks above the next block
	let listTop = 0; // the margin that a list asks above its first item
	let afterSpace = false;
	let atLineStart = true;
	let preDepth = 0;
	const open = []; // the open elements: { tag, pushed }

	const addBlock = (block) => {
		block.top = Math.max(block.top, pendingTop);
		pendingTop = 0;
		blocks.push(block);
		current = block;
		afterSpace = false;
		atLineStart = true;
		return block;
	};
	const ensureBlock = () => current !== null ? current : addBlock(newBlock(indent));
	const hasContent = (block) => block.runs.length > 0;

	const pushBreak = (synthetic = false, spacer = false) => {
		const block = ensureBlock();
		block.runs.push({ br: true, synthetic, spacer, style: { ...style() } });
		afterSpace = false;
		atLineStart = true;
	};

	const pushText = (text) => {
		if (text === '') return;
		const s = style();
		const keep = preDepth > 0 || s.pre;
		if (keep) {
			const parts = text.split('\n');
			for (let i = 0; i < parts.length; i++) {
				if (i > 0) pushBreak();
				if (parts[i] !== '') appendText(parts[i], s);
			}
			return;
		}
		let out = '';
		for (const ch of text) {
			if (ch === ' ' || ch === '\t' || ch === '\r' || ch === '\n' || ch === '\f') {
				if (!afterSpace && !atLineStart) out += ' ';
				afterSpace = true;
			} else {
				out += ch;
				afterSpace = false;
				atLineStart = false;
			}
		}
		if (out !== '') appendText(out, s);
	};

	const appendText = (text, s) => {
		const block = ensureBlock();
		atLineStart = false;
		const last = block.runs[block.runs.length - 1];
		if (last !== undefined && last.text !== undefined && sameStyle(last.style, s)) last.text += text;
		else block.runs.push({ text, style: { ...s } });
	};

	// A block that a tag starts; an empty block that a list item or a quote made is the one that a paragraph inside it is.
	const startBlock = (apply) => {
		let block;
		if (current !== null && !hasContent(current) && current.fresh) {
			block = current;
			block.top = Math.max(block.top, pendingTop);
			pendingTop = 0;
		} else {
			block = addBlock(newBlock(indent));
		}
		if (apply) apply(block);
		return block;
	};

	const clone = () => {
		const s = { ...style() };
		styles.push(s);
		return s;
	};

	const handleOpen = (tag, attributes) => {
		const css = attributes.style !== undefined ? parseCss(attributes.style) : null;
		const parent = style();
		const level = (delta) => Math.max(1, Math.min(7, parent.level + delta));
		switch (tag) {
			case 'b': case 'strong':
				clone().bold = true;
				return true;
			case 'i': case 'em': case 'cite': case 'var': case 'dfn':
				clone().italic = true;
				return true;
			case 'u': case 'ins':
				clone().underline = true;
				return true;
			case 's': case 'strike': case 'del':
				clone().strike = true;
				return true;
			case 'tt': case 'code': case 'kbd': case 'samp':
				clone().family = 'monospace';
				return true;
			case 'big': {
				const s = clone();
				s.level = level(1);
				s.sizePx = null;
				return true;
			}
			case 'small': {
				const s = clone();
				s.level = level(-1);
				s.sizePx = null;
				return true;
			}
			case 'sub':
				clone().script = -1;
				return true;
			case 'sup':
				clone().script = 1;
				return true;
			case 'nobr':
				clone().nowrap = true;
				return true;
			case 'a': {
				const s = clone();
				if (attributes.href !== undefined) {
					s.href = attributes.href;
					s.underline = true;
				}
				return true;
			}
			case 'font': {
				const s = clone();
				if (attributes.color !== undefined) s.color = parseColor(attributes.color) ?? s.color;
				if (attributes.face !== undefined && !styled) s.family = attributes.face.split(',')[0].trim();
				if (attributes.size !== undefined) {
					const text = attributes.size.trim();
					const n = parseInt(text, 10);
					if (Number.isFinite(n)) {
						s.level = Math.max(1, Math.min(7, text[0] === '+' || text[0] === '-' ? parent.level + n : n));
						s.sizePx = null;
					}
				}
				return true;
			}
			case 'span': {
				const s = clone();
				if (css !== null) applyCharacterCss(s, css);
				return true;
			}
			case 'body': case 'html':
				if (css !== null) {
					const tmp = baseStyle();
					applyCharacterCss(tmp, css);
					if (css['font-size'] !== undefined && tmp.sizePx !== null) base.sizePx = tmp.sizePx;
					if (css['font-family'] !== undefined) base.family = tmp.family;
					if (css.color !== undefined) base.color = tmp.color;
				}
				return false;
			case 'br':
				pushBreak();
				return false;
			case 'p': {
				if (styled) {
					if (current !== null && hasContent(current) && !atLineStart) pushBreak();
					return false;
				}
				startBlock((block) => {
					block.top = Math.max(block.top, 12);
					block.bottom = 12;
					block.fresh = false;
					if (attributes.align !== undefined) block.align = attributes.align.toLowerCase();
					if (css !== null) applyBlockCss(block, css);
				});
				if (css !== null) {
					const s = clone();
					applyCharacterCss(s, css);
					return true;
				}
				return false;
			}
			case 'div': case 'center': {
				if (styled) return false;
				// a <div> starts a block when the one in front has text; its end does not end the block (as in Qt)
				if (current !== null && hasContent(current)) current = null;
				if (current === null || tag === 'center' || attributes.align !== undefined || css !== null) startBlock(null);
				const block = current;
				block.fresh = false;
				if (tag === 'center') block.align = 'center';
				if (attributes.align !== undefined) block.align = attributes.align.toLowerCase();
				if (css !== null) {
					applyBlockCss(block, css);
					const s = clone();
					applyCharacterCss(s, css);
					return true;
				}
				return false;
			}
			case 'h1': case 'h2': case 'h3': case 'h4': case 'h5': case 'h6': {
				const h = HEADINGS[tag];
				if (styled && current !== null && hasContent(current)) {
					// the heading starts after an empty line: the breaks are of the text in front of it
					pushBreak();
					pushBreak(false, true);
				}
				const s = clone();
				s.level = h.level;
				s.sizePx = null;
				s.bold = true;
				if (!styled) {
					startBlock((block) => {
						block.top = Math.max(block.top, h.top);
						block.bottom = h.bottom;
						block.fresh = false;
						if (attributes.align !== undefined) block.align = attributes.align.toLowerCase();
						if (css !== null) applyBlockCss(block, css);
					});
					if (css !== null) applyCharacterCss(s, css);
				}
				return true;
			}
			case 'pre': {
				if (styled) return false;
				startBlock((block) => {
					block.top = Math.max(block.top, 12);
					block.bottom = 12;
					block.fresh = false;
				});
				const s = clone();
				s.family = 'monospace';
				s.pre = true;
				s.nowrap = true;
				preDepth++;
				atLineStart = false;
				return true;
			}
			case 'blockquote':
				if (styled) return false;
				current = null;
				indents.push(indent);
				indent = { left: indent.left + 40, right: indent.right + 40 };
				pendingTop = Math.max(pendingTop, 12);
				return false;
			case 'ul': case 'ol': {
				if (styled) return false;
				current = null;
				const margin = lists.length <= 1 ? 12 : 0;
				lists.push({ ordered: tag === 'ol', number: attributes.start !== undefined ? parseInt(attributes.start, 10) - 1 : 0, marginBottom: margin, type: (attributes.type || '').toLowerCase() });
				indents.push(indent);
				let steps = 1;
				if (css !== null && css['-qt-list-indent'] !== undefined) {
					const n = parseInt(css['-qt-list-indent'], 10);
					if (Number.isFinite(n) && n > 1) steps = n;
				}
				indent = { left: indent.left + 40 * steps, right: indent.right };
				listTop = Math.max(listTop, margin);
				return false;
			}
			case 'li': {
				if (styled) return false;
				current = null;
				const list = lists[lists.length - 1];
				const ordered = list !== undefined && list.ordered;
				const number = ordered ? ++list.number : 0;
				const block = addBlock(newBlock(indent));
				block.top = Math.max(block.top, listTop);
				listTop = 0;
				block.marker = { ordered, number, type: list !== undefined ? list.type : '' };
				if (css !== null) applyBlockCss(block, css);
				return false;
			}
			case 'dd':
				if (styled) return false;
				current = null;
				startBlock((block) => {
					block.left += 30;
					block.fresh = false;
				});
				return false;
			case 'dt': case 'dl': case 'tr':
				if (!styled) current = null;
				return false;
			case 'td': case 'th':
				if (!styled && current !== null && hasContent(current)) pushText(' ');
				return false;
			case 'img': {
				if (styled) return false;
				const width = parseFloat(attributes.width);
				const height = parseFloat(attributes.height);
				const block = ensureBlock();
				block.runs.push({ img: true, width: Number.isFinite(width) ? width : 0, height: Number.isFinite(height) ? height : 0, src: attributes.src || '', style: { ...style() } });
				atLineStart = false;
				afterSpace = false;
				return false;
			}
			case 'hr': {
				if (styled) return false;
				current = null;
				const block = addBlock(newBlock(indent));
				block.rule = true;
				block.fresh = false;
				current = null;
				return false;
			}
			default:
				return false;
		}
	};

	const handleClose = (tag) => {
		switch (tag) {
			case 'p':
				if (styled) {
					if (current !== null) pushBreak(true);
				} else current = null;
				break;
			case 'h1': case 'h2': case 'h3': case 'h4': case 'h5': case 'h6':
				if (styled) {
					if (current !== null) pushBreak(true);
				} else current = null;
				break;
			case 'li': case 'dd': case 'center':
				current = null;
				break;
			case 'pre':
				preDepth = Math.max(0, preDepth - 1);
				current = null;
				break;
			case 'blockquote':
				if (!styled) {
					current = null;
					indent = indents.pop() ?? { left: 0, right: 0 };
					if (blocks.length > 0) blocks[blocks.length - 1].bottom = Math.max(blocks[blocks.length - 1].bottom, 12);
				}
				break;
			case 'ul': case 'ol': {
				if (styled) break;
				current = null;
				const list = lists.pop();
				indent = indents.pop() ?? { left: 0, right: 0 };
				if (blocks.length > 0 && list !== undefined) blocks[blocks.length - 1].bottom = Math.max(blocks[blocks.length - 1].bottom, list.marginBottom);
				break;
			}
			default:
				break;
		}
	};

	for (const token of tokens) {
		if (token.text !== undefined) {
			pushText(decodeEntities(token.text));
			continue;
		}
		const { tag, close, attributes } = token;
		if (styled && !STYLED_TAGS.has(tag)) continue;
		if (!close) {
			const pushed = handleOpen(tag, attributes);
			if (!VOID_TAGS.has(tag)) open.push({ tag, pushed });
			continue;
		}
		if (VOID_TAGS.has(tag)) continue;
		let at = -1;
		for (let i = open.length - 1; i >= 0; i--) {
			if (open[i].tag === tag) {
				at = i;
				break;
			}
		}
		if (at < 0) continue;
		// the elements that were opened after this one end with it
		while (open.length > at) {
			const element = open.pop();
			if (element.pushed) styles.pop();
			handleClose(element.tag);
		}
	}

	return { blocks, base };
}

function sameStyle(a, b) {
	for (const key of Object.keys(a)) if (a[key] !== b[key]) return false;
	return true;
}

// ----- layout -----

// options: { fonts, family, size, bold, italic, width (0: no wrapping), wrap ('none' | 'word' | 'anywhere'), styled, lineHeightFactor, align }
// Returns { lines, blocks, contentWidth, contentHeight, implicitHeight, naturalWidth, links }.
export function layoutRichText(doc, options) {
	const { fonts, styled } = options;
	const baseSize = doc.base.sizePx ?? options.size;
	const baseFamily = doc.base.family ?? options.family;
	const factor = options.lineHeightFactor > 0 ? options.lineHeightFactor : 1;

	const infoOf = (style, scaled) => {
		let size = style.sizePx ?? Math.round(baseSize * SIZE_FACTORS[style.level]);
		if (scaled && style.script !== 0) size = Math.round((size * 2) / 3);
		const family = style.family !== null ? style.family : baseFamily;
		return fonts.get(family, Math.max(1, size), style.bold || options.bold, style.italic || options.italic);
	};
	const heightOf = (info) => (styled ? info.lineHeight : info.lineSpacing);

	// Blocks without content are dropped, but a text that has none keeps one empty line
	const blocks = doc.blocks.filter((block) => block.runs.length > 0 || block.rule);
	const result = { lines: [], blocks: [], contentWidth: 0, contentHeight: 0, implicitHeight: 0, naturalWidth: 0, links: [], baseSize, baseFamily, baseColor: doc.base.color ?? null };
	if (blocks.length === 0) {
		const info = infoOf(baseStyle(), false);
		const height = styled ? info.lineHeight : info.lineSpacing;
		result.empty = true;
		result.contentHeight = result.implicitHeight = height;
		return result;
	}

	const avail = options.width > 0 && options.wrap !== 'none' ? options.width : Infinity;
	let y = 0;
	let previous = null;
	for (const block of blocks) {
		if (previous !== null) y += Math.max(previous.bottom, block.top);
		const room = avail === Infinity ? Infinity : Math.max(1, avail - block.left - block.right);
		const laid = layoutBlock(block, room);
		const placed = { block, lines: laid.lines, y, left: block.left, right: block.right, height: laid.height, width: laid.width, natural: laid.natural, marker: block.marker, rule: block.rule };
		for (const line of laid.lines) {
			line.block = placed;
			line.y += y;
		}
		result.lines.push(...laid.lines);
		result.blocks.push(placed);
		y += laid.height;
		previous = block;
	}
	result.contentHeight = y;
	result.implicitHeight = y;
	// a break that ends a paragraph or a heading of StyledText is in the implicit height only
	const last = result.lines[result.lines.length - 1];
	if (styled && last !== undefined && last.trailingSynthetic) result.contentHeight -= last.height;

	let natural = 0;
	let widest = 0;
	for (const placed of result.blocks) {
		natural = Math.max(natural, placed.left + placed.natural + placed.right);
		widest = Math.max(widest, placed.left + placed.width);
	}
	result.naturalWidth = natural;
	result.contentWidth = widest;

	// where each line is: the offset of its alignment
	const box = options.width > 0 ? options.width : widest;
	for (const line of result.lines) {
		const placed = line.block;
		const align = placed.block.align !== null ? placed.block.align : options.align;
		const space = box - placed.left - placed.right - line.width;
		let shift = 0;
		if (align === 'right' || align === 2) shift = space;
		else if (align === 'center' || align === 4 || align === 132) shift = space / 2;
		line.x = placed.left + shift + line.indent;
		let pen = line.x;
		for (const item of line.items) {
			item.x = pen;
			pen += item.width;
			if (item.style.href !== null) result.links.push({ href: item.style.href, x: item.x, y: line.y, width: item.width, height: line.height });
		}
	}
	return result;

	function layoutBlock(block, room) {
		const lines = [];
		let line = newLine(block, true);
		const finishLine = (synthetic = false) => {
			while (line.items.length > 0 && line.items[line.items.length - 1].space) {
				const item = line.items.pop();
				line.width -= item.width;
			}
			let height = line.height > 0 ? line.height : heightOf(infoOf(line.emptyStyle ?? baseStyle(), false));
			if (block.lineHeight !== null) height = block.lineHeight.px !== undefined ? block.lineHeight.px : height * block.lineHeight.factor;
			height *= factor;
			line.height = height;
			line.trailingSynthetic = synthetic === true;
			lines.push(line);
		};

		for (const run of block.runs) {
			if (run.br) {
				line.emptyStyle = run.style;
				line.spacer = run.spacer === true;
				finishLine();
				line = newLine(block, false);
				line.emptyStyle = run.style;
				line.afterSynthetic = run.synthetic === true;
				continue;
			}
			if (run.img) {
				if (room !== Infinity && line.items.length > 0 && line.width + run.width > room + 0.001) {
					finishLine();
					line = newLine(block, false);
				}
				line.height = Math.max(line.height, run.height);
				line.items.push({ text: '', img: true, style: run.style, info: infoOf(run.style, true), width: run.width, space: false, x: 0 });
				line.width += run.width;
				continue;
			}
			const info = infoOf(run.style, true);
			const lineInfo = infoOf(run.style, false);
			const unbreakable = run.style.nowrap || run.style.pre;
			const atoms = unbreakable ? [{ text: run.text, space: false }] : splitAtoms(run.text);
			for (const atom of atoms) {
				const width = textWidth(info, atom.text);
				if (atom.space) {
					if (line.items.length === 0) continue;
					addItem(line, run, info, lineInfo, atom.text, width, true);
					continue;
				}
				if (room !== Infinity && line.items.length > 0 && line.width + width > room + 0.001 && !run.style.nowrap) {
					finishLine();
					line = newLine(block, false);
				}
				if (room !== Infinity && width > room + 0.001 && options.wrap === 'anywhere' && !unbreakable) {
					let rest = Array.from(atom.text);
					while (rest.length > 0) {
						let count = 0;
						let used = 0;
						while (count < rest.length && (count === 0 || line.width + used + info.advance(rest[count]) <= room + 0.001)) {
							used += info.advance(rest[count]);
							count++;
						}
						addItem(line, run, info, lineInfo, rest.slice(0, count).join(''), used, false);
						rest = rest.slice(count);
						if (rest.length > 0) {
							finishLine();
							line = newLine(block, false);
						}
					}
					continue;
				}
				addItem(line, run, info, lineInfo, atom.text, width, false);
			}
		}
		// the last line: a block that ended with a break keeps its empty line
		const endsWithBreak = block.runs.length > 0 && block.runs[block.runs.length - 1].br === true;
		if (line.items.length > 0 || endsWithBreak || lines.length === 0) {
			if (line.items.length === 0 && endsWithBreak && line.afterSynthetic) line.trailingSynthetic = true;
			finishLine(line.trailingSynthetic);
		}

		let height = 0;
		let width = 0;
		for (const l of lines) {
			l.y = height;
			height += l.height;
			width = Math.max(width, l.width + l.indent);
		}
		// the natural width: the same text without wrapping
		const natural = room === Infinity ? width : layoutBlock(block, Infinity).width;
		return { lines, height, width, natural };
	}

	function newLine(block, first) {
		return { items: [], width: 0, height: 0, indent: first ? block.textIndent : 0, x: 0, y: 0, emptyStyle: null, trailingSynthetic: false, afterSynthetic: false, spacer: false };
	}

	function addItem(line, run, info, lineInfo, text, width, space) {
		const h = heightOf(lineInfo);
		line.height = Math.max(line.height, run.style.script !== 0 ? heightOf(infoOf({ ...run.style, script: 0 }, false)) : h);
		line.items.push({ text, style: run.style, info, width, space, x: 0 });
		line.width += width;
	}
}

function textWidth(info, text) {
	let width = 0;
	for (const ch of text) width += info.advance(ch);
	return width;
}

// The words and the spaces of a text (a no-break space is a letter of a word).
function splitAtoms(text) {
	const atoms = [];
	const re = / +|[^ ]+/g;
	let m;
	while ((m = re.exec(text)) !== null) atoms.push({ text: m[0], space: m[0][0] === ' ' });
	return atoms;
}

// ----- paint and links -----

// The link at a point of the text (relative to its top left corner), or ''.
export function linkAtPoint(layout, x, y) {
	for (const link of layout.links) {
		if (x >= link.x && x < link.x + link.width && y >= link.y && y < link.y + link.height) return link.href;
	}
	return '';
}

// Paints the lines: `r` is the renderer; `pack(color)` makes the color of a CSS color string, `base` the color of the text, `linkColor`
// the color of links. A bullet is a small quad, a number is text.
export function paintRichText(r, layout, options) {
	const { pack, baseColor, linkColor, fonts, bold, italic } = options;
	if (layout.empty) return;
	const baseSize = layout.baseSize;
	const family = layout.baseFamily;
	const infoFor = (style, scaled) => {
		let px = style.sizePx ?? Math.round(baseSize * SIZE_FACTORS[style.level]);
		if (scaled && style.script !== 0) px = Math.round((px * 2) / 3);
		return fonts.get(style.family !== null ? style.family : family, Math.max(1, px), style.bold || bold, style.italic || italic);
	};
	for (const placed of layout.blocks) {
		if (placed.rule) {
			const mid = placed.y + placed.height / 2;
			r.fillQuad(placed.left, mid, Math.max(1, (options.width > 0 ? options.width : layout.contentWidth) - placed.left - placed.right), 1, baseColor);
			continue;
		}
		if (placed.marker !== null && placed.lines.length > 0) paintMarker(placed);
	}
	for (const line of layout.lines) {
		let ascent = 0;
		for (const item of line.items) {
			const info = infoFor({ ...item.style, script: 0 }, false);
			ascent = Math.max(ascent, info.ascent);
		}
		if (line.items.length === 0) continue;
		const extra = 0;
		for (const item of line.items) {
			if (item.space && item === line.items[line.items.length - 1]) continue;
			const style = item.style;
			const color = style.href !== null ? linkColor : style.color !== null ? pack(style.color) : baseColor;
			const info = item.info;
			let baseline = line.y + ascent + extra;
			if (style.script === -1) baseline += info.size * 0.35;
			else if (style.script === 1) baseline -= info.size * 0.55;
			if (style.background !== null && !item.space) r.fillQuad(item.x, line.y, item.width, line.height, pack(style.background));
			if (!item.space) {
				let pen = item.x;
				for (const ch of item.text) {
					const glyph = info.glyph(ch);
					if (glyph !== null) r.glyph(glyph, pen, baseline, color);
					pen += info.advance(ch);
				}
			}
			if (style.underline) r.fillQuad(item.x, line.y + (info.underlineY - info.ascent) + ascent, item.width, info.lineWidth, color);
			if (style.strike) r.fillQuad(item.x, line.y + (info.strikeY - info.ascent) + ascent, item.width, info.lineWidth, color);
		}
	}

	function paintMarker(placed) {
		const first = placed.lines[0];
		const item = first.items[0];
		const info = item !== undefined ? item.info : infoFor(baseStyleOf(), false);
		const marker = placed.marker;
		const sizePx = info.size;
		const top = first.y;
		if (marker.ordered) {
			let label = `${marker.number}.`;
			if (marker.type === 'a') label = `${String.fromCharCode(96 + ((marker.number - 1) % 26) + 1)}.`;
			else if (marker.type === 'i') label = `${toRoman(marker.number).toLowerCase()}.`;
			let width = 0;
			for (const ch of label) width += info.advance(ch);
			let pen = placed.left - width - sizePx * 0.3;
			let ascent = 0;
			ascent = info.ascent;
			for (const ch of label) {
				const glyph = info.glyph(ch);
				if (glyph !== null) r.glyph(glyph, pen, top + ascent, baseColor);
				pen += info.advance(ch);
			}
			return;
		}
		const dot = Math.max(2, Math.round(sizePx * 0.3));
		const cy = top + first.height / 2 - dot / 2;
		const cx = placed.left - sizePx * 0.9;
		if (marker.type === 'circle') {
			const t = Math.max(1, Math.round(dot / 4));
			r.fillQuad(cx, cy, dot, t, baseColor);
			r.fillQuad(cx, cy + dot - t, dot, t, baseColor);
			r.fillQuad(cx, cy, t, dot, baseColor);
			r.fillQuad(cx + dot - t, cy, t, dot, baseColor);
		} else {
			r.fillQuad(cx, cy, dot, dot, baseColor);
		}
	}

	function baseStyleOf() {
		return baseStyle();
	}
}

function toRoman(value) {
	const table = [[1000, 'M'], [900, 'CM'], [500, 'D'], [400, 'CD'], [100, 'C'], [90, 'XC'], [50, 'L'], [40, 'XL'], [10, 'X'], [9, 'IX'], [5, 'V'], [4, 'IV'], [1, 'I']];
	let n = Math.max(1, value);
	let out = '';
	for (const [v, s] of table) {
		while (n >= v) {
			out += s;
			n -= v;
		}
	}
	return out;
}
