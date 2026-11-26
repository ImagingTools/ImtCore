// A tolerant scan of QML text. The JQML v4 parser stops at the first error, so everything the editor needs while a
// file is being typed (objects, ids, declarations, the statement under the cursor) comes from this token scan.
//
// scan = { tokens, imports, frames, ids, idDecls, root }
// frame = { index, kind: 'object' | 'group' | 'script', type, typeToks, start, end, parent, depth, id, decls, assigns,
//           stmtStarts, propertyName, onProperty }
// decl = { kind: 'property' | 'signal' | 'method' | 'enum', name, typeName, params, tok }

import { continuesAcrossNewline, QmlError, tokenize } from '../lib/lexer.js';

const NOT_GROUPS = new Set(['else', 'try', 'finally', 'do', 'return', 'default', 'case']);

export const isUpper = (text) => text.length > 0 && text[0] >= 'A' && text[0] <= 'Z';

export function safeTokenize(text) {
	let source = text;
	for (let attempt = 0; attempt < 4; attempt++) {
		try {
			return tokenize(source);
		} catch (error) {
			if (!(error instanceof QmlError)) throw error;
			// Cut the text at the line of the error and try again: what precedes it is still usable.
			const lines = source.split('\n');
			const upto = Math.max(0, Math.min(lines.length, error.line - 1));
			source = lines.slice(0, upto).join('\n');
		}
	}
	return [{ t: 'eof', v: '', s: 0, e: 0, line: 1, col: 1, nl: true }];
}

function dottedBefore(tokens, last) {
	// The dotted name that ends at tokens[last]: [first index .. last].
	let first = last;
	while (first >= 2 && tokens[first - 1].t === 'p' && tokens[first - 1].v === '.' && tokens[first - 2].t === 'id') first -= 2;
	return first;
}

function parseImports(tokens) {
	const imports = [];
	let i = 0;
	while (tokens[i].t === 'id' && (tokens[i].v === 'import' || tokens[i].v === 'pragma')) {
		const keyword = tokens[i];
		const rest = [];
		let j = i + 1;
		while (tokens[j].t !== 'eof' && !tokens[j].nl) rest.push(tokens[j++]);
		if (keyword.v === 'import' && rest.length > 0) {
			const item = { kind: 'module', name: '', path: '', alias: '', version: null, line: keyword.line, tok: rest[0], endTok: rest[rest.length - 1] };
			let k = 0;
			if (rest[0].t === 'str') {
				item.path = rest[0].v.slice(1, -1);
				item.kind = item.path.endsWith('.js') ? 'js' : 'dir';
				k = 1;
			} else {
				while (k < rest.length && (rest[k].t === 'id' || (rest[k].t === 'p' && rest[k].v === '.')) && !(rest[k].t === 'id' && rest[k].v === 'as')) {
					item.name += rest[k].v;
					k++;
				}
			}
			for (; k < rest.length; k++) {
				if (rest[k].t === 'id' && rest[k].v === 'as' && rest[k + 1] !== undefined) item.alias = rest[++k].v;
				else if (rest[k].t === 'num' && item.version === null) {
					const parts = rest[k].v.split('.');
					item.version = { major: Number(parts[0]), minor: Number(parts[1] || 0) };
				}
			}
			imports.push(item);
		}
		i = j;
	}
	return { imports, next: i };
}

function readParams(tokens, open) {
	// `(a, b: int, int c)` -> { params, close } with the names.
	const params = [];
	let depth = 0;
	let part = [];
	let k = open;
	for (; k < tokens.length; k++) {
		const tk = tokens[k];
		if (tk.t === 'eof') break;
		if (tk.t === 'p' && (tk.v === '(' || tk.v === '<' || tk.v === '[')) {
			depth++;
			if (depth === 1) continue;
		} else if (tk.t === 'p' && (tk.v === ')' || tk.v === '>' || tk.v === ']')) {
			depth--;
			if (depth === 0) break;
		} else if (tk.t === 'p' && tk.v === ',' && depth === 1) {
			if (part.length > 0) params.push(part);
			part = [];
			continue;
		}
		if (depth >= 1) part.push(tk);
	}
	if (part.length > 0) params.push(part);
	const names = params.map((tokensOfPart) => {
		const colon = tokensOfPart.findIndex((tk) => tk.t === 'p' && tk.v === ':');
		if (colon > 0) return tokensOfPart[0].v;
		const ids = tokensOfPart.filter((tk) => tk.t === 'id');
		return ids.length > 0 ? ids[ids.length - 1].v : '';
	}).filter(Boolean);
	return { params: names, close: k };
}

