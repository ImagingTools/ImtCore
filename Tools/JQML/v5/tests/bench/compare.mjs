// node tests/bench/compare.mjs base.json new.json [--noise noise.json] [--accept target-regex]
// The change of every metric (all of them: lower is better), against its noise threshold: the greatest of 2% of the
// base, 2 sd of either run and the threshold measured by noise.mjs. Verdict: "regression" when one metric is worse
// past its threshold; with --accept, "accepted" when a metric of the target improved past its threshold and by at
// least 3%, and none got worse.
import fs from 'fs'

let argv = process.argv.slice(2)
let files = argv.filter((a, i) => !a.startsWith('--') && !(argv[i - 1] || '').startsWith('--'))
let option = (name) => { let i = argv.indexOf('--' + name); return i >= 0 ? argv[i + 1] : null }
let [base, next] = files.map(f => JSON.parse(fs.readFileSync(f, 'utf8')))
let noise = option('noise') ? JSON.parse(fs.readFileSync(option('noise'), 'utf8')).thresholds : {}
let target = option('accept') ? new RegExp(option('accept')) : null

let rows = [], worse = [], better = []
for(let name of Object.keys(base.metrics)){
    let a = base.metrics[name], b = next.metrics[name]
    if(!b) continue
    let delta = b.median - a.median
    let pct = a.median ? delta / a.median * 100 : (delta ? Infinity : 0)
    let threshold = Math.max(Math.abs(a.median) * 0.02, 2 * a.sd, 2 * b.sd, noise[name] || 0)
    let verdict = Math.abs(delta) <= threshold ? 'noise' : delta < 0 ? 'better' : 'WORSE'
    if(verdict === 'WORSE') worse.push(name)
    if(verdict === 'better' && pct <= -3) better.push(name)
    rows.push([name, a.median, b.median, pct, threshold, verdict, a.unit])
}
for(let name of Object.keys(next.metrics)) if(!base.metrics[name]) rows.push([name, NaN, next.metrics[name].median, NaN, NaN, 'new', next.metrics[name].unit])

let fmt = (x, unit) => Number.isNaN(x) ? '-' : unit === 'ms' ? x.toFixed(3) : Math.round(x).toString()
console.log('metric'.padEnd(46) + 'base'.padStart(14) + 'new'.padStart(14) + 'Δ%'.padStart(9) + 'noise'.padStart(12) + '  verdict')
for(let [name, a, b, pct, threshold, verdict, unit] of rows){
    console.log(name.padEnd(46) + fmt(a, unit).padStart(14) + fmt(b, unit).padStart(14) + (Number.isFinite(pct) ? (pct > 0 ? '+' : '') + pct.toFixed(1) : '-').padStart(9) + fmt(threshold, unit).padStart(12) + '  ' + verdict)
}
let verdict = worse.length ? 'regression: ' + worse.join(', ')
    : target ? (better.some(n => target.test(n)) ? 'accepted: ' + better.filter(n => target.test(n)).join(', ') : 'neutral: the target did not improve past the noise and 3%')
    : better.length ? 'better: ' + better.join(', ') : 'neutral'
console.log('\nverdict: ' + verdict)
process.exitCode = worse.length ? 1 : 0
