// node tests/bench/noise.mjs run1.json run2.json run3.json > noise.json
// Runs of the same code: the noise threshold of a metric is the greatest of 2% of its median and 2 sd, the sd of the
// medians between the runs or within a run.
import fs from 'fs'

let runs = process.argv.slice(2).map(f => JSON.parse(fs.readFileSync(f, 'utf8')))
let thresholds = {}, spread = {}
for(let name of Object.keys(runs[0].metrics)){
    let medians = runs.map(r => r.metrics[name] && r.metrics[name].median).filter(x => x !== undefined)
    let mean = medians.reduce((s, x) => s + x, 0) / medians.length
    let between = Math.sqrt(medians.reduce((s, x) => s + (x - mean) * (x - mean), 0) / Math.max(1, medians.length - 1))
    let within = Math.max(...runs.map(r => r.metrics[name] ? r.metrics[name].sd : 0))
    thresholds[name] = Math.max(Math.abs(mean) * 0.02, 2 * between, 2 * within)
    spread[name] = { medians, between, within, thresholdPct: mean ? thresholds[name] / Math.abs(mean) * 100 : 0 }
}
process.stdout.write(JSON.stringify({ runs: runs.length, thresholds, spread }, null, 1) + '\n')