export function scanText(text) {
	const tokens = safeTokenize(text);
	const { imports, next } = parseImports(tokens);
	const frames = [];
	const ids = new Map();
	const idDecls = [];
	const stack = [];
	const top = () => (stack.length > 0 ? stack[stack.length - 1] : null);

	const startStatement = (frame, index) => {
		frame.fresh = false;
		frame.stmtStarts.push(index);
	};

	for (let k = next; k < tokens.length; k++) {
		const tk = tokens[k];
		if (tk.t === 'eof') break;
		const frame = top();
		const inObject = frame !== null && frame.kind !== 'script';

		if (inObject) {
			if (frame.paren === 0 && frame.fresh === false && tk.nl && k > 0 && !(tokens[k - 1].t === 'p' && tokens[k - 1].v === '{') && !continuesAcrossNewline(tokens[k - 1], tk)) {
				frame.fresh = true;
			}
			if (tk.t === 'p' && tk.v === ';' && frame.paren === 0) {
				frame.fresh = true;
				continue;
			}
		}

		if (tk.t === 'p' && tk.v === '{') {
			const parent = frame;
			const prev = tokens[k - 1];
			const frameInfo = { index: frames.length, kind: 'script', type: '', typeToks: [], start: tk.s, startTok: k, end: Infinity, endTok: -1, parent: parent === null ? -1 : parent.index, depth: stack.length, id: '', decls: new Map(), assigns: [], stmtStarts: [], propertyName: '', onProperty: '', paren: 0, fresh: true };
			const canBeObject = parent === null || parent.kind !== 'script';
			if (canBeObject && k > next && prev.t === 'id' && !(tokens[k - 2] && tokens[k - 2].t === 'id' && tokens[k - 2].v === 'enum')) {
				let nameLast = k - 1;
				let onProperty = '';
				if (k >= 3 && tokens[k - 2].t === 'id' && tokens[k - 2].v === 'on' && tokens[k - 3].t === 'id') {
					onProperty = prev.v;
					nameLast = k - 3;
				}
				const first = dottedBefore(tokens, nameLast);
				const typeToks = tokens.slice(first, nameLast + 1).filter((item) => item.t === 'id');
				const name = typeToks.map((item) => item.v).join('.');
				if (isUpper(typeToks[0].v)) {
					frameInfo.kind = 'object';
					frameInfo.type = name;
					frameInfo.typeToks = typeToks;
					frameInfo.onProperty = onProperty;
				} else if (parent !== null && onProperty === '' && !NOT_GROUPS.has(prev.v) && typeToks.length === 1) {
					frameInfo.kind = 'group';
					frameInfo.type = name;
					frameInfo.typeToks = typeToks;
				}
			}
			if (frameInfo.kind !== 'script' && parent !== null && parent.kind !== 'script') {
				// `name: Type {`: the property the object is the value of.
				const start = parent.stmtStarts.length > 0 ? parent.stmtStarts[parent.stmtStarts.length - 1] : -1;
				if (start >= 0 && tokens[start].t === 'id') {
					let n = start;
					while (tokens[n + 1] && tokens[n + 1].t === 'p' && tokens[n + 1].v === '.' && tokens[n + 2] && tokens[n + 2].t === 'id') n += 2;
					if (tokens[n + 1] && tokens[n + 1].t === 'p' && tokens[n + 1].v === ':' && n < k) frameInfo.propertyName = tokens.slice(start, n + 1).filter((item) => item.t === 'id').map((item) => item.v).join('.');
				}
			}
			if (inObject) {
				if (frame.fresh) startStatement(frame, k);
			}
			frames.push(frameInfo);
			stack.push(frameInfo);
			continue;
		}

		if (tk.t === 'p' && tk.v === '}') {
			const closed = stack.pop();
			if (closed !== undefined) {
				closed.end = tk.s;
				closed.endTok = k;
				const parent = top();
				if (parent !== null && parent.kind !== 'script') {
					parent.fresh = true;
					parent.paren = 0;
				}
			}
			continue;
		}

		if (!inObject) continue;

		if (tk.t === 'p' && (tk.v === '(' || tk.v === '[')) {
			if (frame.fresh) startStatement(frame, k);
			frame.paren++;
			continue;
		}
		if (tk.t === 'p' && (tk.v === ')' || tk.v === ']')) {
			frame.paren = Math.max(0, frame.paren - 1);
			continue;
		}
		if (frame.paren > 0) continue;

		if (!frame.fresh) continue;
		startStatement(frame, k);
		if (tk.t !== 'id') continue;

		// ----- the statement begins at tokens[k] -----
		let at = k;
		while (tokens[at].t === 'id' && ['default', 'readonly', 'required'].includes(tokens[at].v) && tokens[at + 1].t === 'id' && ['property', 'readonly', 'required', 'default'].includes(tokens[at + 1].v)) at++;
		const word = tokens[at];
		const after = tokens[at + 1];
		const isColon = (t) => t !== undefined && t.t === 'p' && t.v === ':';

		if (word.v === 'id' && isColon(after) && tokens[at + 2].t === 'id') {
			const nameTok = tokens[at + 2];
			frame.id = nameTok.v;
			idDecls.push({ name: nameTok.v, tok: nameTok, frame: frame.index });
			if (!ids.has(nameTok.v)) ids.set(nameTok.v, { name: nameTok.v, tok: nameTok, frame: frame.index });
		} else if (word.v === 'property' && after.t === 'id') {
			let typeTok = after;
			let nameIndex = at + 2;
			let typeName = typeTok.v;
			if (tokens[nameIndex].t === 'p' && tokens[nameIndex].v === '<') {
				let depth = 0;
				while (tokens[nameIndex].t !== 'eof') {
					if (tokens[nameIndex].v === '<') depth++;
					else if (tokens[nameIndex].v === '>' && --depth === 0) break;
					typeName += tokens[nameIndex].v;
					nameIndex++;
				}
				typeName += '>';
				nameIndex++;
			}
			if (tokens[nameIndex].t === 'id') {
				const decl = { kind: 'property', name: tokens[nameIndex].v, typeName, tok: tokens[nameIndex], typeTok, params: [] };
				frame.decls.set(decl.name, decl);
			}
		} else if (word.v === 'signal' && after.t === 'id') {
			const decl = { kind: 'signal', name: after.v, typeName: '', tok: after, params: [] };
			if (tokens[at + 2].t === 'p' && tokens[at + 2].v === '(') decl.params = readParams(tokens, at + 2).params;
			frame.decls.set(decl.name, decl);
		} else if (word.v === 'function' && after.t === 'id') {
			const decl = { kind: 'method', name: after.v, typeName: '', tok: after, params: [] };
			if (tokens[at + 2].t === 'p' && tokens[at + 2].v === '(') decl.params = readParams(tokens, at + 2).params;
			frame.decls.set(decl.name, decl);
		} else if (word.v === 'enum' && after.t === 'id') {
			frame.decls.set(after.v, { kind: 'enum', name: after.v, typeName: '', tok: after, params: [], values: [] });
		} else {
			// `name: value` or `a.b.c: value`.
			let n = at;
			while (tokens[n + 1].t === 'p' && tokens[n + 1].v === '.' && tokens[n + 2].t === 'id') n += 2;
			if (isColon(tokens[n + 1])) {
				const nameToks = tokens.slice(at, n + 1).filter((item) => item.t === 'id');
				frame.assigns.push({ name: nameToks.map((item) => item.v).join('.'), toks: nameToks, tok: tokens[at] });
			}
		}
	}

	const objects = frames.filter((frame) => frame.kind === 'object');
	const root = objects.find((frame) => frame.parent === -1) || null;
	return { tokens, imports, frames, ids, idDecls, root };
}

