// The sources of Runtime/ as one script, like dist/core.js (window.JQ): the stress tests and the benchmarks run a
// compiled application on either runtime. Plain CommonJS: every module reachable from Runtime/index.js by require.
const fs = require('fs')
const path = require('path')

const RUNTIME = path.resolve(__dirname, '../Runtime')
let cached = null

function sourcesBundle(){
    if(cached) return cached
    let ids = new Map()
    let modules = []
    let visit = (file) => {
        if(ids.has(file)) return ids.get(file)
        let id = modules.length
        ids.set(file, id)
        modules.push(null)
        let source = fs.readFileSync(file, 'utf8')
        let body = source.replace(/require\((['"])(\.{1,2}\/[^'"]+)\1\)/g, (match, quote, request) => {
            let target = path.resolve(path.dirname(file), request)
            if(!target.endsWith('.js')) target = fs.existsSync(target + '.js') ? target + '.js' : path.join(target, 'index.js')
            return '__req(' + visit(target) + ')'
        })
        modules[id] = 'function(module, exports, __req){\n' + body + '\n}'
        return id
    }
    visit(path.join(RUNTIME, 'index.js'))
    cached = '(function(){\nvar __defs = [' + modules.join(',\n') + '];\nvar __cache = [];\n' +
        'function __req(id){ if(__cache[id]) return __cache[id].exports; var m = __cache[id] = { exports: {} }; __defs[id](m, m.exports, __req); return m.exports }\n' +
        '__req(0)\n})();\n'
    return cached
}

module.exports = { sourcesBundle }
