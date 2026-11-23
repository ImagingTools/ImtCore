// Run: node --test tests/unit
// Canvas and Context2D as qquickcanvasitem.cpp and qquickcontext2d.cpp (Qt 6.8), on a stand-in for the DOM canvas
// that behaves as the browser's: a new size resets the DOM state of its context.
// - available once in a window (queued); contextType makes the context; getContext("2d") sets it; another type warns;
// - canvasSize, tileSize and canvasWindow follow the item until set; requestPaint() marks the canvas window;
// - paint(region) at polish, painted() only when commands were drawn; requestAnimationFrame callbacks first;
// - the methods return the context; Qt colors in the styles and the gradient stops; the transform in item
//   coordinates over the device pixel ratio; fillRule in the state; shear, conical gradients, text(), reset();
// - the state survives a new canvas size; loadImage through a shared cache; toDataURL.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const tick = () => new Promise((resolve) => setTimeout(resolve, 0))

const STATE_DEFAULTS = { fillStyle: '#000000', strokeStyle: '#000000', globalAlpha: 1, globalCompositeOperation: 'source-over',
    lineWidth: 1, lineCap: 'butt', lineJoin: 'miter', miterLimit: 10, lineDashOffset: 0, shadowOffsetX: 0,
    shadowOffsetY: 0, shadowBlur: 0, shadowColor: 'rgba(0, 0, 0, 0)', font: '10px sans-serif', textAlign: 'start',
    textBaseline: 'alphabetic' }

function multiply(m, n){
    let [a, b, c, d, e, f] = m, [A, B, C, D, E, F] = n
    return [a * A + c * B, b * A + d * B, a * C + c * D, b * C + d * D, a * E + c * F + e, b * E + d * F + f]
}

class FakeGradient {
    constructor(){ this.stops = [] }
    addColorStop(offset, color){
        if(color === 'not a color') throw new Error('SyntaxError')
        this.stops.push([offset, color])
    }
}

// the DOM CanvasRenderingContext2D: state accessors on the prototype, methods recorded
class FakeContext {
    constructor(){
        this.calls = []
        this.$domReset()
    }
    $domReset(){
        this.s = Object.assign({}, STATE_DEFAULTS, { transform: [1, 0, 0, 1, 0, 0], dash: [] })
        this.domStack = []
    }
    save(){ this.domStack.push(JSON.parse(JSON.stringify(this.s))) }
    restore(){ if(this.domStack.length) this.s = this.domStack.pop() }
    setTransform(...args){
        let m = args.length === 1 ? [args[0].a, args[0].b, args[0].c, args[0].d, args[0].e, args[0].f] : args
        this.calls.push(['setTransform', ...m])
        this.s.transform = m.slice()
    }
    getTransform(){ let [a, b, c, d, e, f] = this.s.transform; return { a, b, c, d, e, f } }
    transform(...m){ this.calls.push(['transform', ...m]); this.s.transform = multiply(this.s.transform, m) }
    translate(x, y){ this.transform(1, 0, 0, 1, x, y) }
    scale(x, y){ this.transform(x, 0, 0, y, 0, 0) }
    setLineDash(d){ this.s.dash = d.slice() }
    getLineDash(){ return this.s.dash.slice() }
    createLinearGradient(...a){ this.calls.push(['createLinearGradient', ...a]); return new FakeGradient() }
    createRadialGradient(...a){ this.calls.push(['createRadialGradient', ...a]); return new FakeGradient() }
    createConicGradient(...a){ this.calls.push(['createConicGradient', ...a]); return new FakeGradient() }
    createPattern(...a){ this.calls.push(['createPattern', ...a]); return {} }
    createImageData(w, h){ return { width: w, height: h, data: [] } }
    isPointInPath(...a){ this.calls.push(['isPointInPath', ...a]); return false }
}
for(let name of ['beginPath', 'moveTo', 'lineTo', 'closePath', 'ellipse', 'roundRect', 'rect', 'arc', 'fill', 'stroke', 'clip',
    'fillRect', 'strokeRect', 'clearRect', 'fillText', 'strokeText', 'drawImage', 'putImageData', 'quadraticCurveTo',
    'bezierCurveTo', 'arcTo', 'rotate']){
    FakeContext.prototype[name] = function(...a){
        this.calls.push([name, ...a])
        if(name === 'fillText' || name === 'strokeText') this.calls[this.calls.length - 1].push(this.s.textAlign, this.s.textBaseline, this.s.transform.slice())
    }
}
for(let name of Object.keys(STATE_DEFAULTS)){
    Object.defineProperty(FakeContext.prototype, name, {
        get(){ return this.s[name] },
        set(v){ this.s[name] = v },
        configurable: true,
    })
}

