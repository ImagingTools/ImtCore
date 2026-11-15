// `ctx.createPattern()` against what desktop Qt gives (read with getImageData): the 8 x 8 bitmaps of the brush patterns, a picture
// repeated from the origin of the coordinates of the context, in both directions whatever the repetition says, with globalAlpha, a
// stroke, and what is drawn for a picture that is not loaded.
// `node test/canvas-pattern.mjs`
import { CanvasContext, CanvasPattern } from '../src/canvas.js';
import { rasterize } from './canvas-raster.mjs';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `\n      got      ${JSON.stringify(actual)}\n      expected ${JSON.stringify(expected)}`}`);
}

// The letters a pixel of the picture of the tests has: red, green, blue, yellow, or _ for nothing.
function letter(p) {
	if (p[3] === 0) return '_';
	if (p[0] > 200 && p[1] < 50 && p[2] < 50) return 'R';
	if (p[1] > 200 && p[0] < 50 && p[2] < 50) return 'G';
	if (p[2] > 200 && p[0] < 50 && p[1] < 50) return 'B';
	if (p[0] > 200 && p[1] > 200 && p[2] < 50) return 'Y';
	return '?';
}
const letters = (image, x0, y0, w, h) => {
	const rows = [];
	for (let y = y0; y < y0 + h; y++) {
		let row = '';
		for (let x = x0; x < x0 + w; x++) row += letter(image.at(x, y));
		rows.push(row);
	}
	return rows.join('|');
};
const bits = (image, w, h) => {
	const rows = [];
	for (let y = 0; y < h; y++) {
		let row = '';
		for (let x = 0; x < w; x++) row += image.at(x, y)[3] > 128 ? '#' : '.';
		rows.push(row);
	}
	return rows.join('|');
};

// The brush patterns, as Qt draws them in black (Qt.XPattern is the number).
const BRUSHES = {
	1: '########|########|########|########|########|########|########|########',
	2: '########|##.###.#|########|########|########|##.###.#|########|########',
	3: '###.###.|########|#.###.##|########|###.###.|########|#.###.##|########',
	4: '#.#.#.#.|##.###.#|#.#.#.#.|.###.###|#.#.#.#.|##.###.#|#.#.#.#.|.###.###',
	5: '.#.#.#.#|#.#.#.#.|.#.#.#.#|#.#.#.#.|.#.#.#.#|#.#.#.#.|.#.#.#.#|#.#.#.#.',
	6: '.#.#.#.#|..#...#.|.#.#.#.#|#...#...|.#.#.#.#|..#...#.|.#.#.#.#|#...#...',
	7: '...#...#|........|.#...#..|........|...#...#|........|.#...#..|........',
	8: '........|..#...#.|........|........|........|..#...#.|........|........',
	0: '........|........|........|........|........|........|........|........',
	9: '........|........|........|########|........|........|........|........',
	10: '....#...|....#...|....#...|....#...|....#...|....#...|....#...|....#...',
	11: '....#...|....#...|....#...|########|....#...|....#...|....#...|....#...',
	12: '.......#|......#.|.....#..|....#...|...#....|..#.....|.#......|#.......',
	13: '#.......|.#......|..#.....|...#....|....#...|.....#..|......#.|.......#',
	14: '#......#|.#....#.|..#..#..|...##...|...##...|..#..#..|.#....#.|#......#',
};
for (const [mode, expected] of Object.entries(BRUSHES)) {
	const commands = [];
	const ctx = new CanvasContext(100, 100, commands);
	ctx.fillStyle = ctx.createPattern('#000000', Number(mode));
	ctx.fillRect(0, 0, 16, 16);
	check(`brush ${mode}`, bits(rasterize(commands, 16, 16), 8, 8), expected);
	// a tile is the same everywhere: the bitmap repeats every 8 pixels
	const image = rasterize(commands, 16, 16);
	const shifted = [];
	for (let y = 8; y < 16; y++) {
		let row = '';
		for (let x = 8; x < 16; x++) row += image.at(x, y)[3] > 128 ? '#' : '.';
		shifted.push(row);
	}
	check(`brush ${mode} repeats`, shifted.join('|'), expected);
}
{
	const commands = [];
	const ctx = new CanvasContext(100, 100, commands);
	ctx.fillStyle = ctx.createPattern('#ff0000', 9);
	ctx.save();
	ctx.translate(0, 2);
	ctx.fillRect(0, 0, 8, 8);
	ctx.restore();
	const image = rasterize(commands, 8, 10);
	const rows = [];
	for (let y = 0; y < 10; y++) rows.push(letter(image.at(0, y)) + letter(image.at(1, y)) + letter(image.at(2, y)));
	check('a brush follows translate() and has the colour', rows.join('|'), '___|___|___|___|___|RRR|___|___|___|___');
}
{
	const commands = [];
	const ctx = new CanvasContext(100, 100, commands);
	const p = ctx.createPattern('#ff0000', 1);
	ctx.fillStyle = p;
	ctx.fillRect(0, 0, 4, 4);
	check('the solid brush is a plain fill', [commands.length, commands[0].k, commands[0].color.R], [1, 'rect', 255]);
	check('and fillStyle gives the colour of the brush, not the pattern', [ctx.fillStyle, p instanceof CanvasPattern], ['#ff0000', true]);
}

// The picture of the tests: 4 x 4 pixels, red / green on top, blue / yellow below, in blocks of 2 x 2.
const colours = [[255, 0, 0, 255], [0, 255, 0, 255], [0, 0, 255, 255], [255, 255, 0, 255]];
const data = new Uint8ClampedArray(4 * 4 * 4);
for (let y = 0; y < 4; y++) for (let x = 0; x < 4; x++) data.set(colours[(x >> 1) + 2 * (y >> 1)], (y * 4 + x) * 4);
const pictures = new Map([['tile.png', { width: 4, height: 4, data }]]);
const context = (commands) => new CanvasContext(100, 100, commands, pictures);

for (const repetition of ['repeat', 'repeat-x', 'repeat-y', 'no-repeat', 'bogus']) {
	const commands = [];
	const ctx = context(commands);
	ctx.fillStyle = ctx.createPattern('tile.png', repetition);
	ctx.fillRect(2, 1, 10, 9);
	check(`a picture, ${repetition}: repeats in both directions from the origin`, letters(rasterize(commands, 14, 11), 0, 0, 14, 11), '______________|__GGRRGGRRGG__|__YYBBYYBBYY__|__YYBBYYBBYY__|__GGRRGGRRGG__|__GGRRGGRRGG__|__YYBBYYBBYY__|__YYBBYYBBYY__|__GGRRGGRRGG__|__GGRRGGRRGG__|______________');
}
{
	const commands = [];
	const ctx = context(commands);
	ctx.fillStyle = ctx.createPattern('tile.png', 'repeat');
	ctx.save();
	ctx.translate(3, 2);
	ctx.fillRect(0, 0, 10, 8);
	ctx.restore();
	check('a picture starts where the context starts', letters(rasterize(commands, 14, 11), 0, 0, 14, 11), '______________|______________|___RRGGRRGGRR_|___RRGGRRGGRR_|___BBYYBBYYBB_|___BBYYBBYYBB_|___RRGGRRGGRR_|___RRGGRRGGRR_|___BBYYBBYYBB_|___BBYYBBYYBB_|______________');
}
{
	const commands = [];
	const ctx = context(commands);
	ctx.fillStyle = ctx.createPattern('tile.png', 'repeat');
	ctx.globalAlpha = 0.5;
	ctx.fillRect(0, 0, 4, 2);
	const image = rasterize(commands, 4, 2);
	check('globalAlpha', [image.at(0, 0), image.at(3, 0)], [[255, 0, 0, 128], [0, 255, 0, 128]]);
}
{
	const commands = [];
	const ctx = context(commands);
	const pattern = ctx.createPattern('tile.png', 'repeat');
	ctx.strokeStyle = pattern;
	ctx.lineWidth = 4;
	ctx.beginPath();
	ctx.moveTo(1, 5);
	ctx.lineTo(13, 5);
	ctx.stroke();
	const image = rasterize(commands, 14, 9);
	check('a stroke takes its colours from the pattern', [letter(image.at(2, 4)), letter(image.at(4, 4)), letter(image.at(2, 6)), letter(image.at(2, 2)), ctx.strokeStyle], ['G', 'R', 'Y', '_', '#000000']);
}
{
	const commands = [];
	const ctx = context(commands);
	ctx.fillStyle = ctx.createPattern('missing.png', 'repeat');
	ctx.fillRect(0, 0, 10, 10);
	check('a picture that is not loaded draws nothing', rasterize(commands, 10, 10).at(3, 3), [0, 0, 0, 0]);
	check('and fillStyle is black', ctx.fillStyle, '#000000');
}
{
	const commands = [];
	const ctx = context(commands);
	ctx.fillStyle = '#123456';
	ctx.save();
	ctx.fillStyle = ctx.createPattern('tile.png', 'repeat');
	ctx.restore();
	ctx.fillRect(0, 0, 2, 2);
	check('restore() brings the colour back', [commands[0].k, commands[0].color.R, commands[0].color.G, commands[0].color.B], ['rect', 0x12, 0x34, 0x56]);
}
{
	// A large area and a tile of one pixel: the pieces are made of the repeated tile, not of one piece for every pixel.
	const commands = [];
	const ctx = new CanvasContext(1000, 1000, commands, new Map([['one.png', { width: 1, height: 1, data: Uint8ClampedArray.of(0, 255, 0, 255) }]]));
	ctx.fillStyle = ctx.createPattern('one.png', 'repeat');
	ctx.fillRect(0, 0, 1000, 1000);
	check('a tile of one pixel is repeated to a tile of 64', commands.length < 300, true);
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
