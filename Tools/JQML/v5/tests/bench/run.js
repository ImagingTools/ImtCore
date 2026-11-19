// JQML v5 benchmarks.
//   node --expose-gc tests/bench/run.js [--suite size,compile,docs,micro,mem] [--runs 10] [--out file.json] [--filter re]
// Every metric: warmup runs, then --runs samples; the JSON keeps the samples and their median, p95, mean, sd.
// Compare two results with: node tests/bench/compare.mjs base.json new.json
const fs = require('fs')
const path = require('path')
const { CORE, stats, now, gc, heapUsed, gzipSize, applicationPart, load, measure, disposeAll } = require('./lib')
const { available } = require('./apps')
const micro = require('./micro')

function args(){
    let options = { suite: 'size,docs,micro,mem', runs: 10, out: '', filter: '' }
    let argv = process.argv.slice(2)
    for(let i = 0; i < argv.length; i++){
        let key = argv[i].replace(/^--/, '')
        options[key] = argv[++i]
    }
    options.runs = +options.runs
    options.suites = new Set(options.suite.split(','))
    return options
}

const options = args()
const results = {}

// several suites: each in a process of its own, so one does not leave garbage and timers to the next
const ORDER = ['size', 'compile', 'docs', 'micro', 'mem']
if(options.suites.size > 1){
    const { execFileSync } = require('child_process')
    const os = require('os')
    let merged = {}
    let tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-bench-run-'))
    for(let suite of ORDER.filter(s => options.suites.has(s))){
        let out = path.join(tmp, suite + '.json')
        let argv = ['--expose-gc', __filename, '--suite', suite, '--runs', String(options.runs), '--out', out]
        if(options.filter) argv.push('--filter', options.filter)
        execFileSync(process.execPath, argv, { stdio: ['ignore', 'inherit', 'inherit'] })
        Object.assign(merged, JSON.parse(fs.readFileSync(out, 'utf8')).metrics)
    }
    fs.rmSync(tmp, { recursive: true, force: true })
    let output = { date: new Date().toISOString(), node: process.version, commit: commitOf(), runs: options.runs, metrics: merged }
    if(options.out) fs.writeFileSync(options.out, JSON.stringify(output, null, 1))
    else process.stdout.write(JSON.stringify(output, null, 1) + '\n')
    process.exit(0)
}

function commitOf(){
    try { return require('child_process').execSync('git rev-parse --short HEAD', { cwd: __dirname }).toString().trim() } catch(e) { return '' }
}
const filter = options.filter ? new RegExp(options.filter) : null

function record(name, unit, samples, extra){
    if(filter && !filter.test(name)) return
    results[name] = { unit, ...stats(samples), samples, ...(extra || {}) }
    let s = results[name]
    process.stderr.write(name.padEnd(46) + ' ' + s.median.toFixed(unit === 'ms' ? 3 : 0).padStart(14) + ' ' + unit + '  p95 ' + s.p95.toFixed(unit === 'ms' ? 3 : 0) + '  sd ' + s.sd.toFixed(unit === 'ms' ? 3 : 0) + '\n')
}
const wanted = (name) => !filter || filter.test(name)

// ---- sizes: the runtime and the bundles of the applications (deterministic) ----

const bundles = new Map()       // app name -> compiled bundle (one compile, minified as a Release build)
function bundleOf(app){
    if(bundles.has(app.name)) return bundles.get(app.name)
    const { compile } = require('../../compiler/index')
    let out = fs.mkdtempSync(path.join(require('os').tmpdir(), 'jqml-app-'))
    let log = console.log
    console.log = () => {}
    try {
        compile({ mode: 'js', config: app.config, entry: app.entry, output: out, name: 'index', root: app.root, minify: true })
    } finally {
        console.log = log
    }
    let code = fs.readFileSync(path.join(out, 'index.js'), 'utf8')
    fs.rmSync(out, { recursive: true, force: true })
    bundles.set(app.name, code)
    return code
}

if(options.suites.has('size')){
    let core = fs.readFileSync(CORE, 'utf8')
    record('size.core.raw', 'B', [Buffer.byteLength(core)])
    record('size.core.gzip', 'B', [gzipSize(core)])
    for(let app of available()){
        if(!wanted('size.' + app.name)) continue
        let bundle = bundleOf(app)
        record('size.' + app.name + '.raw', 'B', [Buffer.byteLength(bundle)])
        record('size.' + app.name + '.gzip', 'B', [gzipSize(bundle)])
    }
}

// ---- compile: parse, model, codegen, minify of each application ----

