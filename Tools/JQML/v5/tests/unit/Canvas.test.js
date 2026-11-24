// Run: node --test tests/unit
// Canvas and Context2D as qquickcanvasitem.cpp and qquickcontext2d.cpp (Qt 6.12), the context drawing on a stand-in for
// the DOM canvas that records what it is told (a new size resets its DOM state, as the browser's).
// - available once in a window (queued); contextType makes the context; getContext("2d") sets it; another type warns;
// - canvasSize, tileSize and canvasWindow follow the item until set; paint(region), painted() when commands went to the
//   canvas (once after the context is made: its reset()); requestAnimationFrame callbacks first;
// - the state as Qt reads and checks it (the values below are Qt 6.12's: qml -platform offscreen), the returns and the
//   exceptions of the methods, the pixel array of an image data;
// - what goes to the DOM: the matrix over the device pixel ratio (transform() after the current matrix, as Qt), the
//   dashes in units of the line width, Qt's compositions, shadows through the matrix, arcs as Qt's curves, the clips
//   of the states again after a new canvas size; loadImage through a shared cache; toDataURL.
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
    save(){ this.calls.push(['save']); this.domStack.push(JSON.parse(JSON.stringify(this.s))) }
    restore(){ this.calls.push(['restore']); if(this.domStack.length) this.s = this.domStack.pop() }
    measureText(text){ return { width: text.length * 6.4, fontBoundingBoxAscent: 9.2, fontBoundingBoxDescent: 2.4 } }
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
    get canvas(){ return this.owner }
}

