// JavaScript resources: qmldir "<Qualifier> <version> <file>.js", `import "file.js" as Qualifier` and, in a script,
// `.import "file.js" as Qualifier` / `.import Module as Qualifier`.
// Like Qt: a script without `.pragma library` gets an instance per importing document instance and sees the
// context of that document (its ids and the properties of its root); a library script is one instance without
// a context. The qualifier shows the script's top-level names with their current values.
const fs = require('fs')
const U = require('uglify-js')

// A script whose top level only declares (literals, functions, its own names, JavaScript globals) does nothing
// observable when it is made: made on its first use, it cannot be told from one made with the document
function isPure(ast, isGlobal){
    const pure = (node) => {
        if(!node) return true
        if(node instanceof U.AST_Constant || node instanceof U.AST_Lambda) return true
        if(node instanceof U.AST_SymbolRef) return !(node.thedef && node.thedef.undeclared) || isGlobal(node.name)
        if(node instanceof U.AST_Array) return node.elements.every(pure)
        if(node instanceof U.AST_Object){
            return node.properties.every(p => (p instanceof U.AST_ObjectGetter || p instanceof U.AST_ObjectSetter)
                || (p instanceof U.AST_ObjectKeyVal && typeof p.key === 'string' && pure(p.value)))
        }
        if(node instanceof U.AST_UnaryPrefix) return node.operator !== 'delete' && node.operator !== '++' && node.operator !== '--' && pure(node.expression)
        if(node instanceof U.AST_Binary) return pure(node.left) && pure(node.right)
        if(node instanceof U.AST_Conditional) return pure(node.condition) && pure(node.consequent) && pure(node.alternative)
        if(node instanceof U.AST_Dot) return pure(node.expression)
        if(node instanceof U.AST_Template) return !node.tag && node.expressions.every(pure)
        return false
    }
    ast.figure_out_scope()
    return ast.body.every(node => node instanceof U.AST_Defun || node instanceof U.AST_EmptyStatement || node instanceof U.AST_Directive
        || (node instanceof U.AST_Definitions && node.definitions.every(def => pure(def.value))))
}

function topLevelNames(ast){
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

const IMPORT = /^[ \t]*\.import[ \t]+("[^"\n]*"|'[^'\n]*'|[\w.]+)(?:[ \t]+\d+(?:\.\d+)?)?[ \t]+as[ \t]+([A-Za-z_$][\w$]*)[ \t]*;?[ \t]*$/

function loadScript(fileName){
    let raw = fs.readFileSync(fileName, 'utf8').replace(/^﻿/, '')
    let library = false
    let imports = []
    // .pragma / .import lines are QML directives, not JavaScript: blank lines keep the line numbers
    let source = raw.replace(/^[ \t]*\.(pragma|import)\b.*$/gm, (line, kind) => {
        if(kind === 'pragma' && /\blibrary\b/.test(line)) library = true
        let m = kind === 'import' ? IMPORT.exec(line) : null
        if(m) imports.push({ target: m[1].replace(/^["']|["']$/g, ''), file: /^["']/.test(m[1]), as: m[2] })
        return ''
    })
    let ast
    try {
        ast = U.parse(source)
    } catch(error) {
        throw new Error(fileName + ': ' + error.message)
    }
    return { source, library, imports, names: topLevelNames(ast), ast }
}

function expression(source){
    return U.parse('(' + source + ')').body[0].body
}

// a free name of a script with a context: not declared in it, not a JavaScript global
function contextName(node, env){
    return node instanceof U.AST_SymbolRef && node.thedef && node.thedef.undeclared && !env.isGlobal(node.name) && node.name !== 'JQ'
}

// env.isGlobal(name): a JavaScript global; env.importRef(import, fileName): the JS source of an import's script
// reference or module namespace
function emitScript(ref, fileName, url, env){
    let script = loadScript(fileName)
    let params = ['Qt', '$c'].concat(script.imports.map(imp => imp.as))
    let accessors = script.names.map(name => 'get ' + name + '(){return ' + name + '},set ' + name + '($v){' + name + '=$v}').join(',')
    let ast = U.parse('(function(' + params.join(',') + '){' + script.source + '\nreturn{' + accessors + '}})')
    ast.figure_out_scope()
    if(!script.library){
        // a free name is looked up in the importing context, then in the global object (JQ.sctx)
        ast = ast.transform(new U.TreeTransformer(function(node){
            if(node instanceof U.AST_Assign && contextName(node.left, env)){
                node.right = node.right.transform(this)
                if(node.operator !== '=') return node
                return new U.AST_Call({ expression: expression('JQ.sset'), args: [expression('$c'), new U.AST_String({ value: node.left.name }), node.right] })
            }
            if(node instanceof U.AST_Unary && (node.operator === '++' || node.operator === '--') && contextName(node.expression, env)) return node
            if(node instanceof U.AST_UnaryPrefix && node.operator === 'typeof' && contextName(node.expression, env)){
                return expression('typeof JQ.sctxValue($c,' + JSON.stringify(node.expression.name) + ')')
            }
            if(contextName(node, env)) return expression('JQ.sctx($c,' + JSON.stringify(node.name) + ')')
            return undefined
        }))
    }
    let factory = ast.print_to_string({ beautify: false, comments: false, ascii_only: false }).replace(/;$/, '')
    let imports = '[' + script.imports.map(imp => env.importRef(imp, fileName)).join(',') + ']'
    let key = JSON.stringify(url || '')
    let pure = isPure(script.ast, env.isGlobal)
    return 'const ' + ref + '=JQ.script(' + key + ',' + script.library + ',' + factory + ',()=>' + imports + (pure ? ',true' : '') + ');JQ.scripts[' + key + ']=' + ref + ';'
}

module.exports = { loadScript, emitScript, isPure }
