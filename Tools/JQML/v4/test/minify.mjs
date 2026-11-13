// The smaller scripts of the compiler (compiler/minify.js) do the same as the scripts they come from: strings, templates and regular
// expressions are not touched, a line break stays (automatic semicolons), tokens that would join stay apart. Every case is run both ways.
// `node test/minify.mjs`
import { minifyScript } from '../compiler/minify.js';

let failures = 0;
function check(name, ok, detail = '') {
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}${ok ? '' : `: ${detail}`}`);
}

const cases = {
	'comments go': ['// line\nreturn 1 /* block */ + 2 // end', 3],
	'a line break stays': ['var a = 1\nvar b = 2\nreturn a + b', 3],
	'return then a line break': ['return\n42', undefined],
	'a string with //': ['return "http://x.y/z" + \'/* not a comment */\'', 'http://x.y/z/* not a comment */'],
	'a string with spaces and escapes': ["return \"a  b\\\" c\" + 'd  e'", 'a  b" cd  e'],
	'a template': ['var n = 3\nreturn `a ${ n + 1 }  b ${ `x ${ n }` } //`', 'a 4  b x 3 //'],
	'a regular expression with spaces': ['return /a  +b/.test("a   b") && /[/ ]+/.test("/ /")', true],
	'a regular expression with flags and a division': ['var a = 8, b = 2\nreturn a / b / 2 + "x y".replace(/ /g, "_").length', 5],
	'division after a bracket': ['var a = [8]\nreturn (a[0] + 0) / 2 / (1)', 4],
	'minus minus': ['var a = 5, b = 2\nreturn a - -b + a - - b', 14],
	'plus plus': ['var a = 5, b = 2\nreturn a + +b + a + + b', 14],
	'increment next to a sign': ['var a = 5, b = 1\nvar c = a++ + b\nreturn c * 10 + a', 66],
	'words stay apart': ['var a = 1\nreturn typeof a + (a in {1: 1}) + (a instanceof Object)', 'numbertruefalse'],
	'a number and a dot': ['return 1 .toFixed(1) + 2..toString()', '1.02'],
	'a ternary': ['var a = 1\nreturn a ? "yes" : "no"', 'yes'],
	'an arrow function': ['var f = (x) => x * 2\nreturn [1, 2].map(f).join(" ")', '2 4'],
	'an object literal': ['var o = { a : 1, "b c" : 2 }\nreturn o.a + o["b c"]', 3],
	'a regular expression after return': ['return /x y/.source', 'x y'],
	'a comment after a division': ['var a = 6\nreturn a / 2 // three', 3],
	'html comment markers': ['var a = 3, b = 1\nreturn a < !b || a-- > b', true],
};

for (const [name, [code, expected]] of Object.entries(cases)) {
	const small = minifyScript(code);
	let before;
	let after;
	try {
		before = new Function(code)();
		after = new Function(small)();
	} catch (error) {
		check(name, false, `${error.message}: ${JSON.stringify(small)}`);
		continue;
	}
	check(name, Object.is(before, after) && (expected === undefined || Object.is(after, expected)), `${JSON.stringify(before)} -> ${JSON.stringify(after)} for ${JSON.stringify(small)}`);
}

check('it is smaller', minifyScript('  var a   =  1 ;\n\n\n   // c\n   return a  ') === 'var a=1;\nreturn a', JSON.stringify(minifyScript('  var a   =  1 ;\n\n\n   // c\n   return a  ')));
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
