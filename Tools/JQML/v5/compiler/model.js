// QML file -> elements, scopes and members, from the AST of compiler/parser.js.

// A component body: the file root, an inline Component or a delegate. It owns an id table;
// slot 0 is always the body's root element.
class Scope {
    constructor(parent, isDelegate = false){
        this.parent = parent
        this.ids = new Map()
        this.slots = 1
        this.root = null
        this.isDelegate = isDelegate
        this.index = 0
    }
}

class Element {
    constructor(type, line, scope, parent){
        this.type = type
        this.line = line
        this.scope = scope
        this.parent = parent
        this.id = null
        this.slot = -1
        this.children = []
        this.assigns = []      // { name, value (AST), raw, element, elements, line }
        this.handlers = []     // { name, value, raw, line }
        this.propdefs = []     // { name, type, readonly, defaultProp, value, raw, element, line }
        this.aliases = []      // { name, path: [id, prop...] }
        this.signals = []      // { name, args }
        this.methods = []      // { name, raw, line }
        this.enums = []        // { name, values }
        this.typeRef = null
        this.className = null
        this.index = 0
    }

    hasMembers(){
        return this.propdefs.length || this.aliases.length || this.signals.length || this.methods.length || this.enums.length
    }
}

function pathName(name){
    if(typeof name === 'string') return name
    if(Array.isArray(name) && name[0] === 'dot') return pathName(name[1]) + '.' + pathName(name[2])
    if(Array.isArray(name) && name[0] === 'name') return name[1]
    return String(name)
}

function cleanRaw(raw){
    if(raw === undefined || raw === null) return ''
    return String(raw).trim().replace(/;+$/, '').trim()
}

function lineOf(node, fallback){
    return node && node.info ? node.info.line + 1 : fallback
}

// 1-based column of a node, 0 when the parser gave none
function colOf(node){
    return node && node.info && typeof node.info.col === 'number' ? node.info.col + 1 : 0
}

// 1-based position of a parser token: { line, col }, or null
function tokenAt(token){
    return token && typeof token.line === 'number' ? { line: token.line + 1, col: token.col + 1 } : null
}

// the position of a value (a statement node or the expression in it)
function valueAt(value){
    if(!value) return null
    return tokenAt(value.info) || (Array.isArray(value[1]) ? tokenAt(value[1].info) : null)
}

function isHandlerName(name){
    let last = name.slice(name.lastIndexOf('.') + 1)
    return /^on[A-Z]/.test(last)
}

class FileModel {
    constructor(name, fileName){
        this.name = name
        this.fileName = fileName
        this.imports = []
        this.pragmas = []
        this.root = null
        this.scopes = []
        this.elements = []
        this.singleton = false
    }

    // Moves an element subtree into another scope (a child that turns out to be a delegate)
    reparentToScope(el, scope){
        let old = el.scope
        let visit = (node) => {
            if(node.scope !== old) return
            node.scope = scope
            if(node.id){
                old.ids.delete(node.id)
                node.slot = node === el ? 0 : scope.slots++
                scope.ids.set(node.id, node.slot)
            } else if(node === el){
                node.slot = 0
            }
            for(let child of node.children) visit(child)
        }
        visit(el)
        el.parent = null
        scope.root = el
    }

    newScope(parent, isDelegate){
        let scope = new Scope(parent, isDelegate)
        scope.index = this.scopes.length
        this.scopes.push(scope)
        return scope
    }

    build(ast){
        // ['toplevel', imports, ['qmlelem', ...], pragmas]
        for(let imp of ast[1] || []){
            let at = tokenAt(imp.info) || { line: 1, col: 1 }
            this.imports.push({ path: imp[1], version: imp[2], as: imp[3], isModule: imp[4], line: at.line, col: at.col })
        }
        for(let pragma of ast[3] || []) this.pragmas.push(pragma)
        this.singleton = this.pragmas.some(p => JSON.stringify(p).indexOf('Singleton') >= 0)
        let scope = this.newScope(null, false)
        this.root = this.element(ast[2], scope, null)
        return this
    }

    element(node, scope, parent){
        let el = new Element(pathName(node[1]), lineOf(node, parent ? parent.line : 1), scope, parent)
        el.col = colOf(node)
        el.onProp = node[2] || null     // `Behavior on x`, `NumberAnimation on x`
        el.index = this.elements.length
        this.elements.push(el)
        if(!scope.root){
            scope.root = el
            el.slot = 0
        }

        for(let member of node[3] || []){
            if(!member) continue
            this.member(el, member, scope)
        }
        return el
    }