if(options.suites.has('compile')){
    const parser = require('../../compiler/parser')
    const { FileModel } = require('../../compiler/model')
    const { Codegen } = require('../../compiler/codegen')
    const uglify = require('../../node_modules/uglify-js')
    const { compile } = require('../../compiler/index')
    let phase = { parse: 0, model: 0, codegen: 0, minify: 0 }
    let wrap = (owner, key, name) => {
        let base = owner[key]
        owner[key] = function(){
            let start = now()
            try { return base.apply(this, arguments) } finally { phase[name] += now() - start }
        }
    }
    wrap(parser, 'parse', 'parse')
    wrap(FileModel.prototype, 'build', 'model')
    wrap(Codegen.prototype, 'generate', 'codegen')
    wrap(uglify, 'minify', 'minify')
    for(let app of available()){
        if(!wanted('compile.' + app.name)) continue
        let samples = { parse: [], model: [], codegen: [], minify: [], total: [] }
        let out = fs.mkdtempSync(path.join(require('os').tmpdir(), 'jqml-compile-'))
        let runs = Math.max(3, Math.min(options.runs, +(process.env.COMPILE_RUNS || options.runs)))
        for(let i = 0; i < runs + 1; i++){
            for(let k in phase) phase[k] = 0
            let log = console.log
            console.log = () => {}
            let start = now()
            try {
                compile({ mode: 'js', config: app.config, entry: app.entry, output: out, name: 'index', root: app.root, minify: true })
            } finally {
                console.log = log
            }
            let total = now() - start
            if(i === 0) continue
            for(let k in phase) samples[k].push(phase[k])
            samples.total.push(total)
        }
        fs.rmSync(out, { recursive: true, force: true })
        for(let k in samples) record('compile.' + app.name + '.' + k, 'ms', samples[k])
    }
}

// ---- docs: every document of an application created and destroyed ----

// the documents of the bundle that build: created one by one in a parent item, each destroyed after it
function documentsOf(JQ){
    let keys = Object.keys(JQ.files).filter(key => key.endsWith('.qml')).sort()
    return keys
}

function createAll(JQ, keys, parent, onCreated){
    let created = []
    for(let key of keys){
        let Type
        try {
            Type = JQ.files[key] && JQ.files[key].$lazy ? JQ.files[key]() : JQ.files[key]
        } catch(error) {
            continue
        }
        if(typeof Type !== 'function') continue
        let obj = null
        try {
            obj = JQ.create(() => new Type(parent))
        } catch(error) {
            obj = null
        }
        if(obj) created.push(obj)
        if(onCreated) onCreated(key, obj)
    }
    return created
}

if(options.suites.has('docs')){
    for(let app of available()){
        if(!wanted('docs.' + app.name)) continue
        let application = applicationPart(bundleOf(app))
        // documents that build without errors in the first pass are the ones measured: the same set in every run
        const progress = (text) => { if(process.env.BENCH_DEBUG) process.stderr.write('[docs] ' + text + '\n') }
        progress('loaded bundle')
        let probe = load(application)
        progress('probe context')
        let keys = []
        let parent = probe.root || new probe.JQ.types.Item(null)
        createAll(probe.JQ, documentsOf(probe.JQ), parent, (key, obj) => { if(obj) keys.push(key) })
        probe.dispose()
        progress('probe: ' + keys.length + ' documents')
        let createSamples = [], destroySamples = [], heapSamples = []
        for(let i = 0; i < options.runs + 1; i++){
            let { JQ, root, dispose } = load(application)
            let holder = JQ.create(() => new JQ.types.Item(root || null))
            let before = heapUsed()
            let start = now()
            let created = createAll(JQ, keys, holder)
            JQ.engine.flushFrame()
            let createTime = now() - start
            let after = heapUsed()
            start = now()
            JQ.batch(() => { for(let obj of created) obj.destroy() })
            JQ.engine.flushFrame()
            let destroyTime = now() - start
            dispose()
            progress('run ' + i + ' create ' + createTime.toFixed(0) + ' destroy ' + destroyTime.toFixed(0))
            if(i === 0) continue
            createSamples.push(createTime)
            destroySamples.push(destroyTime)
            heapSamples.push((after - before) / Math.max(1, created.length))
        }
        record('docs.' + app.name + '.create', 'ms', createSamples, { documents: keys.length })
        record('docs.' + app.name + '.destroy', 'ms', destroySamples)
        record('docs.' + app.name + '.heapPerDocument', 'B', heapSamples)
    }
}

// ---- micro: bindings, signals, frames ----

if(options.suites.has('micro')){
    for(let bench of micro.benchmarks){
        if(!wanted(bench.name)) continue
        let samples = bench.run(options.runs)
        disposeAll()
        gc()
        record(bench.name, bench.unit || 'ms', samples)
    }
}

// ---- mem: retained bytes per object ----

if(options.suites.has('mem')){
    for(let bench of micro.memory){
        if(!wanted(bench.name)) continue
        record(bench.name, 'B', bench.run(options.runs))
        disposeAll()
    }
}

let output = { date: new Date().toISOString(), node: process.version, commit: commitOf(), runs: options.runs, metrics: results }
if(options.out) fs.writeFileSync(options.out, JSON.stringify(output, null, 1))
else process.stdout.write(JSON.stringify(output, null, 1) + '\n')
// the timers of the applications in their contexts would keep the process running
process.exit(0)
