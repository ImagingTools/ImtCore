// Makes the text of a script fragment of a QML document smaller without changing what it does: the comments go, the runs of white
// space shrink to what keeps the tokens apart, and a line break stays a line break (the rules of automatic semicolons depend on it).
// Strings, template literals and regular expressions are copied as they are. Names are not changed.

const WORD = /[A-Za-z0-9_$\u0080-￿]/;

// After these words a `/` begins a regular expression, not a division.
const REGEX_AFTER_WORD = new Set(['return', 'typeof', 'instanceof', 'in', 'of', 'new', 'delete', 'void', 'throw', 'case', 'do', 'else', 'yield', 'await']);

function skipString(text, at) {
	const quote = text[at];
	let i = at + 1;
	while (i < text.length && text[i] !== quote) i += text[i] === '\\' ? 2 : 1;
	return Math.min(i + 1, text.length);
}

// The end of the template literal that begins at `at` (the index after the closing backtick).
function skipTemplate(text, at) {
	let i = at + 1;
	while (i < text.length) {
		const c = text[i];
		if (c === '\\') {
			i += 2;
		} else if (c === '`') {
			return i + 1;
		} else if (c === '$' && text[i + 1] === '{') {
			i = skipBraces(text, i + 1);
		} else {
			i++;
		}
	}
	return text.length;
}

// The end of the `{ ... }` that begins at `at`, with strings, templates and comments inside it.
function skipBraces(text, at) {
	let depth = 0;
	let i = at;
	while (i < text.length) {
		const c = text[i];
		if (c === '"' || c === "'") {
			i = skipString(text, i);
			continue;
		}
		if (c === '`') {
			i = skipTemplate(text, i);
			continue;
		}
		if (c === '/' && text[i + 1] === '/') {
			while (i < text.length && text[i] !== '\n') i++;
			continue;
		}
		if (c === '/' && text[i + 1] === '*') {
			const close = text.indexOf('*/', i + 2);
			i = close < 0 ? text.length : close + 2;
			continue;
		}
		if (c === '{') depth++;
		else if (c === '}' && --depth === 0) return i + 1;
		i++;
	}
	return text.length;
}

// The end of the regular expression literal that begins at `at`, or -1 when it is not one (a division).
function skipRegex(text, at) {
	let i = at + 1;
	let inClass = false;
	while (i < text.length) {
		const c = text[i];
		if (c === '\n') return -1;
		if (c === '\\') {
			i += 2;
			continue;
		}
		if (c === '[') inClass = true;
		else if (c === ']') inClass = false;
		else if (c === '/' && !inClass) {
			i++;
			while (i < text.length && /[a-z]/.test(text[i])) i++;
			return i;
		}
		i++;
	}
	return -1;
}

// Whether a `/` after the token `prev` (its text, or '' at the start) begins a regular expression.
function regexAllowed(prev) {
	if (prev === '') return true;
	if (WORD.test(prev[prev.length - 1])) return REGEX_AFTER_WORD.has(prev);
	const last = prev[prev.length - 1];
	if (last === '"' || last === "'" || last === '`') return false;
	if (prev.length > 1 && prev[0] === '/') return false; // a regular expression before
	return last !== ')' && last !== ']';
}

// The tokens of the text with what lay between them: { text, gap } where gap is '' (nothing), ' ' (white space) or '\n' (a line break).
function scan(text) {
	const tokens = [];
	let gap = '';
	let prev = '';
	let i = 0;
	const push = (end) => {
		const token = text.slice(i, end);
		tokens.push({ text: token, gap });
		gap = '';
		prev = token;
		i = end;
	};
	while (i < text.length) {
		const c = text[i];
		if (c === '\n' || c === '\r' || c === ' ' || c === ' ') {
			gap = '\n';
			i++;
		} else if (c === ' ' || c === '\t' || c === '\f' || c === '\v' || c === ' ' || c === '﻿') {
			if (gap === '') gap = ' ';
			i++;
		} else if (c === '/' && text[i + 1] === '/') {
			while (i < text.length && text[i] !== '\n' && text[i] !== '\r') i++;
			if (gap === '') gap = ' ';
		} else if (c === '/' && text[i + 1] === '*') {
			const close = text.indexOf('*/', i + 2);
			const end = close < 0 ? text.length : close + 2;
			if (text.slice(i, end).includes('\n')) gap = '\n';
			else if (gap === '') gap = ' ';
			i = end;
		} else if (c === '"' || c === "'") {
			push(skipString(text, i));
		} else if (c === '`') {
			push(skipTemplate(text, i));
		} else if (c === '/' && regexAllowed(prev)) {
			const end = skipRegex(text, i);
			if (end < 0) push(i + 1);
			else push(end);
		} else if (WORD.test(c)) {
			let end = i + 1;
			while (end < text.length && WORD.test(text[end])) end++;
			push(end);
		} else {
			push(i + 1);
		}
	}
	return tokens;
}

// Whether the two tokens must not touch: they would read as one token or as the start of a comment.
function mustSeparate(a, b) {
	const x = a[a.length - 1];
	const y = b[0];
	if (WORD.test(x) && WORD.test(y)) return true;
	if ((x === '+' || x === '-') && x === y) return true;
	if (x === '/' && (y === '/' || y === '*')) return true;
	if (x === '<' && y === '!') return true;
	if (x === '-' && y === '>') return true;
	if (y === '.' && /[0-9]/.test(x)) return true;
	if (x === '.' && /[0-9]/.test(y) && !WORD.test(a[a.length - 2] || ' ')) return true;
	return false;
}

export function minifyScript(text) {
	const tokens = scan(text);
	let out = '';
	for (let i = 0; i < tokens.length; i++) {
		const token = tokens[i];
		if (i > 0) {
			if (token.gap === '\n') out += '\n';
			// tokens that touched in the text still do: only what was apart needs to stay apart
			else if (token.gap === ' ' && mustSeparate(tokens[i - 1].text, token.text)) out += ' ';
		}
		out += token.text;
	}
	return out;
}

// The tokens of a script without the white space between them: two texts that do the same have the same list (tests).
export function scriptTokens(text) {
	return scan(text).map((token) => token.text);
}
