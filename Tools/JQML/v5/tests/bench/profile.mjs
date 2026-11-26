// node tests/bench/profile.mjs <file.cpuprofile> [top=40]
// The functions of a CPU profile (node --cpu-prof) by self time, with their share and their place in the code.
// Example: node --expose-gc --cpu-prof --cpu-prof-dir=/tmp/prof tests/bench/run.js --suite docs --filter ProLife --runs 2
import fs from 'fs'

let [file, top = 40] = process.argv.slice(2)
let profile = JSON.parse(fs.readFileSync(file, 'utf8'))
let byId = new Map(profile.nodes.map(node => [node.id, node]))
let self = new Map()
let deltas = profile.timeDeltas
for(let i = 0; i < profile.samples.length; i++){
    let node = byId.get(profile.samples[i])
    let frame = node.callFrame
    let key = (frame.functionName || '(anonymous)') + ' ' + (frame.url || '').replace(/^.*\//, '') + ':' + (frame.lineNumber + 1) + ':' + (frame.columnNumber + 1)
    self.set(key, (self.get(key) || 0) + (deltas[i] || 0))
}
let total = [...self.values()].reduce((s, x) => s + x, 0)
let rows = [...self.entries()].sort((a, b) => b[1] - a[1]).slice(0, +top)
console.log('total ' + (total / 1000).toFixed(0) + ' ms')
for(let [key, us] of rows) console.log(((us / total) * 100).toFixed(1).padStart(5) + '%  ' + (us / 1000).toFixed(1).padStart(9) + ' ms  ' + key)
