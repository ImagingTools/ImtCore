// Shared parts of the benchmarks: statistics, compiling QML, running a compiled application on dist/core.js in a
// context of its own (no DOM, as the unit tests), the heap after gc().
const fs = require('fs')
const os = require('os')
const path = require('path')
const vm = require('vm')
const zlib = require('zlib')

const V5 = path.resolve(__dirname, '../..')
const CORE = path.join(V5, 'dist/core.js')

function stats(samples){
    let sorted = samples.slice().sort((a, b) => a - b)
    let at = (q) => sorted[Math.min(sorted.length - 1, Math.max(0, Math.ceil(q * sorted.length) - 1))]
    let mean = samples.reduce((s, x) => s + x, 0) / samples.length
    let sd = Math.sqrt(samples.reduce((s, x) => s + (x - mean) * (x - mean), 0) / Math.max(1, samples.length - 1))
    let median = sorted.length % 2 ? sorted[(sorted.length - 1) / 2] : (sorted[sorted.length / 2 - 1] + sorted[sorted.length / 2]) / 2
    return { median, p95: at(0.95), mean, sd, min: sorted[0], max: sorted[sorted.length - 1], n: samples.length }
}

const now = () => Number(process.hrtime.bigint()) / 1e6

function gc(){
    if(typeof global.gc !== 'function') throw new Error('run with node --expose-gc')
    global.gc()
    global.gc()
}

function heapUsed(){
    gc()
    return process.memoryUsage().heapUsed
}

function gzipSize(text){
    return zlib.gzipSync(Buffer.from(text, 'utf8'), { level: 9 }).length
}

// compiles the QML files { 'Main.qml': text, ... } (Main.qml is the entry); returns the application code without the
// runtime and without the bootstrap's mounting
function compileQml(documents, options = {}){
    const { compile } = require(path.join(V5, 'compiler/index'))
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-bench-'))
    try {
        for(let [name, text] of Object.entries(documents)){
            let file = path.join(root, 'app', name)
            fs.mkdirSync(path.dirname(file), { recursive: true })
            fs.writeFileSync(file, text)
        }
        let output = path.join(root, 'out')
        let result = compile({ mode: 'js', entry: path.join(root, 'app', 'Main.qml'), output, name: 'index', minify: !!options.minify })
        if(result.warnings.length && !options.allowWarnings) throw new Error(result.warnings.join('\n'))
        return applicationPart(fs.readFileSync(path.join(output, 'index.js'), 'utf8'))
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

// the generated code of a compiled bundle (after the runtime and the QML compiler)
function applicationPart(bundle){
    let at = bundle.lastIndexOf('\n;(function(){')
    if(at < 0) at = bundle.lastIndexOf('\n(function(){')
    return bundle.slice(at + 1)
}

// a context with the runtime and the application; mount: build the root object as the page does
function load(application, options = {}){
    let loads = []
    let context = { console: options.console || quietConsole(), setTimeout, clearTimeout, queueMicrotask, performance }
    context.window = context
    // manual frames: requestAnimationFrame queues, frame() runs the queue with the clock 16 ms later
    let frames = [], time = 0
    if(options.manualFrames){
        context.performance = { now: () => time }
        context.requestAnimationFrame = (fn) => { frames.push(fn); return frames.length }
        context.cancelAnimationFrame = () => {}
    }
    const frame = () => {
        time += 16
        let due = frames
        frames = []
        for(let fn of due) fn(time)
    }
    vm.createContext(context)
    vm.runInContext(fs.readFileSync(options.core || CORE, 'utf8'), context)
    context.addEventListener = (type, fn) => { if(type === 'load') loads.push(fn) }
    context.location = { pathname: '/', origin: '', href: '/' }
    context.innerWidth = 1600
    context.innerHeight = 1000
    // the bootstrap without mounting into a document
    // (a minified bundle has its own name for content)
    vm.runInContext(application.replace(/[\w$]+\.\$mount\(document\.body\)/, '0'), context)
    if(options.mount !== false && loads[0]) loads[0]()
    return { context, JQ: context.JQ, root: context.__root, frame }
}

function quietConsole(){
    let errors = { count: 0 }
    let c = { log(){}, info(){}, debug(){}, warn(){ errors.count++ }, error(){ errors.count++ }, errors }
    return c
}

// runs fn() runs times after warmup runs; fn returns the time it measured, or undefined to be timed whole
function measure(fn, { runs = 10, warmup = 2 } = {}){
    let samples = []
    for(let i = 0; i < warmup + runs; i++){
        let start = now()
        let measured = fn(i)
        let time = typeof measured === 'number' ? measured : now() - start
        if(i >= warmup) samples.push(time)
    }
    return samples
}

module.exports = { V5, CORE, stats, now, gc, heapUsed, gzipSize, compileQml, applicationPart, load, measure }
