// QML compiler in the browser, for Qt.createQmlObject: one document compiled against the types the bundle holds.
// The compiled QML classes of the bundle are known by their metadata ($type, $qmlAliases, methods on the prototype),
// like the runtime types; the document is created in the calling context, so the ids of the caller resolve through
// $s.$p as in a nested component.
const parser = require('./parser')
const { FileModel, Scope, Element } = require('./model')
const { Codegen, TypeRef } = require('./codegen')
const { checkFile } = require('./checks')
const JQ = require('../Runtime')
const { qtModule } = require('../Runtime/Qt/modules')

function dirOf(key){
    let at = key.lastIndexOf('/')
    return at < 0 ? '' : key.slice(0, at)
}

function normalize(path){
    let parts = []
    for(let part of path.split('/')){
        if(part === '..') parts.pop()
        else if(part && part !== '.') parts.push(part)
    }
    return parts.join('/')
}

function load(T){
    return T && T.$lazy ? T() : T
}

class RuntimeCodegen extends Codegen {
    constructor(file, env){
        let files = new Map([[file.url, file]])
        files.$modules = new Map()
        file.$env = env
        file.$errors = []
        super(files, file)
        this.env = env
        this.refs = []              // the classes the generated code names as $T[i]
        this.refByType = new Map()
    }

    get errors(){ return this.entry.$errors }

    // the errors of Codegen.report and compiler/checks.js fail the document
    // follow: an error Qt reports right after this one
    report(file, phase, at, message, warning, follow){
        this.entry.$errors.push({ phase, line: at ? at.line : 0, col: at ? at.col : 0, message, follow })
    }

