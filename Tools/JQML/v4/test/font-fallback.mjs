// A font family that is not installed (Segoe UI on macOS) is drawn with the sans-serif font of the browser, as Qt takes a font of
// the system for it; the built-in square look stays for text without a family. The canvas is a stand-in that knows a few families.
// `node test/font-fallback.mjs`
import { FontManager } from '../src/fonts.js';

// The width of a text in a CSS font: an installed family has its own width, a missing one falls back to the next of the list.
const INSTALLED = { monospace: 10, serif: 8, 'sans-serif': 7, Arial: 6 };

function fakeContext() {
	return {
		font: '',
		measureText(text) {
			const families = this.font.replace(/^\s*(?:bold\s+|italic\s+)*[\d.]+px\s+/, '').split(',');
			let width = INSTALLED.serif;
			for (const family of families) {
				const name = family.trim().replace(/^"|"$/g, '');
				if (name in INSTALLED) {
					width = INSTALLED[name];
					break;
				}
			}
			return { width: width * text.length, actualBoundingBoxAscent: 9, actualBoundingBoxDescent: 3 };
		},
	};
}

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

const fonts = new FontManager();
fonts.measure = fakeContext();
const warnings = [];
fonts.onWarning = (message) => warnings.push(message);

check('an installed family is taken', fonts.resolveFamily('Arial'), 'Arial');
check('the first installed family of a list is taken', fonts.resolveFamily('Segoe UI, Arial'), 'Arial');
check('a family that is not installed is drawn with sans-serif', fonts.resolveFamily('Segoe UI'), 'sans-serif');
check('a missing family is told once', warnings, ["Font family 'Segoe UI' not found, using sans-serif"]);
fonts.resolveFamily('Segoe UI');
check('the warning is not repeated', warnings.length, 1);

const missing = fonts.get('Segoe UI', 12, false, false);
check('text of a missing family has the metrics of a font, not the square cells', missing.style !== null, true);
check('its characters are as wide as in sans-serif', missing.advance('i'), (INSTALLED['sans-serif'] * 12) / 100);
check('text without a family keeps the built-in look', fonts.get('', 12, false, false).style, null);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
