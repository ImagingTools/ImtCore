// The markup of Text (RichText, StyledText, AutoText): what the parser makes of it, how the lines are laid out across the runs of
// different fonts, what the pointer does with a link (the hand, hoveredLink, linkHovered, linkActivated) and what it leaves to the items
// below. The sizes, margins and breaks are compared with desktop Qt in test/desktop/RichText_*; `node test/rich-text.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 400
	height: 300
	property string activated: ""
	property string hovered: ""
	property int clicks: 0
	property alias label: label
	property alias plain: plain
	property alias below: below

	MouseArea {
		id: below
		width: 400
		height: 40
		onClicked: root.clicks++
	}
	Text {
		id: label
		font.pixelSize: 20
		textFormat: Text.RichText
		text: "abc <a href=\\"one\\">link</a> <b>bold</b> abc"
		onLinkActivated: function(link) { root.activated = link }
		onLinkHovered: function(link) { root.hovered = link }
	}
	Text {
		id: plain
		y: 100
		font.pixelSize: 20
		text: "<b>x</b>"
	}
}
`,
};

class MemoryFS {
	async readText(path) {
		return path in FILES ? FILES[path] : null;
	}

	async readBytes() {
		return null;
	}
}

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 300, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame((t += 0.016));
	}
};
frames(4);
const root = viewer.content.$children[0];
const label = root.label;

// the text of the plain format is shown as it is
check('PlainText shows the markup', [root.plain.contentWidth, root.plain.$kind()], [160, 'plain']);

// the box font: 20 px per character, the line is 23 px high in a rich text
check('RichText is laid out in lines of runs', [label.$kind(), label.contentHeight, label.implicitWidth], ['rich', 23, 20 * 'abc link bold abc'.length]);
const layout = label.$richLayout(0);
const words = layout.lines[0].items.filter((item) => !item.space).map((item) => [item.text, item.style.bold, item.style.href, item.style.underline]);
check('the runs keep their style', words, [['abc', false, null, false], ['link', false, 'one', true], ['bold', true, null, false], ['abc', false, null, false]]);
check('the position of each run', layout.lines[0].items.filter((item) => !item.space).map((item) => item.x), [0, 80, 180, 280]);

// links
check('linkAt on a link, off it, below the line', [label.linkAt(100, 5), label.linkAt(10, 5), label.linkAt(100, 40), label.linkAt(70, 5)], ['one', '', '', '']);
check('the link has the color of links', label.linkColor, '#FF0000FF');
label.linkColor = '#ff0000';
check('linkColor can be set', label.linkColor, '#FFFF0000');

// a wrapped text: the runs break at the spaces
label.width = 140;
label.wrapMode = 1; // Text.WordWrap
frames(2);
const wrapped = label.$richLayout(140).lines.map((line) => line.items.filter((item) => !item.space).map((item) => item.text).join(' '));
check('the lines of a wrapped text', wrapped, ['abc', 'link', 'bold', 'abc']);
check('and their number does not matter for lineCount (as in Qt, a rich text has one)', label.lineCount, 1);
label.width = 0;
label.wrapMode = 0; // Text.NoWrap
frames(2);

// the pointer (the content of the viewer lies below its own header: the positions are the scene's)
label.y = 100;
frames(2);
const at = (x, y) => [root.$sx + x, root.$sy + y];
const inLink = at(100, 110);
const away = at(10, 110);
scene.pointerMove(...inLink);
check('over a link: hoveredLink, the hand, linkHovered', [label.hoveredLink, label.$cursorShape, root.hovered], ['one', 13, 'one']);
scene.pointerMove(...away);
check('off the link', [label.hoveredLink, label.$cursorShape, root.hovered], ['', 0, '']);
scene.pointerDown(...inLink);
scene.pointerUp(...inLink);
check('a click on the link activates it', root.activated, 'one');
root.activated = '';
scene.pointerDown(...away);
scene.pointerUp(...away);
check('a click beside the link does not', root.activated, '');

// the MouseArea below the text still gets the clicks that are not on a link
label.y = 0;
frames(2);
root.clicks = 0;
scene.pointerDown(...at(10, 10));
scene.pointerUp(...at(10, 10));
check('the item below a text gets the clicks that are not on a link', root.clicks, 1);
root.clicks = 0;
root.activated = '';
scene.pointerDown(...at(100, 10));
scene.pointerUp(...at(100, 10));
check('and not the ones on the link', [root.clicks, root.activated], [0, 'one']);
label.y = 100;
frames(2);

// the parser
const parse = (text, format = 'RichText') => {
	label.textFormat = format;
	label.text = text;
	return label.$richLayout(0);
};
const texts = (layout) => layout.lines.map((line) => line.items.filter((item) => !item.space).map((item) => item.text).join(' '));
check('entities', texts(parse('&lt;a&gt; &amp; &#65;&#x42; &nbsp;x')), ['<a> & AB \u00a0x']);
check('white space folds, a break stays', texts(parse('a   b\n c<br>d')), ['a b c', 'd']);
check('a tag that is not known is dropped, its text stays', texts(parse('<foo>abc</foo>')), ['abc']);
check('the unclosed tag ends with the text', parse('<b>abc').lines[0].items[0].style.bold, true);
check('a comment', texts(parse('a<!-- x -->b')), ['ab']);
check('font color, size and a span style', (() => {
	const items = parse('<font color="red" size="5">a</font><span style="color:#00ff00;font-size:10px;font-weight:bold">b</span>').lines[0].items;
	return [items[0].style.color, items[0].style.level, items[1].style.color, items[1].style.sizePx, items[1].style.bold];
})(), ['#ff0000', 5, '#00ff00', 10, true]);
check('a list item has its marker and the indent', (() => {
	const blocks = parse('<ul><li>a</li></ul><ol start="3"><li>b</li></ol>').blocks;
	return [blocks[0].marker.ordered, blocks[0].left, blocks[1].marker.ordered, blocks[1].marker.number];
})(), [false, 40, true, 3]);

// StyledText and AutoText
label.textFormat = 'StyledText';
label.text = '<b>a</b><br>b';
check('StyledText is lines of the plain height', [label.contentHeight, label.lineCount], [40, 2]);
label.textFormat = 'AutoText';
label.text = '<b>a</b>';
check('AutoText finds the markup', label.$kind(), 'styled');
label.text = 'a < b';
check('and a text that has none', label.$kind(), 'plain');

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
