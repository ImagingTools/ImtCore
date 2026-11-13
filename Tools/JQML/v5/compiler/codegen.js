// Code generation for the prototype core (Docs/CoreDesign.md, 4.4):
// one class per QML type, shared binding/handler functions, a build function per component body,
// ids resolved to slots of the body's scope array $s.
const JQ = require('../Runtime')
const fs = require('fs')
const path = require('path')
const { rewriteValue, rewriteHandler, rewriteMethod, expressionText } = require('./expr')
const { emitScript } = require('./scripts')

const JS_GLOBALS = new Set([
    'Math', 'JSON', 'Date', 'Number', 'String', 'Boolean', 'Array', 'Object', 'RegExp', 'Error', 'TypeError', 'RangeError',
    'Promise', 'Map', 'Set', 'WeakMap', 'WeakSet', 'Symbol', 'Infinity', 'NaN', 'undefined', 'isNaN', 'isFinite',
    'parseInt', 'parseFloat', 'encodeURIComponent', 'decodeURIComponent', 'encodeURI', 'decodeURI', 'escape', 'unescape',
    'console', 'setTimeout', 'clearTimeout', 'setInterval', 'clearInterval', 'arguments', 'eval', 'window', 'document',
    'navigator', 'location', 'localStorage', 'XMLHttpRequest', 'FileReader', 'Blob', 'URL', 'performance',
    'qsTr', 'qsTranslate', 'Uint8Array', 'Int32Array', 'Float32Array', 'Float64Array', 'ArrayBuffer', 'DataView',
    'globalThis', 'BigInt', 'Intl', 'Reflect', 'Proxy', 'alert', 'print', 'gc',
])

const QML_KIND = {
    int: 'int', real: 'real', double: 'real', bool: 'bool', string: 'string', url: 'string',
    color: 'color', var: 'var', variant: 'var', Component: 'component',
}

const IDENT = /^[A-Za-z_$][\w$]*$/

const FN_MARK = '\u0001'
const FN_MARKS = /\u0001(\d+)\u0001/g
const LOCAL_NAME = /\bT\d*_\w*\$(?!\(\))/

// list properties whose element values are assigned as an array (data / children / resources only parent them)
const LIST_VALUE = new Set(['states', 'transitions', 'targets', 'animations', 'changes'])

class TypeRef {
    constructor(name, kind, Class, file, ref){
        this.name = name
        this.kind = kind          // 'builtin' | 'qml'
        this.Class = Class
        this.file = file
        this.ref = ref
        this.base = null
    }
}

// Literal AST: compiled as a plain assignment, no binding
function isConstant(node){
    if(!node) return false
    switch(node[0]){
        case 'stat': return isConstant(node[1])
        case 'num': case 'string': case 'regexp': case 'atom': return true
        case 'name': return node[1] === 'true' || node[1] === 'false' || node[1] === 'null' || node[1] === 'undefined'
        case 'array': return node[1].every(isConstant)
        case 'object': return node[1].every(pair => pair.length < 3 && isConstant(pair[1]))
        case 'unary-prefix': case 'unary-postfix': return isConstant(node[2])
        case 'binary': return isConstant(node[2]) && isConstant(node[3])
        case 'conditional': return isConstant(node[1]) && isConstant(node[2]) && isConstant(node[3])
        default: return false
    }
}