class FakeCanvas {
    constructor(){
        this.ctx = new FakeContext()
        this.style = {}
        this.w = 0
        this.h = 0
    }
    get width(){ return this.w }
    set width(v){ this.w = v; this.ctx.$domReset() }
    get height(){ return this.h }
    set height(v){ this.h = v; this.ctx.$domReset() }
    getContext(type){ return type === '2d' ? this.ctx : null }
    toDataURL(mime){ return 'data:' + mime + ';base64,AAAA' }
}

// the browser's image loader: an image loads when the test says so
class FakeImage {
    constructor(){ FakeImage.made.push(this) }
    set src(url){ this.url = url }
    get src(){ return this.url }
    finish(){ this.naturalWidth = 4; this.naturalHeight = 4; this.onload() }
}
FakeImage.made = []

function cases(name, runtime){
    const { engine, Item, types, delivery } = runtime
    const { Canvas, Component } = types

    function window(){
        delivery.scene.roots.length = 0
        let root
        engine.batch(() => {
            root = new Item(null)
            root.width = 400
            root.height = 400
        })
        delivery.scene.roots.push(root)
        return root
    }

    function canvasIn(parent, setup){
        let canvas
        engine.batch(() => {
            canvas = new Canvas(parent)
            canvas.$canvas = new FakeCanvas()
            canvas.width = 100
            canvas.height = 50
            if(setup) setup(canvas)
        })
        return canvas
    }

    async function ready(setup){
        let canvas = canvasIn(window(), setup)
        await tick()
        engine.flushFrame()
        return canvas
    }

    test(name + ': available once the canvas is in a window, queued', async () => {
        let alone = canvasIn(null)
        assert.equal(alone.available, false)
        await tick()
        assert.equal(alone.available, false)
        let root = window()
        engine.batch(() => alone.parent = root)
        assert.equal(alone.available, false)
        await tick()
        assert.equal(alone.available, true)
    })

    test(name + ': contextType makes the context when available; getContext("2d") sets it; another type warns', async () => {
        let typed = await ready((it) => { it.contextType = '2d' })
        assert.ok(typed.context)
        assert.equal(typed.getContext('2d'), typed.context)
        let warnings = []
        let warn = console.warn
        console.warn = (m) => warnings.push(m)
        try {
            assert.equal(typed.getContext('webgl'), null)
            let plain = await ready()
            // QQuickCanvasItem::context(): QJSValue() without a context
            assert.equal(plain.context, undefined)
            assert.equal(plain.getContext('2D'), null)
            let ctx = plain.getContext('2d')
            assert.ok(ctx)
            assert.equal(plain.contextType, '2d')
        } finally {
            console.warn = warn
        }
        assert.ok(warnings.some((m) => /different context type/.test(m)))
    })

    test(name + ': canvasSize, tileSize and canvasWindow follow the item until set', async () => {
        let canvas = await ready()
        assert.deepEqual([canvas.canvasSize.width, canvas.canvasSize.height], [100, 50])
        assert.deepEqual([canvas.tileSize.width, canvas.tileSize.height], [100, 50])
        assert.deepEqual([canvas.canvasWindow.x, canvas.canvasWindow.width, canvas.canvasWindow.height], [0, 100, 50])
        engine.batch(() => { canvas.width = 120 })
        assert.equal(canvas.canvasWindow.width, 120)
        engine.batch(() => { canvas.canvasSize = runtime.Qt.size(300, 300) })
        engine.batch(() => { canvas.width = 140 })
        assert.equal(canvas.canvasSize.width, 300)
        assert.equal(canvas.canvasWindow.width, 140)
    })

    test(name + ': paint(region) of the canvas window; painted() only after commands', async () => {
        let regions = [], painted = 0
        let canvas = canvasIn(window(), (it) => { it.contextType = '2d' })
        engine.connect(canvas, Canvas.SIGNAL_paint, (region) => regions.push([region.x, region.y, region.width, region.height]))
        engine.connect(canvas, Canvas.SIGNAL_painted, () => painted++)
        await tick()
        engine.flushFrame()
        assert.deepEqual(regions, [[0, 0, 100, 50]])
        assert.equal(painted, 0)
        engine.connect(canvas, Canvas.SIGNAL_paint, () => canvas.context.fillRect(0, 0, 10, 10))
        engine.batch(() => canvas.requestPaint())
        engine.flushFrame()
        assert.equal(painted, 1)
    })

    test(name + ': requestAnimationFrame: one counter, the time as the argument, before paint; not while hidden', async () => {
        let canvas = await ready((it) => { it.contextType = '2d' })
        let seen = []
        let a = canvas.requestAnimationFrame((time) => seen.push(typeof time))
        let b = canvas.requestAnimationFrame(() => seen.push('second'))
        assert.equal(b, a + 1)
        await tick()
        engine.flushFrame()
        assert.deepEqual(seen, ['number', 'second'])
        engine.batch(() => { canvas.visible = false })
        canvas.requestAnimationFrame(() => seen.push('hidden'))
        await tick()
        engine.flushFrame()
        assert.equal(seen.length, 2)
        engine.batch(() => { canvas.visible = true })
        engine.flushFrame()
        assert.deepEqual(seen.slice(2), ['hidden'])
    })

    test(name + ': the methods return the context; Qt colors in the styles and in the gradient stops', async () => {
        let ctx = (await ready((it) => { it.contextType = '2d' })).context
        assert.equal(ctx.beginPath().moveTo(0, 0).lineTo(5, 5), ctx)
        assert.equal(ctx.save().translate(1, 1).restore(), ctx)
        ctx.fillStyle = '#80ff0000'
        assert.equal(ctx.fillStyle, 'rgba(255,0,0,' + 0x80 / 255 + ')')
        ctx.strokeStyle = 'steelblue'
        assert.equal(ctx.strokeStyle, 'steelblue')
        let gradient = ctx.createLinearGradient(0, 0, 10, 0)
        gradient.addColorStop(0, '#ff00ff00')
        gradient.addColorStop(1, 'red')
        assert.deepEqual(gradient.stops, [[0, 'rgba(0,255,0,1)'], [1, 'red']])
        assert.throws(() => gradient.addColorStop(2, 'red'), /out of range/)
        assert.throws(() => gradient.addColorStop(0.5, 'not a color'), /not a valid color/)
        assert.equal(ctx.createLinearGradient(0, 0), ctx)
    })

    test(name + ': setTransform, resetTransform and isPointInPath in item coordinates over the device pixel ratio', async () => {
        let canvas = canvasIn(window(), (it) => { it.contextType = '2d' })
        canvas.$ratio = () => 2
        await tick()
        let ctx = canvas.context
        let dom = canvas.$canvas.ctx
        assert.deepEqual(dom.s.transform, [2, 0, 0, 2, 0, 0])
        ctx.setTransform(1, 0, 0, 1, 10, 20)
        assert.deepEqual(dom.s.transform, [2, 0, 0, 2, 20, 40])
        ctx.resetTransform()
        assert.deepEqual(dom.s.transform, [2, 0, 0, 2, 0, 0])
        ctx.isPointInPath(3, 4)
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'isPointInPath').pop(), ['isPointInPath', 6, 8, 'nonzero'])
    })

    test(name + ': fillRule: WindingFill by default, part of the state, used by fill and clip', async () => {
        let ctx = (await ready((it) => { it.contextType = '2d' })).context
        let dom = ctx.canvas.$canvas.ctx
        assert.equal(ctx.fillRule, 1)
        ctx.fill()
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'fill').pop(), ['fill', 'nonzero'])
        ctx.save()
        ctx.fillRule = 0
        ctx.fill()
        ctx.clip()
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'fill').pop(), ['fill', 'evenodd'])
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'clip').pop(), ['clip', 'evenodd'])
        ctx.restore()
        assert.equal(ctx.fillRule, 1)
        ctx.fillRule = 'OddEvenFill'
        assert.equal(ctx.fillRule, 0)
    })

    test(name + ': shear, createConicalGradient (counter-clockwise), roundedRect, ellipse, text, unsupported methods', async () => {
        let ctx = (await ready((it) => { it.contextType = '2d' })).context
        let dom = ctx.canvas.$canvas.ctx
        ctx.shear(0.5, 0.25)
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'transform').pop(), ['transform', 1, 0.25, 0.5, 1, 0, 0])
        ctx.resetTransform()
        let conical = ctx.createConicalGradient(10, 20, Math.PI / 2)
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'createConicGradient').pop(), ['createConicGradient', -Math.PI / 2, 10, 20])
        conical.addColorStop(0.25, 'red')
        assert.deepEqual(conical.stops, [[0.75, 'red']])
        ctx.roundedRect(0, 0, 20, 10, 30, 3)
        assert.equal(JSON.stringify(dom.calls.filter((c) => c[0] === 'roundRect').pop()), JSON.stringify(['roundRect', 0, 0, 20, 10, [{ x: 10, y: 3 }]]))
        ctx.ellipse(0, 0, 20, 10)
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'ellipse').pop(), ['ellipse', 10, 5, 10, 5, 0, 0, Math.PI * 2])
        // text(): the glyphs from the left end of the baseline, filled with the path, gone with beginPath
        ctx.textAlign = 'center'
        ctx.text('Qt', 5, 6)
        ctx.fill()
        let filled = dom.calls.filter((c) => c[0] === 'fillText').pop()
        assert.deepEqual(filled.slice(0, 6), ['fillText', 'Qt', 5, 6, 'left', 'alphabetic'])
        assert.equal(ctx.textAlign, 'center')
        ctx.beginPath()
        let count = dom.calls.filter((c) => c[0] === 'fillText').length
        ctx.fill()
        assert.equal(dom.calls.filter((c) => c[0] === 'fillText').length, count)
        assert.throws(() => ctx.drawFocusRing(), /not supported/)
    })

    // QQuickJSContext2DPrototype::method_arc / method_arcTo: a finite negative radius throws THROW_DOM(INDEX_SIZE_ERR,
    // "Incorrect argument radius"), an Error with the code; arcTo returns the context and reaches the canvas
    test(name + ': arc and arcTo refuse a negative radius as Qt, arcTo returns the context', async () => {
        let ctx = (await ready((it) => { it.contextType = '2d' })).context
        let dom = ctx.canvas.$canvas.ctx
        assert.equal(ctx.arcTo(10, 10, 20, 10, 5), ctx)
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'arcTo').pop(), ['arcTo', 10, 10, 20, 10, 5])
        for(let call of [() => ctx.arcTo(10, 10, 20, 10, -3), () => ctx.arc(10, 10, -1, 0, Math.PI)]){
            let error = null
            try { call() } catch(e) { error = e }
            assert.ok(error)
            assert.equal(error.name, 'Error')
            assert.equal(error.message, 'Incorrect argument radius')
            assert.equal(error.code, 1)
        }
        // not a finite number: no check (qt_is_finite)
        ctx.arcTo(0, 0, 1, 1, NaN)
        assert.ok(Number.isNaN(dom.calls.filter((c) => c[0] === 'arcTo').pop()[5]))
    })

    test(name + ': reset(): a new state, an empty stack, the canvas cleared', async () => {
        let ctx = (await ready((it) => { it.contextType = '2d' })).context
        let dom = ctx.canvas.$canvas.ctx
        ctx.save()
        ctx.lineWidth = 7
        ctx.fillRule = 0
        ctx.translate(5, 5)
        ctx.reset()
        assert.equal(ctx.lineWidth, 1)
        assert.equal(ctx.fillRule, 1)
        assert.deepEqual(dom.s.transform, [1, 0, 0, 1, 0, 0])
        assert.equal(ctx.$stack.length, 0)
        assert.deepEqual(dom.calls.filter((c) => c[0] === 'clearRect').pop(), ['clearRect', 0, 0, 100, 50])
    })

    // QQuickContext2DTexture makes a new image for a new canvas size; the QQuickContext2D state is kept
    test(name + ': the state and the saved states survive a new canvas size', async () => {
        let canvas = await ready((it) => { it.contextType = '2d' })
        let ctx = canvas.context
        ctx.lineWidth = 4
        ctx.fillStyle = 'red'
        ctx.translate(10, 0)
        ctx.save()
        ctx.lineWidth = 9
        ctx.fillRule = 0
        engine.batch(() => { canvas.width = 200 })
        assert.equal(ctx.lineWidth, 9)
        assert.equal(ctx.fillRule, 0)
        assert.deepEqual(canvas.$canvas.ctx.s.transform, [1, 0, 0, 1, 10, 0])
        ctx.restore()
        assert.equal(ctx.lineWidth, 4)
        assert.equal(ctx.fillStyle, 'red')
        assert.equal(ctx.fillRule, 1)
    })

    test(name + ': loadImage through the pixmap cache; drawImage of a url, of an Image item; toDataURL', async () => {
        let previous = globalThis.Image
        globalThis.Image = FakeImage
        FakeImage.made = []
        try {
            let first = await ready((it) => { it.contextType = '2d' })
            let loaded = 0
            engine.connect(first, Canvas.SIGNAL_imageLoaded, () => loaded++)
            first.loadImage('http://host/a.png')
            assert.equal(first.isImageLoading('http://host/a.png'), true)
            // not loaded yet: nothing drawn
            first.context.drawImage('http://host/a.png', 0, 0)
            assert.equal(first.$canvas.ctx.calls.filter((c) => c[0] === 'drawImage').length, 0)
            FakeImage.made[0].finish()
            assert.equal(loaded, 1)
            assert.equal(first.isImageLoaded('http://host/a.png'), true)
            first.context.drawImage('http://host/a.png', 1, 2)
            assert.deepEqual(first.$canvas.ctx.calls.filter((c) => c[0] === 'drawImage').pop(), ['drawImage', FakeImage.made[0], 1, 2])
            // another canvas: ready from the cache, no signal, no second load
            let second = canvasIn(first.parent, (it) => { it.contextType = '2d' })
            await tick()
            let again = 0
            engine.connect(second, Canvas.SIGNAL_imageLoaded, () => again++)
            second.loadImage('http://host/a.png')
            assert.equal(second.isImageLoaded('http://host/a.png'), true)
            assert.equal(again, 0)
            assert.equal(FakeImage.made.length, 1)
            // an Image item draws its pixmap
            let image = { __proto__: Item.prototype }
            image = new Item(null)
            image.$url = 'http://host/a.png'
            second.context.drawImage(image, 3, 4)
            assert.deepEqual(second.$canvas.ctx.calls.filter((c) => c[0] === 'drawImage').pop(), ['drawImage', FakeImage.made[0], 3, 4])
            assert.equal(second.toDataURL(), 'data:image/png;base64,AAAA')
            assert.equal(second.toDataURL('image/BMP'), 'data:,')
            assert.equal(second.toDataURL('text/plain'), 'data:,')
            let none = await ready()
            assert.equal(none.toDataURL(), 'data:,')
        } finally {
            globalThis.Image = previous
        }
    })
}

for(let [name, load] of runtimes) cases(name, load())