    // QQmlImports: a module import must exist; a directory import is relative to the document
    resolveImports(file){
        let env = file.$env
        file.typeImports = []
        file.namespaces = new Map()
        for(let imp of file.imports){
            let target = String(imp.path || '').replace(/^["']|["']$/g, '')
            let source = null
            if(imp.isModule){
                if(qtModule(target)) source = { qt: target }
                else if(Object.prototype.hasOwnProperty.call(env.modules, target)) source = { module: target }
                else file.$errors.push({ phase: 0, line: imp.line || 1, col: imp.col || 1, message: 'module "' + target + '" is not installed' })
            } else if(target.endsWith('.js')){
                // a script of the bundle (compiler/scripts.js registers it under its url)
                let key = normalize((file.dir ? file.dir + '/' : '') + target)
                if(Object.prototype.hasOwnProperty.call(env.scripts, key)){
                    if(imp.as) file.scriptImports.set(imp.as, 'JQ.scripts[' + JSON.stringify(key) + ']')
                } else {
                    file.$errors.push({ phase: 0, line: imp.line, col: imp.col, message: 'Script ' + target + ' unavailable' })
                }
                continue
            } else {
                let dir = normalize((file.dir ? file.dir + '/' : '') + target)
                // the bundle stands for the files: a directory of none of its files does not exist
                if(dir && !Object.keys(env.files).some(key => key.startsWith(dir + '/'))){
                    file.$errors.push({ phase: 0, line: imp.line, col: imp.col, message: '"' + target + '": no such directory' })
                }
                source = { dir }
            }
            if(imp.as) file.namespaces.set(imp.as, source)
            else if(source) file.typeImports.unshift(source)
        }
    }

    importedType(source, name){
        if(source.qt){
            let module = qtModule(source.qt)
            return module.types.has(name) ? this.types.get(name === 'ListElement' ? 'QtObject' : name) || null : null
        }
        if(source.module){
            let types = this.env.modules[source.module]
            return types && Object.prototype.hasOwnProperty.call(types, name) ? this.runtimeRef(name, types[name]) : null
        }
        let type = this.env.files[(source.dir ? source.dir + '/' : '') + name + '.qml']
        return type ? this.runtimeRef(name, type) : null
    }

    // a QML type compiled into the bundle: named $T[i] by the generated code
    runtimeRef(name, type){
        let Class = load(type)
        if(this.refByType.has(Class)) return this.refByType.get(Class)
        let ref = new TypeRef(name, 'builtin', Class, null, '$T[' + this.refs.length + ']')
        ref.compiledQml = true
        this.refs.push(Class)
        this.refByType.set(Class, ref)
        return ref
    }

    // Like QQmlImports, without the old compiler's lookup by name: an unknown type is an error
    typeOf(name, file = null, qualifier = ''){
        if(qualifier){
            let source = file.namespaces.get(qualifier)
            return source ? this.importedType(source, name) : null
        }
        if(this.types.has(name)) return this.types.get(name)
        if(!file) return null
        for(let source of file.typeImports){
            let found = this.importedType(source, name)
            if(found) return found
        }
        return this.importedType({ dir: file.dir }, name)
    }

    // the code of the document: emitFile of the build, as a function body that returns the class
    emitDocument(file){
        this.emitFile(file, true)
        return this.sharedFunctions(this.out.join('\n'))
    }
}

// The scopes of the calling context, innermost first: ids by name, and the class of each scope root for its members
function callerScopes(codegen, scope, info){
    let scopes = []
    for(let level = info, array = scope; level; level = level.parent, array = array ? array.$p : null){
        let synthetic = new Scope(null, !!level.delegate)
        synthetic.index = 1000 + scopes.length
        for(let name of Object.keys(level.ids)) synthetic.ids.set(name, level.ids[name])
        let rootObject = array ? array[0] : null
        if(rootObject && rootObject.constructor && rootObject.constructor.$type){
            let root = new Element(rootObject.constructor.name, 0, synthetic, null)
            root.typeRef = codegen.runtimeRef(rootObject.constructor.name, rootObject.constructor)
            synthetic.root = root
        }
        if(scopes.length) scopes[scopes.length - 1].parent = synthetic
        scopes.push(synthetic)
    }
    return scopes
}

// The message of the QQmlJS parser for an error of the QmlWeb parser, checked against Qt 6.8 for the usual mistakes;
// the position is the parser's own
function parseMessage(error, source){
    let text = String(error.message || error).split('\n')[0].replace(/\s*\(line: .*$/, '').trim()
    if(/^Expected token /.test(text)) return text
    if(/^Unexpected token: eof\b/.test(text)) return "Expected token `}'"
    let expected = /^Unexpected token .*, expected punc (\S+)$/.exec(text)
    // an unclosed parenthesis or array: QQmlJS waits for the next element
    if(expected) return 'Expected token `' + (expected[1] === ')' || expected[1] === ']' ? ',' : expected[1]) + "'"
    let pos = typeof error.pos === 'number' ? error.pos - 1 : -1
    let before = pos >= 0 ? source.slice(0, pos).replace(/\s+$/, '') : ''
    let last = before.slice(-1)
    if(/^Unexpected token: punc \(\}\)$/.test(text)){
        if(/[-+*\/%&|^<>=!~?,]/.test(last)) return "Expected token `numeric literal'"
        if(last === ':') return "Expected token `;'"
        return "Unexpected token `}'"
    }
    return 'Syntax error'
}

// Compiled documents per calling context (the ids of a scope of the bundle are a const of its file), by url and text;
// the classes of the caller's scope roots are checked, because the members of the context depend on them. A context
// keeps the last CACHE_SIZE documents, so a loop of varying texts does not grow the memory.
const CACHE_SIZE = 64
const cache = new WeakMap()
const NO_CONTEXT = {}

function scopeRoots(scope){
    let roots = []
    for(let array = scope; array; array = array.$p) roots.push(array[0] && array[0].constructor)
    return roots
}

function compile(source, url, key, env, scope, info){
    let context = info || NO_CONTEXT
    let documents = cache.get(context)
    if(!documents) cache.set(context, documents = new Map())
    let text = url + '\n' + key + '\n' + source
    let roots = info ? scopeRoots(scope) : []
    let entry = documents.get(text)
    if(entry && entry.roots.length === roots.length && entry.roots.every((root, i) => root === roots[i])){
        documents.delete(text)
        documents.set(text, entry)
        return entry.result
    }
    let result = compileDocument(source, url, key, env, scope, info)
    documents.set(text, { roots, result })
    if(documents.size > CACHE_SIZE) documents.delete(documents.keys().next().value)
    return result
}

// Compiles a document; { Class } or { errors: [{ line, col, message }] }
function compileDocument(source, url, key, env, scope, info){
    let ast
    try {
        parser.parse.nowParsingFile = url
        ast = parser.parse(source)
    } catch(error){
        let line = typeof error.line === 'number' ? error.line : 0
        let col = typeof error.col === 'number' ? error.col : 0
        return { errors: [{ line, col, message: parseMessage(error, source) }] }
    }
    // imports and no object: QQmlJS stops at the end of the text
    if(!ast[2]){
        let lines = source.split('\n')
        return { errors: [{ line: lines.length, col: lines[lines.length - 1].length + 1, message: "Expected token `numeric literal'" }] }
    }
    // every document is compiled in a function of its own: one class name for all
    let name = 'QmlObject'
    let file = new FileModel(name, url).build(ast)
    file.url = key || name + '.qml'
    file.dir = dirOf(key || '')
    file.createsComponents = source.includes('createComponent')
    file.createsQmlObjects = source.includes('createQmlObject')
    let codegen = new RuntimeCodegen(file, env)
    if(info){
        let scopes = callerScopes(codegen, scope, info)
        if(scopes.length) file.scopes[0].parent = scopes[0]
    }
    if(!file.root) return { errors: [{ line: 1, col: 1, message: 'Syntax error' }] }
    codegen.resolveTypes(file)
    codegen.expandElementValues(file)
    codegen.prepareConnections(file)
    checkFile(codegen, file, true)
    let code = codegen.emitDocument(file)
    // Qt stops at the first error of its earliest stage
    if(codegen.errors.length){
        let first = codegen.errors.slice().sort((a, b) => a.phase - b.phase || a.line - b.line || a.col - b.col)[0]
        return { errors: first.follow ? [first, first.follow] : [first] }
    }
    let refs = codegen.refs
    let Class = new Function('JQ', '$T', code)(JQ, refs)
    return { Class }
}

JQ.Qt.$qmlCompiler = { compile }

module.exports = { compile }