// The nearest frame (self or ancestor) that is an object or a group.
export function objectFrameOf(scan, frame) {
	let current = frame;
	while (current !== null && current !== undefined && current.kind === 'script') current = current.parent >= 0 ? scan.frames[current.parent] : null;
	return current || null;
}

export function parentFrame(scan, frame) {
	return frame.parent >= 0 ? scan.frames[frame.parent] : null;
}

// The innermost frame around the offset (the offset is inside the braces), or null.
export function frameAt(scan, offset) {
	let best = null;
	for (const frame of scan.frames) {
		if (frame.start < offset && offset <= frame.end && (best === null || frame.depth >= best.depth)) best = frame;
	}
	return best;
}

// The index of the token that contains the offset (a token that ends at the offset counts), or -1.
export function tokenIndexAt(scan, offset) {
	const tokens = scan.tokens;
	let lo = 0;
	let hi = tokens.length - 1;
	while (lo <= hi) {
		const mid = (lo + hi) >> 1;
		const tk = tokens[mid];
		if (tk.e < offset) lo = mid + 1;
		else if (tk.s > offset) hi = mid - 1;
		else {
			// the offset touches this token; prefer the one that starts at the offset
			if (tk.e === offset && tokens[mid + 1] !== undefined && tokens[mid + 1].s === offset && tokens[mid + 1].t !== 'eof') return mid + 1;
			return tk.t === 'eof' ? -1 : mid;
		}
	}
	return -1;
}

// What stands before the cursor in the statement of an object frame: the tokens since the statement began.
export function statementBefore(scan, frame, beforeOffset) {
	const tokens = scan.tokens;
	let begin = -1;
	for (const start of frame.stmtStarts) {
		if (tokens[start].s < beforeOffset) begin = start;
		else break;
	}
	if (begin < 0) return [];
	const out = [];
	for (let k = begin; k < tokens.length && tokens[k].e <= beforeOffset; k++) out.push(tokens[k]);
	if (out.length === 0) return out;
	// A statement that ended before the cursor (a closed child object, or a finished line) does not continue.
	const last = out[out.length - 1];
	if (last.t === 'p' && last.v === '}' ) return [];
	return out;
}
