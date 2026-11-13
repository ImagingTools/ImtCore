// A JavaScript-aware tokenizer. QML structure is parsed from these tokens; script fragments
// (expressions, handler bodies, functions) are only delimited, then run by the JS engine of the
// browser.

export class QmlError extends Error {
	constructor(message, line = 0, column = 0) {
		super(message);
		this.name = 'QmlError';
		this.line = line;
		this.column = column;
	}
}

// Token kinds: id, num, str, tpl (template literal), re (regular expression), p (punctuator), eof.
// Fields: t kind, v text, s/e source range, line/col position, nl (a line break precedes it).

const REGEX_PREFIX_KEYWORDS = new Set([
	'return', 'typeof', 'instanceof', 'in', 'of', 'new', 'delete', 'void', 'throw', 'case', 'do', 'else', 'yield', 'await',
]);

const PUNCT3 = new Set(['...', '===', '!==', '**=', '<<=', '>>=', '>>>', '&&=', '||=', '??=']);
const PUNCT2 = new Set([
	'=>', '==', '!=', '<=', '>=', '&&', '||', '??', '?.', '++', '--', '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<', '>>', '**',
]);

const ID_RE = /[A-Za-z0-9_$\u0080-\uffff]+/y;
const NUM_RE = /0[xX][0-9a-fA-F_]+n?|0[bB][01_]+n?|0[oO][0-7_]+n?|(?:\d[\d_]*\.?[\d_]*|\.\d[\d_]*)(?:[eE][+-]?\d+)?n?/y;