class Codegen {
    constructor(files, entry){
        this.files = files              // url -> FileModel
        this.fnCodes = []
        this.entry = entry
        this.types = new Map()          // builtin types
        this.modules = files.$modules || new Map()
        this.byDir = new Map()          // dir -> (type -> FileModel)
        this.byName = new Map()         // type -> FileModels, for files that use a type without importing it
        this.warnings = []
        this.out = []
        this.functionCount = 0
        for(let name of Object.keys(JQ.types)){
            this.types.set(name, new TypeRef(name, 'builtin', JQ.types[name], null, 'JQ.types.' + name))
        }
        // JavaScript resources: one const per file, module qualifiers are global like module types
        this.scripts = new Map()        // .js file -> const name
        this.moduleScripts = new Map()  // qualifier -> const name
        for(let [qualifier, fileName] of files.$scripts || []) this.moduleScripts.set(qualifier, this.scriptRef(fileName))
        for(let file of files.values()){
            file.scriptImports = new Map()
            for(let imp of file.imports){
                let target = String(imp.path || '').replace(/^["']|["']$/g, '')
                if(!target.endsWith('.js')) continue
                let fileName = path.resolve(path.dirname(file.fileName), target)
                if(imp.as) file.scriptImports.set(imp.as, this.scriptRef(fileName))
            }
        }
        let refs = new Set()
        for(let file of entry ? [entry, ...files.values()] : files.values()){
            if(file.typeRef) continue
            let ref = 'T_' + file.name
            for(let n = 2; refs.has(ref); n++) ref = 'T' + n + '_' + file.name
            refs.add(ref)
            file.typeRef = new TypeRef(file.name, 'qml', null, file, ref)
            if(!this.byDir.has(file.dir)) this.byDir.set(file.dir, new Map())
            this.byDir.get(file.dir).set(file.name, file)
            if(!this.byName.has(file.name)) this.byName.set(file.name, [])
            this.byName.get(file.name).push(file)
        }
        for(let file of files.values()) this.resolveImports(file)
    }

    // file.typeImports: modules ({ module }) and dirs ({ dir }) imported without a qualifier, the last import
    // first; file.namespaces: qualifier -> one of them, null for a Qt module
    resolveImports(file){
        file.typeImports = []
        file.namespaces = new Map()
        for(let imp of file.imports){
            let target = String(imp.path || '').replace(/^["']|["']$/g, '')
            if(target.endsWith('.js')) continue
            let source = imp.isModule ? (this.modules.has(target) ? { module: target } : null) : imp.dirKey ? { dir: imp.dirKey } : null
            if(imp.as) file.namespaces.set(imp.as, source)
            else if(source) file.typeImports.unshift(source)
        }
    }

    importedType(source, name){
        let types = source.module ? this.modules.get(source.module) : this.byDir.get(source.dir)
        let found = types && types.get(name)
        return found ? found.typeRef : null
    }

    scriptRef(fileName){
        let key = path.normalize(fileName)
        if(!this.scripts.has(key)) this.scripts.set(key, '$js' + this.scripts.size)
        return this.scripts.get(key)
    }

    warn(file, line, message){
        this.warnings.push(file.fileName + ':' + line + ': warning: ' + message)
    }

    // Like QQmlImports: a qualified name looks in its module only; otherwise the builtin types (native
    // replacements win over module files of the same name), the file's imports, the last one first,
    // then its own dir, which ranks below every module import
    typeOf(name, file = null, qualifier = ''){
        if(qualifier){
            let source = file.namespaces.get(qualifier)
            return source && this.importedType(source, name) || this.types.get(name) || null
        }
        if(this.types.has(name)) return this.types.get(name)
        if(!file) return null
        for(let source of file.typeImports){
            let found = this.importedType(source, name)
            if(found) return found
        }
        let own = this.importedType({ dir: file.dir }, name)
        if(own) return own
        // not imported: Qt fails, the old compiler took the type by name
        let named = this.byName.get(name)
        if(!named) return null
        if(named.length > 1) this.warn(file, 0, name + ' is not imported and is ambiguous: ' + named.map(f => f.url).join(', ') + ', the last one is used')
        return named[named.length - 1].typeRef
    }

    // `Q.Name` of an element type: the qualifier and the name
    splitType(file, type){
        let dot = type.indexOf('.')
        return dot > 0 && file.namespaces.has(type.slice(0, dot)) ? [type.slice(0, dot), type.slice(dot + 1)] : ['', type]
    }

    // ---- members ----------------------------------------------------------------

    ownMember(el, name){
        for(let p of el.propdefs){
            if(p.name === name) return { kind: 'prop', qmlType: p.type, own: el }
            if(p.name + 'Changed' === name) return { kind: 'signal', args: [], own: el }
        }
        for(let a of el.aliases){
            if(a.name === name) return { kind: 'alias', own: el, alias: a }
            if(a.name + 'Changed' === name) return { kind: 'signal', args: [], own: el }
        }
        for(let s of el.signals) if(s.name === name) return { kind: 'signal', args: s.args, own: el }
        for(let m of el.methods) if(m.name === name) return { kind: 'method', own: el }
        return null
    }

    typeMember(typeRef, name){
        if(!typeRef) return null
        if(typeRef.kind === 'builtin'){
            let type = typeRef.Class.$type
            let prop = type.propByName.get(name)
            if(prop) return { kind: 'prop', prop, group: prop.group, builtin: true, componentKind: false }
            let signal = type.signalByName.get(name)
            if(signal !== undefined) return { kind: 'signal', args: type.signalArgs[signal] }
            // QML property names start lowercase: a capitalized name is a type (Component.Ready), not the attached getter
            if(/^[a-z_$]/.test(name) && name in typeRef.Class.prototype) return { kind: 'method' }
            return null
        }
        let root = typeRef.file.root
        return this.ownMember(root, name) || this.typeMember(root.typeRef, name)
    }

    member(el, name){
        return this.ownMember(el, name) || this.typeMember(el.typeRef, name)
    }

    isComponentProperty(el, name){
        let m = this.member(el, name)
        if(!m) return false
        if(m.kind === 'prop' && m.builtin) return m.prop.kind === 'component'
        if(m.kind === 'prop') return m.qmlType === 'Component'
        return false
    }

    classRef(el){
        return el.className || this.use(el.typeRef)
    }

    // a QML type is defined on its first use, through its accessor T$()
    use(typeRef){
        return typeRef.kind === 'qml' ? '(' + typeRef.ref + '$())' : typeRef.ref
    }

    // ---- name resolution -----------------------------------------------------------

    // memberEl: element whose members are the scope object (PropertyChanges: its target)
    resolver(el, selfExpr, file, memberEl){
        return (name) => {
            if(name === '$self' || name === '$s' || name === 'JQ') return null
            let depth = 0
            for(let scope = el.scope; scope; scope = scope.parent, depth++){
                if(scope.ids.has(name)) return '$s' + '.$p'.repeat(depth) + '[' + scope.ids.get(name) + ']'
            }
            if(name === 'parent') return selfExpr + '.parent'
            if(this.member(memberEl || el, name)) return selfExpr + '.' + name
            depth = 0
            for(let scope = el.scope; scope; scope = scope.parent, depth++){
                if(scope.isDelegate && (name === 'index' || name === 'model' || name === 'modelData')){
                    return 'JQ.mctx($s' + '.$p'.repeat(depth) + ').' + name
                }
                if(scope.root && scope.root !== el && this.member(scope.root, name)){
                    return '$s' + '.$p'.repeat(depth) + '[0].' + name
                }
            }
            if(name === 'Qt') return file.createsComponents ? file.typeRef.ref + '$qt' : 'JQ.Qt'
            if(file.scriptImports && file.scriptImports.has(name)) return file.scriptImports.get(name)
            if(file.namespaces.has(name)){
                let source = file.namespaces.get(name)
                return 'JQ.ns(' + JSON.stringify(source && source.module || '') + ')'
            }
            let type = this.typeOf(name, file)
            if(type) return (type.kind === 'qml' ? type.file.singleton : type.Class.$singleton) ? 'JQ.singleton(' + this.use(type) + ')' : this.use(type)
            if(this.moduleScripts.has(name)) return this.moduleScripts.get(name)
            if(JS_GLOBALS.has(name)) return null
            // inside a delegate an unknown name is a model role: `text: name`
            depth = 0
            for(let scope = el.scope; scope; scope = scope.parent, depth++){
                if(scope.isDelegate) return 'JQ.mctx($s' + '.$p'.repeat(depth) + ').$role(' + JSON.stringify(name) + ')'
            }
            if(name !== 'model' && name !== 'index' && name !== 'modelData' && name !== 'context') this.warn(file, el.line, name + ' is not found')
            return 'JQ.ctx($s,' + JSON.stringify(name) + ')'
        }
    }

    fn(file, el, raw, value, params, selfExpr, memberEl){
        let isBlock = value && value[0] === 'block'
        let where = file.fileName + ':' + el.line
        let { code } = rewriteValue(raw, isBlock, params, this.resolver(el, selfExpr, file, memberEl), where)
        return this.defineFn(file, code)
    }

    // The generated functions take the object and its scope, so many are the same text: emitted once when they
    // do not name anything local to their file (T_x$e1, T_x$qt, T_x$f1); only the accessor T_x$() is global
    defineFn(file, code){
        let name = file.typeRef.ref + '$f' + (++this.functionCount)
        this.fnCodes.push(code)
        this.out.push('const ' + name + '=' + FN_MARK + (this.fnCodes.length - 1) + FN_MARK + ';')
        return name
    }

    sharedFunctions(text){
        let counts = new Map()
        for(let code of this.fnCodes) counts.set(code, (counts.get(code) || 0) + 1)
        let names = new Map(), shared = []
        let body = text.replace(FN_MARKS, (m, index) => {
            let code = this.fnCodes[index]
            if(counts.get(code) < 2 || LOCAL_NAME.test(code)) return code
            if(!names.has(code)){
                names.set(code, '$F' + names.size)
                shared.push('const ' + names.get(code) + '=' + code + ';')
            }
            return names.get(code)
        })
        return shared.join('\n') + '\n' + body
    }

    // ---- types ------------------------------------------------------------------------

    resolveTypes(file){
        this.resolveNew(file, file.elements.filter(el => !el.typeRef))
        file.typeRef.base = file.root.typeRef
    }

    // Element values of properties become elements of the model before code is generated.
    expandElementValues(file){
        let queue = file.elements.slice()
        while(queue.length){
            let el = queue.shift()
            for(let assign of el.assigns){
                if(assign.elementNode && !assign.element){
                    // `delegate: Component {...}` is already a component
                    let asComponent = this.isComponentProperty(el, assign.name) && assign.elementNode[1] !== 'Component'
                    let before = file.elements.length
                    assign.element = file.elementValue(assign.elementNode, el.scope, null, asComponent)
                    assign.asComponent = asComponent
                    let created = file.elements.slice(before)
                    this.resolveNew(file, created)
                    queue.push(...created)
                }
                if(assign.elementNodes && !assign.elements){
                    let before = file.elements.length
                    assign.elements = assign.elementNodes.map(node => file.elementValue(node, el.scope, el, false))
                    let created = file.elements.slice(before)
                    this.resolveNew(file, created)
                    queue.push(...created)
                }
            }
            for(let def of el.propdefs){
                if(def.elementNode && !def.element){
                    let asComponent = def.type === 'Component' && def.elementNode[1] !== 'Component'
                    let before = file.elements.length
                    def.element = file.elementValue(def.elementNode, el.scope, null, asComponent)
                    def.asComponent = asComponent
                    let created = file.elements.slice(before)
                    this.resolveNew(file, created)
                    queue.push(...created)
                }
            }
        }
    }

    resolveNew(file, elements){
        for(let el of elements){
            let [qualifier, name] = this.splitType(file, el.type)
            el.type = name
            let type = this.typeOf(name === 'ListElement' ? 'QtObject' : name, file, qualifier)
            if(!type){
                this.warn(file, el.line, 'type ' + el.type + ' is not found, Item is used')
                type = this.typeOf('Item')
            }
            el.typeRef = type
        }
    }

    // ---- classes ----------------------------------------------------------------------

    emitClass(file, el, className, isFileType){
        el.className = className
        let base = this.use(el.typeRef)
        if(isFileType){
            this.out.push(className + '=class ' + className + ' extends ' + base + '{constructor(p){super(p);' + className + '$build(this)}}')
        } else {
            this.out.push('class ' + className + ' extends ' + base + '{}')
        }
        let props = []
        for(let p of el.propdefs){
            let kind = QML_KIND[p.type] || 'var'
            props.push(JSON.stringify(p.name) + ':' + (p.readonly ? '{kind:' + JSON.stringify(kind) + ',readonly:true}' : JSON.stringify(kind)))
        }
        let signals = el.signals.map(s => s.name + '(' + s.args.join(',') + ')')
        for(let a of el.aliases) signals.push(a.name + 'Changed')
        this.out.push('JQ.defineType(' + className + ',{props:{' + props.join(',') + '},signals:' + JSON.stringify(signals) + '});' + className + '.$scopeKey="$s_' + className + '";')

        for(let e of el.enums){
            for(let key of Object.keys(e.values || {})) this.out.push(className + '.' + key + '=' + JSON.stringify(e.values[key]) + ';')
        }

        let aliasTargets = []
        for(let a of el.aliases){
            let slot = el.scope.ids.get(a.path[0])
            if(slot === undefined){
                this.warn(file, a.line, 'alias target ' + a.path.join('.') + ' is not found')
                continue
            }
            let target = 'this.$s_' + className + '[' + slot + ']'
            if(a.path.length === 1){
                this.out.push('Object.defineProperty(' + className + '.prototype,' + JSON.stringify(a.name) + ',{get(){return ' + target + '},configurable:true});')
            } else {
                let path = a.path.slice(1)
                let owner = [target].concat(path.slice(0, -1)).join('.')
                let last = path[path.length - 1]
                this.out.push('Object.defineProperty(' + className + '.prototype,' + JSON.stringify(a.name) + ',{get(){return ' + owner + '.' + last + '},set(v){' + owner + '.' + last + '=v},configurable:true});')
                aliasTargets.push(JSON.stringify(a.name) + ':(o)=>{let t=o.$s_' + className + '[' + slot + ']' + path.slice(0, -1).map(p => '.' + p).join('') + ';return[t,t.constructor.$type.propByName.get(' + JSON.stringify(last) + ').id]}')
            }
        }
        let defaultAlias = el.defaultAlias
        if(isFileType && defaultAlias && defaultAlias.path.length === 2 && (defaultAlias.path[1] === 'data' || defaultAlias.path[1] === 'children')){
            let slot = el.scope.ids.get(defaultAlias.path[0])
            if(slot !== undefined) this.out.push(className + '.$defaultParent=(o)=>o.$s_' + className + '[' + slot + '];')
        }
        if(aliasTargets.length){
            this.out.push(className + '.$aliases=Object.assign({},' + base + '.$aliases,{' + aliasTargets.join(',') + '});')
        }

        for(let m of el.methods){
            let prologue = 'const $self=this,$s=this.$s_' + className + ';'
            let code = rewriteMethod(m.raw, prologue, this.resolver(el, '$self', file), file.fileName + ':' + m.line)
            // A QML method stays bound to its object when passed around (signal.connect(handler))
            this.out.push('JQ.defineMethod(' + className + ',' + JSON.stringify(m.name) + ',' + code + ');')
        }
    }

    // ---- members of one element ---------------------------------------------------------

    applyMembers(file, el, v, post){
        let cls = this.classRef(el)

        if(el.className && (el.methods.length || el.aliases.length)) this.current.push(v + '.$s_' + el.className + '=$s;')
        // Loader resolves a relative source against its context: the file that declares it, also for a
        // Loader that is the root of its own file
        if(file.url && el !== file.root && this.inherits(el, JQ.types.Loader)) this.current.push(v + '.$base=' + JSON.stringify(file.url) + ';')

        for(let def of el.propdefs){
            let constName = cls + '.' + def.name.toUpperCase()
            if(def.element){
                // in declaration order, like QQmlObjectCreator populating the object
                this.current.push(v + '.$write(' + constName + ',' + this.elementExpr(file, def.element, def.asComponent, post) + ');')
                continue
            }
            if(!def.value) continue
            this.assignValue(file, el, v, def.name, def.value, def.raw, constName, null, def.readonly)
        }

        for(let assign of el.assigns){
            if(assign.element){
                // through an alias the target may be a later element of this body
                let viaAlias = (this.member(el, assign.name.split('.')[0]) || {}).kind === 'alias'
                let line = this.assignTarget(file, el, v, assign.name) + '=' + this.elementExpr(file, assign.element, assign.asComponent, post) + ';'
                if(viaAlias) post.push(line)
                else this.current.push(line)
                continue
            }
            if(assign.elements){
                let vars = assign.elements.map(child => this.emitElement(file, child, v, post))
                let m = this.member(el, assign.name)
                if(m && m.kind === 'prop' && LIST_VALUE.has(assign.name)) this.current.push(v + '.' + assign.name + '=[' + vars.join(',') + '];')
                continue
            }
            this.assignProperty(file, el, v, assign)
        }

        if(el.$subAssigns){
            for(let [name, entries] of el.$subAssigns) post.push('JQ.subAssign(' + v + ',' + JSON.stringify(name) + ',[' + entries.join(',') + '],$s);')
            el.$subAssigns = null
        }

        for(let handler of el.handlers) this.emitHandler(file, el, v, handler, post)

        for(let a of el.aliases){
            if(a.path.length < 2) continue
            let slot = el.scope.ids.get(a.path[0])
            if(slot === undefined) continue
            let owner = '$s[' + slot + ']' + a.path.slice(1, -1).map(p => '.' + p).join('')
            let last = a.path[a.path.length - 1]
            post.push('JQ.forward(' + owner + ',' + owner + '.constructor.$type.signalByName.get(' + JSON.stringify(last + 'Changed') + '),' + v + ',' + cls + '.SIGNAL_' + a.name + 'Changed);')
        }
    }

    assignTarget(file, el, v, name){
        let parts = name.split('.')
        return v + parts.map(p => IDENT.test(p) ? '.' + p : '[' + JSON.stringify(p) + ']').join('')
    }

    assignProperty(file, el, v, assign){
        if(this.inherits(el, JQ.types.PropertyChanges) && !this.member(el, assign.name)){
            this.assignChange(file, el, v, assign)
            return
        }
        let parts = assign.name.split('.')
        if(parts.length === 1){
            let m = this.member(el, assign.name)
            if(!m){
                this.warn(file, assign.line, el.type + '.' + assign.name + ' is not found')
                return
            }
            if(m.kind === 'alias'){
                this.assignAlias(file, el, v, assign)
                return
            }
            let constName = this.classRef(el) + '.' + assign.name.toUpperCase()
            this.assignValue(file, el, v, assign.name, assign.value, assign.raw, constName, null)
            return
        }
        if(parts.length === 2){
            let m = this.member(el, parts[0])
            if(m && (m.kind === 'alias' || (m.kind === 'prop' && !m.group))){
                let entry = isConstant(assign.value)
                    ? '[' + JSON.stringify(parts[1]) + ',false,(' + expressionText(assign.raw) + '\n)]'
                    : '[' + JSON.stringify(parts[1]) + ',true,' + this.fn(file, el, assign.raw, assign.value, '$self,$s', '$self') + ']'
                let list = el.$subAssigns || (el.$subAssigns = new Map())
                if(!list.has(parts[0])) list.set(parts[0], [])
                list.get(parts[0]).push(entry)
                return
            }
            if(m && m.kind === 'prop' && m.group){
                let groupClass = m.group
                let groupRef = this.classRef(el) + '.$type.propByName.get(' + JSON.stringify(parts[0]) + ').group'
                if(!groupClass.$type.propByName.get(parts[1])){
                    this.warn(file, assign.line, el.type + '.' + assign.name + ' is not found')
                    return
                }
                this.assignValue(file, el, v + '.' + parts[0], parts[1], assign.value, assign.raw, groupRef + '.' + parts[1].toUpperCase(), v)
                return
            }
        }
        this.warn(file, assign.line, el.type + '.' + assign.name + ' is not supported yet')
    }

    isQmlType(typeRef){
        return !!typeRef && typeRef.kind === 'qml'
    }

    // handler onX: the signal X, even where a property X exists too (MouseArea.pressed)
    signalMember(el, name){
        let m = this.member(el, name)
        if(m && m.kind === 'signal') return m
        for(let type = el.typeRef; type; type = type.kind === 'qml' ? type.file.root.typeRef : null){
            if(type.kind === 'builtin'){
                let id = type.Class.$type.signalByName.get(name)
                return id === undefined ? null : { kind: 'signal', args: type.Class.$type.signalArgs[id] }
            }
            let own = this.ownMember(type.file.root, name)
            if(own && own.kind === 'signal') return own
        }
        return null
    }

    inherits(el, Class){
        for(let type = el.typeRef; type; type = type.kind === 'qml' ? type.file.root.typeRef : null){
            if(type.kind === 'builtin') return type.Class === Class || type.Class.prototype instanceof Class
        }
        return false
    }

    // slot expression of an id visible from el, or null
    idExpr(el, name){
        let depth = 0
        for(let scope = el.scope; scope; scope = scope.parent, depth++){
            if(scope.ids.has(name)) return { expr: '$s' + '.$p'.repeat(depth) + '[' + scope.ids.get(name) + ']', scope }
        }
        return null
    }

    elementById(file, el, name){
        let found = this.idExpr(el, name)
        if(!found) return null
        return file.elements.find(e => e.id === name && e.scope === found.scope) || null
    }

    // PropertyChanges { target: rect; width: 10; height: rect.width * 2 } or Qt 6: { rect.width: 10 }.
    // Expressions take the target as their scope object, so its members resolve unqualified.
    assignChange(file, el, v, assign){
        let parts = assign.name.split('.')
        let targetExpr = 'null', path = assign.name, targetEl = null
        let byId = parts.length > 1 ? this.idExpr(el, parts[0]) : null
        if(byId){
            targetExpr = byId.expr
            path = parts.slice(1).join('.')
            targetEl = this.elementById(file, el, parts[0])
        } else {
            let target = el.assigns.find(a => a.name === 'target' && a.value)
            let node = target && (target.value[0] === 'stat' ? target.value[1] : target.value)
            if(node && node[0] === 'name') targetEl = this.elementById(file, el, node[1])
        }
        if(isConstant(assign.value)){
            this.current.push(v + '.$change(' + targetExpr + ',' + JSON.stringify(path) + ',false,(' + expressionText(assign.raw) + '\n));')
            return
        }
        let fn = this.fn(file, el, assign.raw, assign.value, '$self,$s', '$self', targetEl)
        this.current.push(v + '.$change(' + targetExpr + ',' + JSON.stringify(path) + ',true,' + fn + ',$s);')
    }

    assignAlias(file, el, v, assign){
        if(isConstant(assign.value)){
            this.current.push(v + '.' + assign.name + '=(' + expressionText(assign.raw) + '\n);')
            return
        }
        let fn = this.fn(file, el, assign.raw, assign.value, '$self,$s', '$self')
        this.current.push('JQ.bindAlias(' + v + ',' + JSON.stringify(assign.name) + ',' + fn + ',$s);')
    }

    // target: element var or element var + '.group'; ownerVar: element var for a group (self of the expression)
    assignValue(file, el, target, name, value, raw, constName, ownerVar, initialOnly){
        let m = this.member(el, name)
        if(m && m.builtin && m.prop.kind === 'script'){
            let fn = this.fn(file, el, raw, value, '$self,$s', '$self')
            this.current.push(target + '.$write(' + constName + ',{fn:' + fn + ',scope:$s});')
            return
        }
        if(isConstant(value)){
            if(initialOnly) this.current.push(target + '.$write(' + constName + ',(' + expressionText(raw) + '\n));')
            else this.current.push(target + '.' + name + '=(' + expressionText(raw) + '\n);')
            return
        }
        let fn = this.fn(file, el, raw, value, '$self,$s', '$self')
        this.current.push(target + '.$bind(' + constName + ',' + fn + ',$s' + (ownerVar ? ',' + ownerVar : '') + ');')
    }

    emitHandler(file, el, v, handler, post){
        let name = handler.name
        let value = handler.value
        let isFunctionValue = value && (value[0] === 'function' || (value[0] === 'stat' && value[1] && (value[1][0] === 'arrow' || value[1][0] === 'function')))
        let kind = isFunctionValue ? 'function' : value && value[0] === 'block' ? 'block' : 'stat'
        let where = file.fileName + ':' + handler.line
        if(name === 'Component.onCompleted' || name === 'Component.onDestruction'){
            let code = rewriteHandler(handler.raw, kind, '$self,$s', [], this.resolver(el, '$self', file), where)
            let fnName = this.defineFn(file, code)
            let signal = name === 'Component.onCompleted' ? 'SIGNAL_completed' : 'SIGNAL_destruction'
            this.current.push(v + '.$on(JQ.QtObject.' + signal + ',' + fnName + ',$s);')
            if(name === 'Component.onCompleted') this.current.push('JQ.engine.scheduleCompleted(' + v + ');')
            return
        }
        if(name.indexOf('.') >= 0){
            // a grouped attached property of the item (Keys.onPressed): its signal, the item as the handler's object
            let [groupName, handlerName] = name.split('.')
            let m = this.member(el, groupName)
            let group = m && m.kind === 'prop' && m.group
            let signalName = handlerName.length > 2 ? handlerName[2].toLowerCase() + handlerName.slice(3) : ''
            let id = group && group.$type.signalByName.get(signalName)
            if(id == null || !/^on[A-Z]/.test(handlerName)){
                this.warn(file, handler.line, 'attached handler ' + name + ' is not supported yet')
                return
            }
            let code = rewriteHandler(handler.raw, kind, '$self,$s', group.$type.signalArgs[id] || [], this.resolver(el, '$self', file), where)
            let fnName = this.defineFn(file, code)
            this.current.push('JQ.handleGroup(' + v + '.' + groupName + ',' + id + ',' + fnName + ',$s,' + v + ');')
            return
        }
        if(this.isConnections(el)){
            let signalName = name[2].toLowerCase() + name.slice(3)
            let args = kind === 'function' ? [] : this.connectionsSignalArgs(el, signalName)
            let code = rewriteHandler(handler.raw, kind, '$self,$s', args, this.resolver(el, '$self', file), where)
            let fnName = this.defineFn(file, code)
            this.current.push(v + '.$handle(' + JSON.stringify(signalName) + ',' + fnName + ',$s' + (handler.declaredFunction ? ',true' : '') + ');')
            return
        }
        let signalName = name[2].toLowerCase() + name.slice(3)
        let m = this.signalMember(el, signalName)
        if(!m || m.kind !== 'signal'){
            this.warn(file, handler.line, el.type + ' has no signal ' + signalName)
            return
        }
        let code = rewriteHandler(handler.raw, kind, '$self,$s', m.args || [], this.resolver(el, '$self', file), where)
        let fnName = this.defineFn(file, code)
        this.current.push(v + '.$on(' + this.classRef(el) + '.SIGNAL_' + signalName + ',' + fnName + ',$s);')
    }

    isConnections(el){
        for(let type = el.typeRef; type; type = type.kind === 'qml' ? type.file.root.typeRef : null){
            if(type.kind === 'builtin') return type.Class === JQ.types.Connections || type.Class.prototype instanceof JQ.types.Connections
        }
        return false
    }

    // Names of the signal arguments for `onFoo:` handlers: from the target known at compile time
    // (`target: someId` or the parent), otherwise none
    connectionsSignalArgs(el, signalName){
        let target = el.parent
        let assign = el.assigns.find(a => a.name === 'target')
        if(assign){
            target = null
            let raw = (assign.raw || '').trim()
            for(let scope = el.scope; scope && !target; scope = scope.parent){
                if(scope.ids.has(raw)) target = el.scope && this.elementById(scope, raw)
            }
            if(raw === 'parent') target = el.parent
        }
        if(!target) return []
        let m = this.member(target, signalName)
        return m && m.kind === 'signal' ? m.args || [] : []
    }

    elementById(scope, id){
        for(let file of this.files.values()){
            for(let el of file.elements) if(el.scope === scope && el.id === id) return el
        }
        return null
    }

    // `function onFoo(args) {...}` in Connections is a handler; any `onFoo:` handler makes them ignored (Qt docs)
    prepareConnections(file){
        for(let el of file.elements){
            if(!this.isConnections(el)) continue
            let functions = el.methods.filter(m => /^on[A-Z]/.test(m.name))
            el.methods = el.methods.filter(m => !/^on[A-Z]/.test(m.name))
            if(el.handlers.length) continue
            for(let m of functions) el.handlers.push({ name: m.name, value: ['function'], raw: m.raw.replace(/^function\s+[\w$]+\s*\(/, 'function('), line: m.line, declaredFunction: true })
        }
    }

    // ---- elements and component bodies -----------------------------------------------------

    elementClass(file, el){
        if(!el.className && el.hasMembers()){
            this.emitClass(file, el, file.typeRef.ref + '$e' + el.index, false)
        }
        if(el.type === 'Component' && el.componentBody) this.prepareBody(file, el.componentBody)
    }

    prepareBody(file, root){
        if(root.$prepared) return
        root.$prepared = true
        for(let el of file.elements){
            if(el.scope === root.scope) this.elementClass(file, el)
        }
    }

    // Statements creating el under parentVar, appended to this.current; returns the variable
    // Like QQmlObjectCreator: the object is built with its handlers and children first and then
    // added to its parent, so its onParentChanged sees the parent being set
    emitElement(file, el, parentVar, post){
        let v = 'e' + el.index
        this.current.push('const ' + v + '=new ' + this.classRef(el) + '(null);')
        if(this.isQmlType(el.typeRef)) this.current.push(v + '.$cctx=$s;')
        if(el.slot >= 0) this.current.push('$s[' + el.slot + ']=' + v + ';')
        if(el.type === 'Component' && el.componentBody){
            this.current.push(v + '.$build=($parent,$ctx)=>' + this.bodyFunction(file, el.componentBody) + '($parent,$s,$ctx);')
        }
        this.applyMembers(file, el, v, post)
        this.emitChildren(file, el, v, post, this.classRef(el))
        if(parentVar !== 'null') this.current.push(v + '.$setParent(' + parentVar + ');')
        return v
    }

    // typeRef: the class whose default property receives the children
    emitChildren(file, el, v, post, classRef){
        let children = el.children
        if(!children.length) return
        if(el.typeRef.name === 'ListModel' || (el.typeRef.base && el.typeRef.base.name === 'ListModel')){
            if(children.every(child => child.type === 'ListElement')){
                this.current.push(v + '.$init([' + children.map(child => this.listElement(file, child)).join(',') + ']);')
                return
            }
        }
        // A default property holding a component (Repeater.delegate): the child is the delegate
        let defaultProperty = el.typeRef.kind === 'builtin' && el.typeRef.Class.$defaultProperty
        if(defaultProperty && children.length === 1 && this.isComponentProperty(el, defaultProperty)){
            let child = children[0]
            if(!child.$asDelegate){
                child.$asDelegate = true
                file.reparentToScope(child, file.newScope(el.scope, true))
            }
            post.push(v + '.' + defaultProperty + '=' + this.elementExpr(file, child, true, post) + ';')
            return
        }
        let parent = v
        if(this.hasDefaultParent(el.typeRef)){
            parent = v + '$d'
            this.current.push('const ' + parent + '=JQ.defaultParent(' + classRef + ',' + v + ');')
        }
        for(let child of children){
            if(child.onProp){
                let cv = this.emitElement(file, child, v, post)
                this.current.push(cv + '.$setTarget(' + v + ',' + JSON.stringify(child.onProp) + ');')
                continue
            }
            this.emitElement(file, child, parent, post)
        }
    }

    // A QML type in the chain declares `default property alias x: id.data`
    hasDefaultParent(typeRef){
        for(let type = typeRef; type; type = type.kind === 'qml' ? type.file.root.typeRef : null){
            if(type.kind === 'builtin') return !!type.Class.$defaultParent
            let alias = type.file.root.defaultAlias
            if(alias && alias.path.length === 2 && (alias.path[1] === 'data' || alias.path[1] === 'children')) return true
        }
        return false
    }

    // ListElement { name: "a"; items: [ ListElement {...} ] } -> {"name":("a"),"items":[...]}
    listElement(file, el){
        let fields = []
        for(let assign of el.assigns){
            if(assign.elementNodes){
                let nested = assign.elementNodes.map(node => this.listElement(file, file.elementValue(node, el.scope, null, false)))
                fields.push(JSON.stringify(assign.name) + ':[' + nested.join(',') + ']')
            } else if(assign.raw !== undefined){
                fields.push(JSON.stringify(assign.name) + ':(' + expressionText(assign.raw) + '\n)')
            }
        }
        return '{' + fields.join(',') + '}'
    }

    // Expression for an element value of a property: a component bound to this scope, or an object
    // created in place (its ids belong to the owner's scope; Qt gives it no visual parent)
    elementExpr(file, el, asComponent, post){
        if(asComponent){
            let body = this.bodyFunction(file, el)
            return 'new JQ.types.Component(null,($parent,$ctx)=>' + body + '($parent,$s,$ctx))'
        }
        this.elementClass(file, el)
        return this.emitElement(file, el, 'null', post)
    }

    // A function building the component body whose root is el, in a scope of its own
    bodyFunction(file, el){
        if(el.$bodyFunction) return el.$bodyFunction
        this.prepareBody(file, el)
        let name = file.typeRef.ref + '$b' + el.index
        el.$bodyFunction = name
        let saved = this.current
        this.current = []
        let post = []
        let size = el.scope.slots
        this.current.push('const $s=new Array(' + size + ');$s.$p=$ps;$s.$ctx=$ctx||null;')
        this.emitElement(file, el, '$parent', post)
        this.current.push('e' + el.index + '.$own=$s;')
        let body = this.current.concat(post)
        this.current = saved
        this.out.push('function ' + name + '($parent,$ps,$ctx){' + body.join('') + 'return e' + el.index + '}')
        return name
    }

    // The only child of a default property holding a component (Repeater.delegate) is a delegate: it gets its own
    // scope before any class of the file is declared, so the aliases of those classes use the delegate's slots
    prepareDelegates(file){
        for(let el of file.elements.slice()){
            let defaultProperty = el.typeRef && el.typeRef.kind === 'builtin' && el.typeRef.Class.$defaultProperty
            if(!defaultProperty || el.children.length !== 1 || !this.isComponentProperty(el, defaultProperty)) continue
            let child = el.children[0]
            if(child.$asDelegate) continue
            child.$asDelegate = true
            file.reparentToScope(child, file.newScope(el.scope, true))
        }
    }

    emitFile(file){
        let className = file.typeRef.ref
        let root = file.root
        this.prepareDelegates(file)
        this.out.push('// ---- ' + file.fileName)
        // most types of a bundle are never used: the registries hold the accessor, the class is made on the first use
        let url = JSON.stringify(file.url || file.name + '.qml')
        this.out.push('let ' + className + '=null;function ' + className + '$(){return ' + className + '||' + className + '$init()}' + className + '$.$lazy=true;')
        this.out.push('JQ.files[' + url + ']=' + className + '$;')
        if(file.module) this.out.push('JQ.qmlModule(' + JSON.stringify(file.module) + ')[' + JSON.stringify(file.name) + ']=' + className + '$;')
        let start = this.out.length
        this.emitClass(file, root, className, true)
        if(file.createsComponents) this.out.push('const ' + className + '$qt=JQ.Qt.$at(' + url + ');')
        if(file.singleton) this.out.push(className + '.$singleton=true;')
        this.prepareBody(file, root)

        this.current = []
        let post = []
        this.current.push('const $s=new Array(' + root.scope.slots + ');$s[0]=$root;')
        this.applyMembers(file, root, '$root', post)
        this.emitChildren(file, root, '$root', post, this.use(root.typeRef))
        let body = this.current.concat(post)
        this.out.push('function ' + className + '$build($root){' + body.join('') + '}')
        let definition = this.out.splice(start)
        this.out.push('function ' + className + '$init(){' + definition.join('\n') + '\nreturn ' + className + '}')
    }

    generate(){
        for(let file of this.files.values()) this.resolveTypes(file)
        for(let file of this.files.values()) this.expandElementValues(file)
        for(let file of this.files.values()) this.prepareConnections(file)
        for(let [fileName, ref] of this.scripts){
            if(fs.existsSync(fileName)) this.out.push(emitScript(ref, fileName))
            else this.out.push('const ' + ref + '={};')
        }
        for(let file of this.files.values()) this.emitFile(file)
        return this.sharedFunctions(this.out.join('\n'))
    }
}

module.exports = { Codegen }