// Path2D: the segments it is given
class FakePath2D {
    constructor(){ this.ops = [] }
}
for(let name of ['moveTo', 'lineTo', 'bezierCurveTo', 'closePath']) FakePath2D.prototype[name] = function(...a){ this.ops.push([name, ...a.map((v) => Math.round(v * 1000) / 1000)]) }
globalThis.Path2D = FakePath2D
for(let name of ['beginPath', 'moveTo', 'lineTo', 'closePath', 'ellipse', 'roundRect', 'rect', 'arc', 'fill', 'stroke', 'clip',
    'fillRect', 'strokeRect', 'clearRect', 'fillText', 'strokeText', 'drawImage', 'putImageData', 'quadraticCurveTo',
    'bezierCurveTo', 'arcTo', 'rotate']){
    FakeContext.prototype[name] = function(...a){
        this.calls.push([name, ...a])
        // the state the command was drawn with
        this.drawn = Object.assign({}, this.s)
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
        this.ctx.owner = this
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

    test(name + ': paint(region) of the canvas window; painted() when commands went to the canvas', async () => {
        let regions = [], painted = 0
        let canvas = canvasIn(window(), (it) => { it.contextType = '2d' })
        engine.connect(canvas, Canvas.SIGNAL_paint, (region) => regions.push([region.x, region.y, region.width, region.height]))
        engine.connect(canvas, Canvas.SIGNAL_painted, () => painted++)
        await tick()
        engine.flushFrame()
        assert.deepEqual(regions, [[0, 0, 100, 50]])
        // the reset() of the new context is a command
        assert.equal(painted, 1)
        engine.batch(() => canvas.requestPaint())
        engine.flushFrame()
        assert.equal(painted, 1)
        engine.connect(canvas, Canvas.SIGNAL_paint, () => canvas.context.fillRect(0, 0, 10, 10))
        engine.batch(() => canvas.requestPaint())
        engine.flushFrame()
        assert.equal(painted, 2)
        // reading the pixels sends the commands at once
        canvas.context.fillRect(0, 0, 5, 5)
        canvas.toDataURL()
        assert.equal(painted, 3)
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


    const context = async () => (await ready((it) => { it.contextType = '2d' })).context
    const domOf = (ctx) => ctx.canvas.$canvas.ctx
    const lastCall = (dom, name) => dom.calls.filter((c) => c[0] === name).pop()
    // values of the runtime of dist/core.js come from another realm: compared as JSON
    const same = (a, b, m) => assert.deepEqual(JSON.parse(JSON.stringify(a)), b, m)
    const errorOf = (f) => { try { f(); return 'none' } catch(e){ return e.code + ' ' + e.message } }

    test(name + ': the state as Qt reads it: defaults, colors, numbers, names, fill rule, fonts, dashes', async () => {
        let ctx = await context()
        let defaults = {}
        for(let p of ['fillStyle', 'strokeStyle', 'globalAlpha', 'globalCompositeOperation', 'lineWidth', 'lineCap', 'lineJoin', 'miterLimit', 'lineDashOffset',
            'shadowOffsetX', 'shadowOffsetY', 'shadowBlur', 'shadowColor', 'font', 'textAlign', 'textBaseline', 'fillRule']) defaults[p] = ctx[p]
        same(defaults, { fillStyle: '#000000', strokeStyle: '#000000', globalAlpha: 1, globalCompositeOperation: 'source-over', lineWidth: 1,
            lineCap: 'butt', lineJoin: 'miter', miterLimit: 10, lineDashOffset: 0, shadowOffsetX: 0, shadowOffsetY: 0, shadowBlur: 0, shadowColor: '#000000',
            font: 'sans-serif,-1,10,5,400,0,0,0,0,0,0,0,0,0,0,1,,0,0', textAlign: 'start', textBaseline: 'alphabetic', fillRule: 1 })
        let colors = []
        for(let c of ['red', '#80ff0000', '#f00', 'rgba(1, 2, 3, 0.5)', 'rgb(10,20,30)', 'hsl(120, 100%, 50%)', 'transparent', 'foo', '#12345', '']){
            ctx.fillStyle = '#000000'
            ctx.fillStyle = c
            ctx.strokeStyle = c
            ctx.shadowColor = c
            colors.push([ctx.fillStyle, ctx.strokeStyle, ctx.shadowColor].join(' '))
        }
        same(colors, ['#ff0000 #ff0000 #ff0000', 'rgba(255, 0, 0, 0.501961) rgba(255, 0, 0, 0.501961) #ff0000', '#ff0000 #ff0000 #ff0000',
            'rgba(1, 2, 3, 0.501961) rgba(1, 2, 3, 0.501961) #010203', '#0a141e #0a141e #0a141e', '#01ff01 #01ff01 #01ff01',
            'rgba(0, 0, 0, 0.0) rgba(0, 0, 0, 0.0) #000000', '#000000 rgba(0, 0, 0, 0.0) #000000', '#000000 rgba(0, 0, 0, 0.0) #000000',
            '#000000 rgba(0, 0, 0, 0.0) #000000'])
        let numbers = {}
        for(let p of ['lineWidth', 'miterLimit', 'globalAlpha', 'shadowBlur', 'shadowOffsetX', 'lineDashOffset']){
            let out = []
            for(let v of [3, 0, -1, NaN, Infinity, 0.5, '2', 2.5]){
                ctx[p] = v
                out.push(ctx[p])
            }
            numbers[p] = out.join(' ')
            ctx.reset()
        }
        same(numbers, { lineWidth: '3 3 3 3 3 0.5 2 2.5', miterLimit: '3 3 3 3 3 0.5 2 2.5', globalAlpha: '1 0 0 0 0 0.5 0.5 0.5',
            shadowBlur: '3 3 3 3 3 0.5 2 2.5', shadowOffsetX: '3 0 -1 -1 -1 0.5 2 2.5', lineDashOffset: '3 0 -1 -1 -1 0.5 2 2.5' })
        let names = {}
        for(let p of ['lineCap', 'lineJoin', 'textAlign', 'textBaseline', 'globalCompositeOperation']){
            let out = []
            for(let v of ['round', 'bevel', 'center', 'middle', 'ideographic', 'qt-multiply', 'multiply', 'qt-lighten', 'bogus']){
                ctx[p] = v
                out.push(ctx[p])
            }
            names[p] = out.join(' ')
            ctx.reset()
        }
        same(names, { lineCap: 'round round round round round round round round round', lineJoin: 'round bevel bevel bevel bevel bevel bevel bevel bevel',
            textAlign: 'start start center center center center center center center', textBaseline: 'alphabetic alphabetic alphabetic middle middle middle middle middle middle',
            globalCompositeOperation: 'source-over source-over source-over source-over source-over qt-multiply qt-multiply lighter lighter' })
        let rules = []
        for(let v of [0, 1, 'OddEvenFill', 'WindingFill', 'bogus', 7]){
            ctx.fillRule = v
            rules.push(ctx.fillRule)
        }
        same(rules, [0, 1, 0, 1, 1, 1])
        let warn = console.warn
        console.warn = () => {}
        try {
            let fonts = []
            for(let f of ['bold 20px Arial', 'normal normal 600 16px \'Times New Roman\'', 'oblique bold 9px Arial', 'italic 12pt Arial', '20px', 'bogus', 'bold bold 10px Arial']){
                ctx.font = '10px Arial'
                ctx.font = f
                fonts.push(ctx.font)
            }
            same(fonts, ['Arial,-1,20,5,700,0,0,0,0,0,0,0,0,0,0,1,,0,0', 'Times New Roman,-1,16,5,600,0,0,0,0,0,0,0,0,0,0,1,,0,0',
                'Arial,-1,9,5,700,2,0,0,0,0,0,0,0,0,0,1,,0,0', 'Arial,12,-1,5,400,1,0,0,0,0,0,0,0,0,0,1,,0,0', 'Arial,-1,10,5,400,0,0,0,0,0,0,0,0,0,0,1,,0,0',
                'Arial,-1,10,5,400,0,0,0,0,0,0,0,0,0,0,1,,0,0', 'Arial,-1,10,5,400,0,0,0,0,0,0,0,0,0,0,1,,0,0'])
        } finally {
            console.warn = warn
        }
        ctx.setLineDash([5, 3, 2])
        same(ctx.getLineDash(), [5, 3, 2, 5, 3, 2])
        ctx.setLineDash([1, -2])
        ctx.setLineDash([4, NaN])
        same(ctx.getLineDash(), [5, 3, 2, 5, 3, 2])
        ctx.setLineDash([])
        same(ctx.getLineDash(), [])
        // a gradient reads back as itself, a pattern of a color as the color, a state restored
        let g = ctx.createLinearGradient(0, 0, 1, 1)
        ctx.fillStyle = g
        assert.equal(ctx.fillStyle, g)
        ctx.fillStyle = ctx.createPattern('#00ff00', 4)
        assert.equal(ctx.fillStyle, '#00ff00')
        ctx.reset()
        ctx.fillStyle = 'red'
        ctx.save()
        ctx.fillStyle = 'blue'
        ctx.lineWidth = 7
        ctx.save()
        ctx.fillStyle = 'green'
        ctx.restore()
        same([ctx.fillStyle, ctx.lineWidth], ['#0000ff', 7])
        ctx.restore()
        ctx.restore()
        same([ctx.fillStyle, ctx.lineWidth], ['#ff0000', 1])
    })

    test(name + ': what the methods return and throw, as Qt', async () => {
        let ctx = await context()
        let rets = []
        let calls = {
            save: () => ctx.save(), restore: () => ctx.restore(), beginPath: () => ctx.beginPath(), moveTo: () => ctx.moveTo(1, 1), arc: () => ctx.arc(5, 5, 2, 0, 1),
            ellipse: () => ctx.ellipse(0, 0, 4, 4), roundedRect: () => ctx.roundedRect(0, 0, 4, 4, 1, 1), text: () => ctx.text('a', 0, 0), fill: () => ctx.fill(),
            clip: () => ctx.clip(), fillRect: () => ctx.fillRect(0, 0, 0, 0), shear: () => ctx.shear(0, 0), reset: () => ctx.reset(), setLineDash: () => ctx.setLineDash([]),
            drawImage: () => ctx.drawImage('nothing.png', 0, 0), putImageData: () => ctx.putImageData(ctx.createImageData(1, 1), 0, 0),
            isPointInPath: () => ctx.isPointInPath(0, 0), lineToNaN: () => ctx.lineTo(NaN, 1), arcFew: () => ctx.arc(0, 0), fillRectFew: () => ctx.fillRect(0, 0),
            linearFew: () => ctx.createLinearGradient(0, 0), createPatternFew: () => ctx.createPattern('red'), createImageDataFew: () => ctx.createImageData(),
            getImageDataFew: () => ctx.getImageData(0, 0), measureTextNone: () => ctx.measureText(),
        }
        for(let n in calls){
            let v = calls[n]()
            rets.push(n + '=' + (v === ctx ? 'ctx' : v === undefined ? 'undefined' : v === null ? 'null' : typeof v === 'object' ? 'object' : String(v)))
        }
        same(rets, ['save=ctx', 'restore=ctx', 'beginPath=ctx', 'moveTo=ctx', 'arc=ctx', 'ellipse=ctx', 'roundedRect=ctx', 'text=ctx', 'fill=ctx',
            'clip=ctx', 'fillRect=ctx', 'shear=ctx', 'reset=ctx', 'setLineDash=undefined', 'drawImage=undefined', 'putImageData=undefined',
            'isPointInPath=false', 'lineToNaN=undefined', 'arcFew=ctx', 'fillRectFew=ctx', 'linearFew=ctx', 'createPatternFew=undefined',
            'createImageDataFew=undefined', 'getImageDataFew=null', 'measureTextNone=undefined'])
        same([
            errorOf(() => ctx.arc(0, 0, -1, 0, 1)), errorOf(() => ctx.arcTo(0, 0, 1, 1, -1)),
            errorOf(() => ctx.createLinearGradient(0, 0, 1, 1).addColorStop(2, 'red')), errorOf(() => ctx.createLinearGradient(0, 0, 1, 1).addColorStop(0.5, 'foo')),
            errorOf(() => ctx.createLinearGradient(0, 0, NaN, 1)), errorOf(() => ctx.createRadialGradient(0, 0, -1, 0, 0, 1)),
            errorOf(() => ctx.createConicalGradient(NaN, 0, 0)), errorOf(() => ctx.createConicalGradient(0, 0, NaN)),
            errorOf(() => ctx.createImageData(0, 1)), errorOf(() => ctx.getImageData(0, 0, 0, 1)), errorOf(() => ctx.getImageData(NaN, 0, 1, 1)),
            errorOf(() => ctx.putImageData(5, 0, 0, 0, 0, 1, 1)), errorOf(() => ctx.putImageData(ctx.createImageData(1, 1), NaN, 0, 0, 0, 1, 1)),
            errorOf(() => ctx.drawImage(5, 0, 0)), errorOf(() => ctx.drawFocusRing()), errorOf(() => ctx.caretBlinkRate()),
            errorOf(() => ctx.setCaretSelectionRect()),
        ], ['1 Incorrect argument radius', '1 Incorrect argument radius', '1 CanvasGradient: parameter offset out of range',
            '12 CanvasGradient: parameter color is not a valid color string', '9 createLinearGradient(): Incorrect arguments',
            '1 createRadialGradient(): Incorrect arguments', '9 createConicalGradient(): Incorrect arguments', '1 createConicalGradient(): Incorrect arguments',
            '1 createImageData(): invalid arguments', '1 getImageData(): Invalid arguments', '9 getImageData(): Invalid arguments',
            '17 Context2D::putImageData, the image data type mismatch', '9 putImageData() : Invalid arguments', '17 drawImage(), type mismatch',
            '9 Context2D::drawFocusRing is not supported', '9 Context2D::caretBlinkRate is not supported', '9 Context2D::setCaretSelectionRect is not supported'])
        same(Object.keys(ctx.measureText('Hello')), ['width'])
        assert.equal(ctx.measureText('Hello').width, 32)
    })

    test(name + ': an image data: width, height, a pixel array of integers 0..255', async () => {
        let ctx = await context()
        let d = ctx.createImageData(2, 3)
        same([d.width, d.height, d.data.length, d.data[0], d.data[24] === undefined], [2, 3, 24, 0, true])
        d.data[0] = 300
        d.data[1] = -5
        d.data[2] = 12.7
        d.data[3] = '40'
        same([d.data[0], d.data[1], d.data[2], d.data[3]], [0, 0, 12, 40])
        let copy = ctx.createImageData(d)
        same([copy.width, copy.height, copy.data[3]], [2, 3, 0])
        assert.equal(ctx.createImageData(2.6, 1.4).width, 3)
    })

    test(name + ': the DOM gets the matrix over the device pixel ratio; transform() after the current matrix, as Qt', async () => {
        let canvas = await ready((it) => { it.contextType = '2d' })
        canvas.$ratio = () => 2
        let ctx = canvas.context
        let dom = domOf(ctx)
        ctx.scale(2, 2)
        ctx.transform(1, 0, 0, 1, 10, 0)
        ctx.fillRect(0, 0, 5, 5)
        // scale then the translation in the item's coordinates: (2 x, 2 y) + (10, 0), times the ratio
        same(lastCall(dom, 'setTransform').slice(1), [4, 0, 0, 4, 20, 0])
        ctx.setTransform(1, 0, 0, 1, 30, 0)
        ctx.fillRect(0, 0, 5, 5)
        same(lastCall(dom, 'setTransform').slice(1), [2, 0, 0, 2, 60, 0])
        // a matrix that cannot be inverted: nothing is drawn until the matrix is set again
        ctx.scale(0, 1)
        let before = dom.calls.filter((c) => c[0] === 'fillRect').length
        ctx.fillRect(0, 0, 5, 5)
        assert.equal(dom.calls.filter((c) => c[0] === 'fillRect').length, before)
        ctx.resetTransform()
        ctx.fillRect(0, 0, 5, 5)
        assert.equal(dom.calls.filter((c) => c[0] === 'fillRect').length, before + 1)
        // isPointInPath: the point of the item over the ratio
        ctx.beginPath()
        ctx.rect(10, 10, 20, 20)
        ctx.isPointInPath(15, 15)
        let call = lastCall(dom, 'isPointInPath')
        same(call.slice(2, 5), [30, 30, 'nonzero'])
        same(call[1].ops[0], ['moveTo', 10, 10])
    })

    test(name + ': the pen: dashes in units of the width; compositions, shadows through the matrix; a gradient as it was set', async () => {
        let ctx = await context()
        let dom = domOf(ctx)
        ctx.lineWidth = 3
        ctx.setLineDash([2, 1])
        ctx.lineDashOffset = 1
        ctx.strokeRect(0, 0, 10, 10)
        same([dom.drawn.dash, dom.drawn.lineDashOffset, dom.drawn.lineWidth], [[6, 3], 3, 3])
        ctx.globalCompositeOperation = 'qt-multiply'
        ctx.fillRect(0, 0, 1, 1)
        assert.equal(dom.drawn.globalCompositeOperation, 'multiply')
        ctx.reset()
        ctx.scale(2, 1)
        ctx.shadowColor = 'black'
        ctx.shadowOffsetX = 5
        ctx.shadowBlur = 4
        ctx.fillRect(0, 0, 1, 1)
        same([dom.drawn.shadowOffsetX, dom.drawn.shadowOffsetY, Math.round(dom.drawn.shadowBlur * 1000) / 1000], [10, 0, 5.657])
        ctx.reset()
        let g = ctx.createLinearGradient(0, 0, 100, 0)
        g.addColorStop(0, 'red')
        ctx.fillStyle = g
        g.addColorStop(1, 'blue')
        g.addColorStop(0, '#00ff00')
        ctx.fillRect(0, 0, 1, 1)
        same(dom.drawn.fillStyle.stops, [[0, 'rgba(255,0,0,1)']])
        let c = ctx.createConicalGradient(5, 5, Math.PI / 2)
        c.addColorStop(0, 'black')
        c.addColorStop(0.25, 'white')
        ctx.fillStyle = c
        ctx.fillRect(0, 0, 1, 1)
        // QConicalGradient turns counter-clockwise: the DOM's (clockwise) from -angle with the stops mirrored
        same(lastCall(dom, 'createConicGradient').slice(1), [-Math.PI / 2, 5, 5])
        same(dom.drawn.fillStyle.stops, [[0.75, 'rgba(255,255,255,1)'], [1, 'rgba(0,0,0,1)']])
    })

    test(name + ': the path as QPainterPath: arcs and ellipses as Qt\'s curves, closed subpaths, the same point once', async () => {
        let ctx = await context()
        let dom = domOf(ctx)
        ctx.beginPath()
        ctx.arc(50, 40, 20, 0, Math.PI, false)
        ctx.fill()
        let path = lastCall(dom, 'fill')[1]
        same(path.ops[0], ['moveTo', 70, 40])
        assert.equal(path.ops.filter((o) => o[0] === 'bezierCurveTo').length, 2)
        same(path.ops[path.ops.length - 1].slice(-2), [30, 40])
        ctx.beginPath()
        ctx.ellipse(10, 10, 40, 20)
        ctx.fill()
        path = lastCall(dom, 'fill')[1]
        same(path.ops.map((o) => o[0]), ['moveTo', 'bezierCurveTo', 'bezierCurveTo', 'bezierCurveTo', 'bezierCurveTo', 'closePath'])
        same(path.ops[0], ['moveTo', 50, 20])
        ctx.beginPath()
        ctx.roundedRect(10, 10, 40, 40, 10, 10)
        ctx.fill()
        path = lastCall(dom, 'fill')[1]
        same(path.ops[0], ['moveTo', 10, 20])
        assert.equal(path.ops.filter((o) => o[0] === 'bezierCurveTo').length, 4)
        // a point: no line to it; closePath of a point does nothing; a rectangle of no size is a move
        ctx.beginPath()
        ctx.moveTo(1, 1)
        ctx.lineTo(1, 1)
        ctx.closePath()
        ctx.lineTo(5, 1)
        ctx.rect(20, 20, 0, 0)
        ctx.stroke()
        same(lastCall(dom, 'stroke')[1].ops, [['moveTo', 1, 1], ['lineTo', 5, 1], ['moveTo', 20, 20]])
        // fill and stroke of an empty path draw nothing
        let fills = dom.calls.filter((c) => c[0] === 'fill').length
        ctx.beginPath()
        ctx.fill()
        assert.equal(dom.calls.filter((c) => c[0] === 'fill').length, fills)
        // the path stays where it was made across a change of the matrix
        ctx.beginPath()
        ctx.save()
        ctx.translate(10, 10)
        ctx.moveTo(0, 0)
        ctx.restore()
        ctx.lineTo(0, 20)
        ctx.stroke()
        same(lastCall(dom, 'stroke')[1].ops, [['moveTo', 10, 10], ['lineTo', 0, 20]])
        // an SVG path
        ctx.path = 'M 10 10 L 60 10 L 60 60 Z'
        assert.equal(ctx.path, 'M 10 10 L 60 10 L 60 60 Z')
        ctx.fill()
        same(lastCall(dom, 'fill')[1].ops, [['moveTo', 10, 10], ['lineTo', 60, 10], ['lineTo', 60, 60], ['lineTo', 10, 10], ['closePath']])
    })

    test(name + ': the clips of the states again after a new canvas size; reset() takes the clip away', async () => {
        let canvas = await ready((it) => { it.contextType = '2d' })
        let ctx = canvas.context
        ctx.beginPath()
        ctx.rect(0, 0, 10, 10)
        ctx.clip()
        ctx.save()
        ctx.lineWidth = 9
        ctx.beginPath()
        ctx.rect(5, 5, 10, 10)
        ctx.clip()
        engine.batch(() => { canvas.width = 200 })
        let dom = domOf(ctx)
        let after = dom.calls.slice(dom.calls.map((c) => c[0]).lastIndexOf('save') - 1)
        assert.equal(dom.calls.filter((c) => c[0] === 'clip').length >= 4, true)
        assert.equal(ctx.lineWidth, 9)
        ctx.restore()
        assert.equal(ctx.lineWidth, 1)
        assert.ok(after.length)
        let saves = dom.domStack.length
        ctx.reset()
        assert.equal(dom.domStack.length, 1)
        assert.ok(saves >= 1)
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
            // drawImage(image, x, y): the whole image at its size (QQuickContext2DCommandBuffer::drawPixmap)
            assert.deepEqual(first.$canvas.ctx.calls.filter((c) => c[0] === 'drawImage').pop(), ['drawImage', FakeImage.made[0], 0, 0, 4, 4, 1, 2, 4, 4])
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
            assert.deepEqual(second.$canvas.ctx.calls.filter((c) => c[0] === 'drawImage').pop(), ['drawImage', FakeImage.made[0], 0, 0, 4, 4, 3, 4, 4, 4])
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
