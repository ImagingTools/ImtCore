// `ctx.createImageData()` and `ctx.putImageData()` against what desktop Qt gives: the size (rounded), what is refused, and what is drawn:
// with the transform and globalAlpha of the context, over what is there, clipped at the canvas; nothing at all when the rectangle of
// the pixels to take is not given, and what Qt makes of a rectangle that does not start at the corner.
// `node test/canvas-image-data.mjs`
import { CanvasContext, CanvasImageData } from '../src/canvas.js';
import { rasterize } from './canvas-raster.mjs';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `\n      got      ${JSON.stringify(actual)}\n      expected ${JSON.stringify(expected)}`}`);
}
const refused = (fn) => {
	try {
		fn();
		return 'no error';
	} catch (error) {
		return error.message;
	}
};
const grey = [128, 128, 128, 255];

{
	const ctx = new CanvasContext(100, 100, []);
	const a = ctx.createImageData(3, 2);
	check('createImageData(3, 2)', [a instanceof CanvasImageData, a.width, a.height, a.data.length, a.data[0], a.data[23], a.data[24]], [true, 3, 2, 24, 0, 0, undefined]);
	const b = ctx.createImageData(a);
	check('createImageData(imageData) has the size of it', [b.width, b.height, b.data.length, b !== a, b.data !== a.data], [3, 2, 24, true, true]);
	for (const args of [[0, 0], [0, 3], [-2, 3], [3, -2], [undefined, 2], ['x', 2]]) {
		check(`createImageData(${args.map(String).join(', ')}) is refused`, refused(() => ctx.createImageData(...args)), 'createImageData(): invalid arguments');
	}
	const rounded = ctx.createImageData(2.7, 1.2);
	check('the size is rounded', [rounded.width, rounded.height, rounded.data.length], [3, 1, 12]);
	const text = ctx.createImageData('3', '2');
	check('a size can be text', [text.width, text.height], [3, 2]);
	check('no size gives undefined', [ctx.createImageData(), ctx.createImageData({ width: 2, height: 2 })], [undefined, undefined]);
	a.data[2] = 12;
	a.data[3] = 255;
	check('pixels are written and read', [a.data[2], a.data[3]], [12, 255]);
}

// An image of 4 x 2: the red of pixel i is 30 * i, green 100, blue 200.
function gradientImage(ctx) {
	const image = ctx.createImageData(4, 2);
	for (let i = 0; i < 8; i++) image.data.set([30 * i, 100, 200, 255], 4 * i);
	return image;
}
function canvasWithBackground() {
	const commands = [];
	const ctx = new CanvasContext(100, 100, commands);
	ctx.fillStyle = '#808080';
	ctx.fillRect(0, 0, 100, 100);
	return { ctx, commands };
}
const pixels = (commands, points) => {
	const image = rasterize(commands, 100, 100);
	return points.map(([x, y]) => image.at(x, y));
};

{
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(gradientImage(ctx), 10, 20, 0, 0, 4, 2);
	check('putImageData(img, 10, 20, 0, 0, 4, 2)', pixels(commands, [[9, 20], [10, 20], [11, 20], [13, 20], [10, 21], [14, 20], [10, 22]]), [grey, [0, 100, 200, 255], [30, 100, 200, 255], [90, 100, 200, 255], [120, 100, 200, 255], grey, grey]);
}
{
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(gradientImage(ctx), 10, 20);
	check('without the rectangle of the pixels nothing is drawn (Qt)', commands.length, 1);
	check('putImageData gives the context', ctx.putImageData(gradientImage(ctx), 10, 20, 0, 0, 4, 2) === ctx, true);
	check('and with the rectangle it draws', commands.length, 2);
}
{
	const { ctx, commands } = canvasWithBackground();
	ctx.save();
	ctx.translate(30, 30);
	ctx.globalAlpha = 0.5;
	ctx.putImageData(gradientImage(ctx), 50, 50, 0, 0, 4, 2);
	ctx.restore();
	check('the transform and globalAlpha are used', pixels(commands, [[50, 50], [80, 80], [81, 80]]), [grey, [64, 114, 164, 255], [79, 114, 164, 255]]);
}
{
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(ctx.createImageData(2, 1), 5, 5, 0, 0, 2, 1);
	check('transparent pixels leave what is there', pixels(commands, [[5, 5]]), [grey]);
}
{
	const { ctx, commands } = canvasWithBackground();
	const image = ctx.createImageData(2, 1);
	image.data.set([255, 0, 0, 128, 0, 255, 0, 255]);
	ctx.putImageData(image, 5, 5, 0, 0, 2, 1);
	check('half transparent pixels are drawn over it', pixels(commands, [[5, 5], [6, 5]]), [[192, 64, 64, 255], [0, 255, 0, 255]]);
}
{
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(gradientImage(ctx), -2, 5, 0, 0, 4, 2);
	check('what is left of the canvas is cut off', pixels(commands, [[0, 5], [1, 5], [2, 5]]), [[60, 100, 200, 255], [90, 100, 200, 255], grey]);
}
{
	// Qt draws the rectangle of the pixels out of a copy of it, so the offset counts twice (read from Qt: pixel 2 of the first row at dx, dy).
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(gradientImage(ctx), 10, 20, 1, 0, 2, 1);
	check('putImageData(img, 10, 20, 1, 0, 2, 1): one pixel, the third of the row', pixels(commands, [[10, 20], [11, 20], [12, 20], [10, 21]]), [[60, 100, 200, 255], grey, grey, grey]);
}
{
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(gradientImage(ctx), 10, 20, 0, 1, 4, 1);
	check('putImageData(img, 10, 20, 0, 1, 4, 1): nothing', pixels(commands, [[10, 20], [10, 21], [11, 21], [13, 21]]), [grey, grey, grey, grey]);
}
{
	const { ctx, commands } = canvasWithBackground();
	ctx.putImageData(gradientImage(ctx), 10, 20, 0, 0, 2, 1);
	check('a rectangle at the corner is a crop, drawn at dx, dy', pixels(commands, [[10, 20], [11, 20], [12, 20], [10, 21]]), [[0, 100, 200, 255], [30, 100, 200, 255], grey, grey]);
}
{
	const { ctx, commands } = canvasWithBackground();
	const big = ctx.createImageData(160, 160);
	big.data.fill(255);
	ctx.putImageData(big, 50, 5, 80, 80, 80, 80);
	check('(80, 80, 80, 80) of an image of 160 x 160 draws nothing, as in Qt', commands.length, 1);
}
{
	// What was written after the call does not change what is drawn.
	const { ctx, commands } = canvasWithBackground();
	const image = ctx.createImageData(1, 1);
	image.data.set([200, 0, 0, 255]);
	ctx.putImageData(image, 1, 1, 0, 0, 1, 1);
	image.data.set([0, 0, 200, 255]);
	check('the pixels are taken when putImageData is called', pixels(commands, [[1, 1]]), [[200, 0, 0, 255]]);
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
