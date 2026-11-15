// JavaScript resources: qmldir "<Qualifier> <version> <file>.js" and `import "file.js" as Qualifier`.
// "The script's top-level declarations are available as properties of the qualifier" (Qt docs).
// Qt gives every importing component instance its own copy unless the script says `.pragma library`;
// one shared instance is made here: the ImtCore resources hold no mutable state.
const fs = require('fs')
const U = require('uglify-js')

function topLevelNames(source, fileName){
    let ast
    try {
        ast = U.parse(source)
    } catch(error) {
        throw new Error(fileName + ': ' + error.message)
    }
    let names = []
    for(let node of ast.body){
        if(node instanceof U.AST_Defun || node instanceof U.AST_DefClass){
            if(node.name) names.push(node.name.name)
        } else if(node instanceof U.AST_Definitions){
            for(let def of node.definitions){
                if(def.name instanceof U.AST_SymbolDeclaration) names.push(def.name.name)
            }
        }
    }
    return names
}

function loadScript(fileName){
    // .pragma / .import lines are QML directives, not JavaScript
    let source = fs.readFileSync(fileName, 'utf8').replace(/^﻿/, '').replace(/^[ \t]*\.(pragma|import)\b.*$/gm, '')
    return { source, names: topLevelNames(source, fileName) }
}

function emitScript(ref, fileName){
    let script = loadScript(fileName)
    let exports = script.names.map(name => JSON.stringify(name) + ':' + name).join(',')
    return 'const ' + ref + '=(function(Qt){\n' + script.source + '\nreturn {' + exports + '}})(JQ.Qt);'
}

module.exports = { loadScript, emitScript }
