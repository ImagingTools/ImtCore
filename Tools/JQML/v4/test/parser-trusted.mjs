// A compiled application does not check the scripts of its documents again: the compiler parsed every one of them with the checks on.
// `node test/parser-trusted.mjs`
import { parseQmlDocument } from '../src/parser.js';

let failures = 0;
function check(name, ok) {
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}`);
}

const broken = 'import QtQuick\nItem {\n\twidth: 1 +\n}\n';
const fine = 'import QtQuick\nItem {\n\twidth: 1 + 2\n}\n';

let thrown = null;
try {
	parseQmlDocument(broken);
} catch (error) {
	thrown = error;
}
check('a script with a syntax error is reported by default', thrown !== null && thrown.line === 3);
check('it is accepted when the checks are off', parseQmlDocument(broken, false).type === 'Item');
check('a document that is fine parses the same either way', JSON.stringify(parseQmlDocument(fine, false)) === JSON.stringify(parseQmlDocument(fine)));
let again = null;
try {
	parseQmlDocument(broken);
} catch (error) {
	again = error;
}
check('the checks are on again after a parse without them', again !== null);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
