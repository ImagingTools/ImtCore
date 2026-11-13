// uglify-js in the browser bundle of compiler/runtime.js: the lib files the compiler needs (parse, scope, transform,
// print) evaluated in one scope, as the package's tools/node.js does; the minifier is left out.
const FILES = [
    require('uglify-js/lib/utils.js'),
    require('uglify-js/lib/ast.js'),
    require('uglify-js/lib/transform.js'),
    require('uglify-js/lib/parse.js'),
    require('uglify-js/lib/scope.js'),
    require('uglify-js/lib/output.js'),
]

const names = ['parse', 'TreeTransformer', 'TreeWalker']
for(let match of FILES[1].matchAll(/^var (AST_\w+)\s*=/gm)) names.push(match[1])

const uglify = {}
new Function('exports', FILES.join('\n') + '\n' + names.map(name => 'exports.' + name + '=' + name + ';').join('\n'))(uglify)

module.exports = uglify
