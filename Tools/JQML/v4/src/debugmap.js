// The tail of a script that the engine compiles from a QML file in a debug build: a name for the debugger and a source map that points
// its lines at the lines of the QML file, so a breakpoint set in the .qml file stops there (VS Code with the JavaScript debugger, the
// DevTools of the browser). The script is made by `new Function`: V8 puts two lines of its own in front of the body.

const BASE64 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

function vlq(value) {
	let rest = value < 0 ? (-value << 1) | 1 : value << 1;
	let out = '';
	do {
		let digit = rest & 31;
		rest >>>= 5;
		if (rest > 0) digit |= 32;
		out += BASE64[digit];
	} while (rest > 0);
	return out;
}

// The lines that `new Function(args, body)` adds before the body: `function anonymous(args` and `) {`.
const FUNCTION_HEADER_LINES = 2;

// `sourceLines`: the number of lines of the source that goes to `new Function` (its first line holds the `with` chain and the head of
// the function, the body of the QML script starts on it), `firstLine`: the line of the QML file (from 1) on which the text of the script
// starts. Returns the text to append to the source.
export function scriptTail(url, file, firstLine, sourceLines) {
	const mappings = [];
	for (let i = 0; i < FUNCTION_HEADER_LINES; i++) mappings.push('');
	let previousLine = 0;
	for (let i = 0; i < sourceLines; i++) {
		// The first line of the source holds the head of the function and the first line of the script: the line of the QML file
		// where the text starts. Every next line is one line below.
		const line = firstLine - 1 + Math.max(0, i - 1);
		mappings.push(vlq(0) + vlq(0) + vlq(line - previousLine) + vlq(0));
		previousLine = line;
	}
	const map = JSON.stringify({ version: 3, file: `${file}.js`, sources: [url], names: [], mappings: mappings.join(';') });
	return `\n//# sourceURL=jqml-script:///${file}#${firstLine}\n//# sourceMappingURL=data:application/json;charset=utf-8,${encodeURIComponent(map)}`;
}
