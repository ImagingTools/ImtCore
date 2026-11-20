// Shared parts of the stress tests (npm run stress: node --expose-gc --test tests/stress).
// Every test runs on both runtimes: the sources of Runtime/ (bundled as dist/core.js is) and dist/core.js. A compiled QML
// application runs in a context of its own; its errors and warnings are collected.
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compileQml, load, CORE } = require('../bench/lib')
const { sourcesBundle } = require('../sourcesBundle')

let sourcesCore = null
function cores(){
    if(!sourcesCore){
        sourcesCore = path.join(fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-stress-')), 'sources.js')
        fs.writeFileSync(sourcesCore, sourcesBundle())
    }
    return [['Runtime', sourcesCore], ['dist/core.js', CORE]]
}

const compiled = new Map()
// text: Main.qml, or { file: text }; returns { JQ, root, frame, errors: [...], warnings: [...] }
function run(text, core, options = {}){
    let key = JSON.stringify(text)
    if(!compiled.has(key)) compiled.set(key, compileQml(typeof text === 'string' ? { 'Main.qml': text } : text, { allowWarnings: options.allowWarnings }))
    let errors = [], warnings = []
    let console = { log(){}, info(){}, debug(){}, warn: (...a) => warnings.push(a.join(' ')), error: (...a) => errors.push(a.join(' ')) }
    let result = load(compiled.get(key), { core, console, manualFrames: options.manualFrames })
    return { ...result, errors, warnings }
}

// a deterministic random generator (mulberry32)
function random(seed){
    let a = seed >>> 0
    return () => {
        a = (a + 0x6D2B79F5) >>> 0
        let t = a
        t = Math.imul(t ^ (t >>> 15), t | 1)
        t ^= t + Math.imul(t ^ (t >>> 7), t | 61)
        return ((t ^ (t >>> 14)) >>> 0) / 4294967296
    }
}

function gc(){
    if(typeof global.gc !== 'function') throw new Error('run with node --expose-gc (npm run stress)')
    global.gc()
}

// whether the objects behind the WeakRefs were collected: the garbage collector runs a few times, with turns of the
// event loop between (a WeakRef target is kept until the end of the turn that made it)
async function collected(refs){
    for(let i = 0; i < 10; i++){
        await new Promise((resolve) => setImmediate(resolve))
        gc()
        if(refs.every(ref => ref.deref() === undefined)) return true
    }
    return refs.filter(ref => ref.deref() !== undefined).length === 0
}

function heap(){
    gc()
    gc()
    return process.memoryUsage().heapUsed
}

// the number of subscribers of every signal of obj
function linkCount(obj){
    let subs = obj.$subs || [], n = 0
    for(let i = 1; i < subs.length; i += 2) for(let link = subs[i]; link; link = link.nextSub) n++
    return n
}

module.exports = { cores, run, random, gc, collected, heap, linkCount }