const isDigit = (c) => c >= 48 && c <= 57;
const isIdStart = (c) => (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c === 95 || c === 36 || c > 127;

// A raw line break inside a QML string is legal; the JS parser needs it escaped.
// Only string tokens are touched: quotes inside comments, regular expressions (`/^['"]/`) and
// template literals are not string delimiters, so the fragment is tokenized rather than scanned.
export function escapeSourceBreaks(code) {
	if (!/[\n\r]/.test(code)) return code;
	let out = '';
	let at = 0;
	for (const token of tokenize(code)) {
		if (token.t !== 'str' || !/[\n\r]/.test(token.v)) continue;
		out += code.slice(at, token.s) + escapeStringBreaks(token.v);
		at = token.e;
	}
	return out + code.slice(at);
}

// A backslash keeps the character after it (`\` + line break is a line continuation).
function escapeStringBreaks(text) {
	let out = '';
	for (let i = 0; i < text.length; i++) {
		const c = text.charCodeAt(i);
		if (c === 92) {
			out += text[i];
			if (i + 1 < text.length) out += text[++i];
			continue;
		}
		if (c === 13) {
			out += '\\n';
			if (text.charCodeAt(i + 1) === 10) i++;
			continue;
		}
		if (c === 10) {
			out += '\\n';
			continue;
		}
		out += text[i];
	}
	return out;
}

export function tokenize(src) {
	const n = src.length;
	let i = 0;
	let line = 1;
	let lineStart = 0;
	let nl = false;
	let last = null;
	const tokens = [];

	const fail = (message, pos = i) => {
		throw new QmlError(message, line, pos - lineStart + 1);
	};

	function skipSpace() {
		while (i < n) {
			const c = src.charCodeAt(i);
			if (c === 10) {
				line++;
				lineStart = i + 1;
				nl = true;
				i++;
			} else if (c === 32 || c === 9 || c === 13 || c === 11 || c === 12 || c === 0xa0 || c === 0xfeff) {
				i++;
			} else if (c === 47) {
				const d = src.charCodeAt(i + 1);
				if (d === 47) {
					while (i < n && src.charCodeAt(i) !== 10) i++;
				} else if (d === 42) {
					const end = src.indexOf('*/', i + 2);
					if (end < 0) fail('Unterminated comment');
					for (let k = i; k < end; k++) {
						if (src.charCodeAt(k) === 10) {
							line++;
							lineStart = k + 1;
							nl = true;
						}
					}
					i = end + 2;
				} else {
					break;
				}
			} else {
				break;
			}
		}
	}

	function readString(quote) {
		const q = quote.charCodeAt(0);
		i++;
		while (i < n) {
			const c = src.charCodeAt(i);
			if (c === 92) {
				if (src.charCodeAt(i + 1) === 10) {
					line++;
					lineStart = i + 2;
				}
				i += 2;
				continue;
			}
			if (c === q) {
				i++;
				return;
			}
			// QML strings may contain a raw line break; it is part of the value.
			if (c === 10 || c === 13) {
				if (c === 13 && src.charCodeAt(i + 1) === 10) i++;
				line++;
				i++;
				lineStart = i;
				continue;
			}
			i++;
		}
		fail('Unterminated string');
	}

	function readTemplate() {
		i++;
		while (i < n) {
			const c = src.charCodeAt(i);
			if (c === 92) {
				if (src.charCodeAt(i + 1) === 10) {
					line++;
					lineStart = i + 2;
				}
				i += 2;
				continue;
			}
			if (c === 96) {
				i++;
				return;
			}
			if (c === 36 && src.charCodeAt(i + 1) === 123) {
				i += 2;
				let depth = 1;
				while (depth > 0) {
					const t = readToken();
					if (t === null) fail('Unterminated template');
					if (t.t === 'p') {
						if (t.v === '{') depth++;
						else if (t.v === '}') depth--;
					}
				}
				continue;
			}
			if (c === 10) {
				line++;
				lineStart = i + 1;
			}
			i++;
		}
		fail('Unterminated template');
	}

	function readRegex() {
		i++;
		let inClass = false;
		while (i < n) {
			const c = src.charCodeAt(i);
			if (c === 10) fail('Unterminated regular expression');
			if (c === 92) {
				i += 2;
				continue;
			}
			if (c === 91) inClass = true;
			else if (c === 93) inClass = false;
			else if (c === 47 && !inClass) {
				i++;
				while (i < n && isIdStart(src.charCodeAt(i))) i++;
				return;
			}
			i++;
		}
		fail('Unterminated regular expression');
	}

	function regexAllowed(newlineBefore) {
		if (last === null) return true;
		if (last.t === 'p') {
			if (last.v === ')' || last.v === ']') return false;
			if (last.v === '}') return newlineBefore;
			return true;
		}
		if (last.t === 'id') return REGEX_PREFIX_KEYWORDS.has(last.v);
		return false;
	}

	function readToken() {
		skipSpace();
		if (i >= n) return null;
		const tokenNl = nl;
		nl = false;
		const start = i;
		const startLine = line;
		const col = i - lineStart + 1;
		const c = src.charCodeAt(i);
		let t;
		if (isIdStart(c)) {
			ID_RE.lastIndex = i;
			ID_RE.exec(src);
			i = ID_RE.lastIndex;
			t = 'id';
		} else if (isDigit(c) || (c === 46 && isDigit(src.charCodeAt(i + 1)))) {
			NUM_RE.lastIndex = i;
			const m = NUM_RE.exec(src);
			if (m === null) fail('Invalid number');
			i = NUM_RE.lastIndex;
			t = 'num';
		} else if (c === 34 || c === 39) {
			readString(src[i]);
			t = 'str';
		} else if (c === 96) {
			readTemplate();
			nl = false;
			t = 'tpl';
		} else if (c === 47 && regexAllowed(tokenNl)) {
			readRegex();
			t = 're';
		} else {
			let len = 1;
			if (src.startsWith('>>>=', i)) {
				len = 4;
			} else if (PUNCT3.has(src.substr(i, 3))) {
				len = 3;
			} else if (PUNCT2.has(src.substr(i, 2))) {
				len = 2;
				if (src.startsWith('?.', i) && isDigit(src.charCodeAt(i + 2))) len = 1;
			}
			i += len;
			t = 'p';
		}
		const token = { t, v: src.slice(start, i), s: start, e: i, line: startLine, col, nl: tokenNl };
		last = token;
		return token;
	}

	for (let token = readToken(); token !== null; token = readToken()) {
		tokens.push(token);
	}
	tokens.push({ t: 'eof', v: '', s: n, e: n, line, col: n - lineStart + 1, nl: true });
	return tokens;
}

// ----- Where does an expression end? -----
// An expression written after `name:` ends at `;`, at a closing bracket of the enclosing object or,
// when a line break follows, unless the line obviously continues (a trailing operator, or a leading
// operator on the next line).

const PREV_CONTINUES = new Set([
	'+', '-', '*', '/', '%', '**', '=', '+=', '-=', '*=', '/=', '%=', '**=', '==', '!=', '===', '!==', '<', '>', '<=', '>=',
	'&&', '||', '??', '!', '~', '?', ':', ',', '.', '?.', '=>', '&', '|', '^', '<<', '>>', '>>>', '&=', '|=', '^=', '<<=', '>>=',
	'&&=', '||=', '??=', '...', '(', '[', '{',
]);
const PREV_KEYWORDS = new Set(['typeof', 'new', 'void', 'delete', 'in', 'instanceof', 'await', 'yield', 'return', 'throw']);
const NEXT_CONTINUES = new Set([
	'.', '?.', '?', ':', '+', '-', '*', '/', '%', '**', '==', '!=', '===', '!==', '<', '>', '<=', '>=', '&&', '||', '??',
	'&', '|', '^', '<<', '>>', '>>>', ',', '=', '=>', '(', '[', '+=', '-=', '*=', '/=', '%=',
]);

export function continuesAcrossNewline(prev, next) {
	if (prev.t === 'p' && PREV_CONTINUES.has(prev.v)) return true;
	if (prev.t === 'id' && PREV_KEYWORDS.has(prev.v)) return true;
	if (next.t === 'p' && NEXT_CONTINUES.has(next.v)) return true;
	if (next.t === 'id' && (next.v === 'instanceof' || next.v === 'in')) return true;
	return false;
}

// Names declared at the top level of a JavaScript file (var/let/const, function, class).
export function topLevelNames(tokens) {
	const names = [];
	let depth = 0;
	let inDeclaration = false;
	for (let k = 0; k < tokens.length; k++) {
		const tk = tokens[k];
		if (tk.t === 'eof') break;
		if (tk.t === 'p') {
			if (tk.v === '(' || tk.v === '[' || tk.v === '{') depth++;
			else if (tk.v === ')' || tk.v === ']' || tk.v === '}') depth--;
			else if (depth === 0 && tk.v === ';') inDeclaration = false;
			else if (depth === 0 && tk.v === ',' && inDeclaration) {
				const nameToken = tokens[k + 1];
				if (nameToken && nameToken.t === 'id') names.push(nameToken.v);
			}
			continue;
		}
		if (depth !== 0 || tk.t !== 'id') continue;
		if (inDeclaration && tk.nl && k > 0 && !continuesAcrossNewline(tokens[k - 1], tk)) {
			inDeclaration = false;
		}
		if (tk.v === 'function' || tk.v === 'class') {
			let m = k + 1;
			if (tokens[m] && tokens[m].v === '*') m++;
			if (tokens[m] && tokens[m].t === 'id') names.push(tokens[m].v);
		} else if (tk.v === 'var' || tk.v === 'let' || tk.v === 'const') {
			const nameToken = tokens[k + 1];
			if (nameToken && nameToken.t === 'id') names.push(nameToken.v);
			inDeclaration = true;
		}
	}
	return [...new Set(names)];
}