    // Element value of a property: a new scope when the property holds a component
    elementValue(node, scope, owner, asComponent){
        if(asComponent){
            let inner = this.newScope(scope, true)
            return this.element(node, inner, null)
        }
        return this.element(node, scope, owner)
    }

    member(el, m, scope){
        let line = lineOf(m, el.line)
        let col = colOf(m)
        switch(m[0]){
            case 'qmlprop': {
                let name = pathName(m[1])
                let value = m[2]
                if(name === 'id'){
                    el.id = value[1][1]
                    el.idAt = { line, col }
                    el.idValueAt = valueAt(value)
                    // a second object with the id of this scope: QQmlComponentAndAliasResolver reports it
                    if(scope.ids.has(el.id) && !scope.duplicateId) scope.duplicateId = el
                    if(el.slot !== 0) el.slot = scope.slots++
                    scope.ids.set(el.id, el.slot)
                    return
                }
                if(isHandlerName(name)){
                    el.handlers.push({ name, value, raw: cleanRaw(m[3]), line, col })
                    return
                }
                let inner = value && value[0] === 'stat' ? value[1] : null
                if(inner && inner[0] === 'qmlelem'){
                    el.assigns.push({ name, elementNode: inner, line, col })
                    return
                }
                if(inner && inner[0] === 'array' && inner[1].length && inner[1].every(item => item[0] === 'qmlelem')){
                    el.assigns.push({ name, elementNodes: inner[1], line, col })
                    return
                }
                // `anchors.left`: the position of the last name; the value's own position
                let nameAt = Array.isArray(m[1]) ? tokenAt(m[1].info) : null
                el.assigns.push({ name, value, raw: cleanRaw(m[3]), line, col, nameAt: nameAt || { line, col }, valueAt: valueAt(value) })
                return
            }
            case 'qmlobj': {
                let group = pathName(m[1])
                for(let sub of m[2] || []){
                    if(sub[0] !== 'qmlprop') continue
                    this.member(el, ['qmlprop', group + '.' + pathName(sub[1]), sub[2], sub[3]], scope)
                }
                return
            }
            case 'qmlpropdef': {
                let mods = m[1] || []
                let def = { name: m[2], type: m[3], defaultProp: !!mods[0], required: !!mods[1], readonly: !!mods[2], line }
                def.nameAt = tokenAt(m.nameInfo) || { line, col }
                def.declAt = tokenAt(m.declInfo) || tokenAt(m.info) || { line, col }
                let value = m[4]
                def.valueAt = valueAt(value)
                let inner = value && value[0] === 'stat' ? value[1] : null
                if(inner && inner[0] === 'qmlelem') def.elementNode = inner
                else if(value){
                    def.value = value
                    def.raw = cleanRaw(m[5])
                }
                el.propdefs.push(def)
                return
            }
            case 'qmlaliasdef': {
                let path = m.slice(2).filter(part => typeof part === 'string' && part)
                // info: the tokens of the reference `id.property`
                let refAt = Array.isArray(m.info) ? tokenAt(m.info[0]) : null
                el.aliases.push({ name: m[1], path, line: refAt ? refAt.line : el.line, refAt, nameAt: tokenAt(m.nameInfo) })
                return
            }
            case 'qmlsignaldef':
                el.signals.push({ name: m[1], args: (m[2] || []).map(a => a.name), nameAt: tokenAt(m.info) })
                return
            case 'qmlmethod':
                el.methods.push({ name: m[1], raw: cleanRaw(m[3]), line, nameAt: tokenAt(m.info) })
                return
            case 'qmldefaultprop': {
                let inner = m[1]
                this.member(el, inner, scope)
                if(inner[0] === 'qmlaliasdef') el.defaultAlias = el.aliases[el.aliases.length - 1]
                if(inner[0] === 'qmlpropdef') el.propdefs[el.propdefs.length - 1].defaultProp = true
                return
            }
            case 'qmlenumdef':
                el.enums.push({ name: m[1], values: m[2] })
                return
            case 'qmlinlinecomponent':
                // not compiled yet (compiler/checks.js reports it)
                if(!el.inlineComponents) el.inlineComponents = []
                el.inlineComponents.push({ name: m[1], at: tokenAt(m.info) || { line, col } })
                return
            case 'qmlelem': {
                if(el.type === 'Component'){
                    el.componentBody = this.element(m, this.newScope(scope, true), null)
                    return
                }
                let child = this.element(m, scope, el)
                el.children.push(child)
                return
            }
            default:
                return
        }
    }
}

module.exports = { FileModel, Element, Scope, cleanRaw, isHandlerName }
