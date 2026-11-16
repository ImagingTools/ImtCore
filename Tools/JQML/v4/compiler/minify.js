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

const PLAIN_IDENTIFIER = /^[A-Za-z_$][\w$]*$/;
const BAILING_WORD = new Set(['function', 'class', 'yield', 'await', 'with']);

// An expression position opens an object literal; a statement position (after `;`, after a block's own `}`, or at the very
// start, since a `body` fragment has no braces of its own) opens a block instead. Good enough for the shapes this project's
// QML scripts are written in (AGENTS.md keeps them to simple assignments and ternaries); mangleLocals treats a literal it
// does not recognize (a computed or shorthand key, a method) as a reason to leave the whole fragment alone, never to guess.
const EXPRESSION_BEFORE = new Set(['=', '(', ',', '[', '{', ':', '?', '&&', '||', '??', '!', '+', '-', '*', '/', '%', '<', '>', '==', '===', '!=', '!==']);

function bracesOpenLiteral(prevText) {
	if (prevText === undefined) return false; // the very start of a `body` fragment: always a statement
	return EXPRESSION_BEFORE.has(prevText) || prevText === 'return';
}

// The names mangleLocals may rename: the plain parameters of the fragment's own function (no destructuring, no default) and
// its top-level `var` declarations (var is hoisted to the whole fragment, so a nested block does not change its scope), plus a
// top-level `let`/`const` (one declared inside a nested block could be shadowed by a sibling block the same way, which this
// does not try to tell apart, so only the top level counts). Returns null - mangle nothing - the moment anything here is not
// as simple as that: a nested function or arrow (its own scope, closures, possible shadowing), a destructured declaration or
// parameter, or a name declared twice (shadowing a parameter counts as that too).
function localNames(tokens, paramNames) {
	const names = new Set(paramNames);
	let depth = 0;
	for (let i = 0; i < tokens.length; i++) {
		const t = tokens[i].text;
		if (t === '{') depth++;
		else if (t === '}') depth--;
		else if (BAILING_WORD.has(t) && (i === 0 || tokens[i - 1].text !== '.')) return null;
		else if (t === '=' && tokens[i + 1] && tokens[i + 1].text === '>' && tokens[i + 1].gap === '') return null; // an arrow function
		else if ((t === 'var' || t === 'let' || t === 'const') && (i === 0 || tokens[i - 1].text !== '.')) {
			// `for (let x ...)`/`for (const x ...)` scope x to the whole for statement, not to the block depth it is read at
			// here (there may be none yet): the '(' right before the keyword is the tell, since a bare statement never has one.
			if (t !== 'var' && (depth !== 0 || (i > 0 && tokens[i - 1].text === '('))) return null;
			let j = i + 1;
			for (;;) {
				const name = tokens[j];
				if (name === undefined || !PLAIN_IDENTIFIER.test(name.text)) return null; // destructuring, or something unexpected
				if (names.has(name.text)) return null; // declared twice, or shadows a parameter: stay out of it
				names.add(name.text);
				j++;
				let nested = 0; // skips the initializer, '(' / '[' / '{' of its own so a ',' or ';' inside it is not mistaken for the list's
				while (tokens[j] && !(nested === 0 && (tokens[j].text === ',' || tokens[j].text === ';'))) {
					const u = tokens[j].text;
					if (u === '(' || u === '[' || u === '{') nested++;
					else if (u === ')' || u === ']' || u === '}') nested--;
					j++;
				}
				if (tokens[j] === undefined || tokens[j].text === ';') break;
				j++; // the ',' of another name in the same declaration
			}
		}
	}
	return names;
}

// Whether tokens[at] is the key of an object literal's own property (`{ name: ... }`, `{ ..., name: ... }`), not a value:
// mangleLocals never renames one, even when its text happens to match a name it would otherwise rename everywhere else.
// `frames` is the brace/paren/bracket stack built alongside the same walk that looks for an occurrence to rename: a nested
// '(' / '[' / '{' pushes its own frame, so the top of the stack is this literal's own level only until one of those opens.
function isLiteralKey(tokens, at, frames) {
	if (frames.length === 0 || frames[frames.length - 1] !== 'literal') return false;
	const prev = at > 0 ? tokens[at - 1].text : undefined;
	const next = tokens[at + 1] ? tokens[at + 1].text : undefined;
	return (prev === '{' || prev === ',') && next === ':';
}

// Renames the plain parameters and the top-level var/let/const declarations of one script fragment (a property binding, a
// signal handler or a function body) that stay inside it: never an id, a property, a signal or anything else a binding
// outside this one fragment could still be reading or writing by that name. `params` is the function's own parameter list as
// written (its text, not run through this); `body` is the statements (property bindings, see src/parser.js). Returns
// { body, params }, unchanged when nothing here was safe to rename (see localNames).
export function mangleLocals(body, params) {
	const trimmedParams = params.trim();
	// a default value or a destructuring pattern in the parameter list: only a plain, comma-separated list of names is renamed
	if (trimmedParams !== '' && !/^[A-Za-z_$][\w$]*(\s*,\s*[A-Za-z_$][\w$]*)*$/.test(trimmedParams)) return { body, params };
	const paramNames = trimmedParams === '' ? [] : trimmedParams.split(',').map((p) => p.trim());
	const tokens = scan(body);
	const names = localNames(tokens, paramNames);
	if (names === null || names.size === 0) return { body, params };
	const used = new Set(tokens.map((token) => token.text).join(' ').split(/[^\w$]+/));
	const rename = new Map();
	let next = 0;
	for (const name of names) {
		let candidate;
		do {
			candidate = `$a${next++}`;
		} while (used.has(candidate));
		rename.set(name, candidate);
		used.add(candidate);
	}
	// A stack of the open '(' / '[' / '{' this token sits inside of, nearest first; a '{' is pushed as 'literal' or 'block'
	// (bracesOpenLiteral), the others as 'other'. Its top says what isLiteralKey needs: whether this token is directly inside
	// an object literal, not inside some '(' / '[' / '{' that opened after it (which pushed its own frame on top).
	const frames = [];
	let out = '';
	for (let i = 0; i < tokens.length; i++) {
		const token = tokens[i];
		if (i > 0) out += token.gap;
		const t = token.text;
		const replacement = rename.get(t);
		const isMember = i > 0 && tokens[i - 1].text === '.';
		out += replacement !== undefined && !isMember && !isLiteralKey(tokens, i, frames) ? replacement : t;
		if (t === '(' || t === '[') frames.push('other');
		else if (t === '{') frames.push(bracesOpenLiteral(i > 0 ? tokens[i - 1].text : undefined) ? 'literal' : 'block');
		else if (t === ')' || t === ']' || t === '}') frames.pop();
	}
	// paramNames are exactly the comma-separated words of `params`: renaming each occurrence is exact and needs no tokenizer.
	const newParams = paramNames.map((p) => rename.get(p) || p).join(', ');
	return { body: out, params: newParams };
}
