// Builds dist/core.js, the runtime the compiler puts first in every bundle, from the sources of Runtime/:
//   node compiler/buildcore.js        (npm run build)
// The modules bundled (compiler/runtimeBundle.js), minified by uglify-js (compress, mangle). The same sources give the
// same file. Tools/JQML/v5 is where v5 is developed: change Runtime/, build, commit dist/core.js with it.
const fs = require('fs')
const path = require('path')
const uglify = require('uglify-js')
const { sourcesBundle } = require('./runtimeBundle')

function buildCore(){
    let result = uglify.minify(sourcesBundle(), { compress: { passes: 2 }, mangle: true })
    if(result.error) throw result.error
    return result.code + '\n'
}

if(require.main === module){
    let file = path.resolve(__dirname, '../dist/core.js')
    let code = buildCore()
    fs.writeFileSync(file, code)
    console.log('dist/core.js: ' + code.length + ' bytes')
}

module.exports = { buildCore }
