// The errors Qt reports while it loads a document, before the object exists: QmlIR::IRBuilder (phase 1), the type
// compiler and QQmlPropertyValidator (phase 2), the creation (phase 3, Qt.createQmlObject only). Positions and texts
// follow Qt 6.8. Codegen.report decides what they are: warnings of the build, errors of a document compiled in the
// browser (compiler/runtime.js).
const { NAMED } = require('../Runtime/Core/color')

const ILLEGAL_IDS = new Set(['undefined', 'eval', 'parseInt', 'parseFloat', 'isNaN', 'isFinite', 'decodeURI',
    'decodeURIComponent', 'encodeURI', 'encodeURIComponent', 'escape', 'unescape'])

// the value kinds of QQmlPropertyValidator::validateLiteralBinding, by QML type name
const QML_TYPE_KIND = {
    int: 'int', real: 'real', double: 'real', bool: 'bool', string: 'string', url: 'url', color: 'color',
    var: 'var', variant: 'var',
}
// the type Qt names in "expecting ..." (the C++ meta type)
const META_TYPE = { int: 'int', real: 'double', bool: 'bool', string: 'QString', url: 'QUrl', color: 'QColor' }

// a literal binding (QV4::CompiledData::Binding::Type_String, Number, Boolean) or null for a script
function literalOf(value){
    let node = value && value[0] === 'stat' ? value[1] : value
    if(!node) return null
    if(node[0] === 'string') return { type: 'string', value: node[1] }
    if(node[0] === 'num') return { type: 'number', value: node[1] }
    if(node[0] === 'name' && (node[1] === 'true' || node[1] === 'false')) return { type: 'boolean', value: node[1] === 'true' }
    if(node[0] === 'unary-prefix' && node[1] === '-' && node[2] && node[2][0] === 'num') return { type: 'number', value: -node[2][1] }
    return null
}

function isColorString(text){
    let value = String(text).trim()
    if(/^#([0-9a-f]{3}|[0-9a-f]{6}|[0-9a-f]{8}|[0-9a-f]{9}|[0-9a-f]{12})$/i.test(value)) return true
    let name = value.toLowerCase()
    return name === 'transparent' || Object.prototype.hasOwnProperty.call(NAMED, name)
}

// QQmlPropertyValidator::validateLiteralBinding: the message, or null
function literalError(kind, literal){
    switch(kind){
        case 'int': return literal.type === 'number' && Number.isInteger(literal.value) ? null : 'Invalid property assignment: int expected'
        case 'real': return literal.type === 'number' ? null : 'Invalid property assignment: number expected'
        case 'bool': return literal.type === 'boolean' ? null : 'Invalid property assignment: boolean expected'
        case 'string': return literal.type === 'string' ? null : 'Invalid property assignment: string expected'
        case 'url': return literal.type === 'string' ? null : 'Invalid property assignment: url expected'
        case 'color': return literal.type === 'string' && isColorString(literal.value) ? null : 'Invalid property assignment: color expected'
        default: return null
    }
}

// the classes from the type of el up to the runtime class it builds on
function classesOf(typeRef){
    let classes = []
    for(let type = typeRef; type; type = type.kind === 'qml' ? type.file.root.typeRef : null){
        if(type.kind === 'builtin') classes.push(type.Class)
    }
    return classes
}

// what an assignment to name of el writes: { kind, readonly } (kind: a value kind, 'var', 'object', 'alias', 'group')
function propertyOf(codegen, el, name){
    let m = codegen.member(el, name)
    if(!m) return null
    if(m.kind === 'alias') return { kind: 'alias', readonly: false }
    if(m.kind !== 'prop') return null
    if(m.builtin){
        if(m.group) return { kind: 'group', readonly: false }
        let readonly = !!m.prop.readonly
        for(let Class of classesOf(el.typeRef)){
            if(Class.$readonlyNames && Class.$readonlyNames.has(name)) readonly = true
        }
        return { kind: m.prop.kind === 'component' ? 'object' : m.prop.kind, readonly }
    }
    let def = m.own.propdefs.find(p => p.name === name)
    let kind = QML_TYPE_KIND[def.type] || (def.type === 'Component' ? 'object' : /^[A-Z]/.test(def.type) || /^list</.test(def.type) ? 'object' : 'var')
    // a read-only declaration takes its value where it is declared only
    return { kind, readonly: def.readonly && m.own !== el, qmlType: def.type }
}

function checkFile(codegen, file, creation){
    let report = (phase, at, message, warning, follow) => codegen.report(file, phase, at, message, warning, follow)

    // ---- phase 0: QQmlTypeLoader ---------------------------------------------------
    for(let el of file.elements){
        for(let component of el.inlineComponents || []){
            // a document of Qt.createQmlObject has no url for the type of its inline component; the build has none yet
            report(0, component.at, 'Type ' + component.name + ' unavailable', 'inline component ' + component.name + ' is not supported yet',
                { line: -1, col: -1, message: 'No such file or directory' })
        }
    }

    // ---- phase 1: QmlIR::IRBuilder -------------------------------------------------
    for(let el of file.elements){
        if(el.id){
            let at = el.idValueAt || el.idAt
            if(/^[A-Z]/.test(el.id)) report(1, at, 'IDs cannot start with an uppercase letter')
            else if(!/^[a-z_]/.test(el.id)) report(1, at, 'IDs must start with a letter or underscore')
            else if(!/^[A-Za-z0-9_]+$/.test(el.id)) report(1, at, 'IDs must contain only letters, numbers, and underscores')
            else if(ILLEGAL_IDS.has(el.id)) report(1, at, 'ID illegally masks global JavaScript property')
        }
        // QmlIR::Object::appendProperty, appendAlias, appendSignal, appendFunction
        let properties = new Set(), aliases = new Set(), signals = new Set(), methods = new Set()
        let defaults = 0
        for(let def of el.propdefs){
            if(properties.has(def.name)) report(1, def.nameAt, 'Duplicate property name')
            else if(aliases.has(def.name)) report(1, def.nameAt, 'Property duplicates alias name')
            else if(/^[A-Z]/.test(def.name)) report(1, def.nameAt, 'Property names cannot begin with an upper case letter')
            if(def.defaultProp && ++defaults > 1) report(1, def.declAt, 'Duplicate default property')
            properties.add(def.name)
        }
        for(let alias of el.aliases){
            let at = alias.nameAt || alias.refAt
            if(aliases.has(alias.name)) report(1, at, 'Duplicate alias name')
            else if(properties.has(alias.name)) report(1, at, 'Alias has same name as existing property')
            else if(/^[A-Z]/.test(alias.name)) report(1, at, 'Alias names cannot begin with an upper case letter')
            aliases.add(alias.name)
        }
        for(let signal of el.signals){
            if(signals.has(signal.name)) report(1, signal.nameAt, 'Duplicate signal name')
            else if(/^[A-Z]/.test(signal.name)) report(1, signal.nameAt, 'Signal names cannot begin with an upper case letter')
            signals.add(signal.name)
        }
        for(let method of el.methods){
            if(methods.has(method.name)) report(1, method.nameAt, 'Duplicate method name')
            else if(/^[A-Z]/.test(method.name)) report(1, method.nameAt, 'Method names cannot begin with an upper case letter')
            methods.add(method.name)
        }
        // QmlIR::Object::appendBinding: a property bound twice (its declaration with a value counts)
        let bound = new Set(el.propdefs.filter(def => def.value || def.elementNode).map(def => def.name))
        for(let assign of el.assigns){
            if(bound.has(assign.name)) report(1, assign.nameAt || assign, 'Property value set multiple times')
            bound.add(assign.name)
        }
    }

    // ---- phase 2: the type compiler and QQmlPropertyValidator ----------------------
    for(let scope of file.scopes){
        if(scope.duplicateId) report(2, scope.duplicateId.idAt, 'id is not unique')
    }
    for(let el of file.elements){
        for(let alias of el.aliases){
            if(!el.scope.ids.has(alias.path[0])) report(2, alias.refAt, 'Invalid alias reference. Unable to find id "' + alias.path[0] + '"')
        }
        for(let def of el.propdefs){
            let kind = QML_TYPE_KIND[def.type]
            let literal = def.value ? literalOf(def.value) : null
            if(kind && literal){
                let message = literalError(kind, literal)
                if(message) report(2, def.valueAt || def.nameAt, message)
            }
            // QQmlPropertyValidator::validateObjectBinding: an object for a value type
            if(kind && kind !== 'var' && def.element){
                report(2, def.nameAt, 'Cannot assign value of type "' + def.element.type + '" to property "' + def.name + '", expecting "' + (META_TYPE[kind] || def.type) + '"')
            }
        }
        for(let assign of el.assigns){
            if(assign.name.indexOf('.') >= 0) continue
            let property = propertyOf(codegen, el, assign.name)
            if(!property) continue
            let literal = assign.value ? literalOf(assign.value) : null
            if(property.readonly){
                // QQmlEnumTypeResolver rejects a script bound to an int first, at the name; the validator at the value
                let at = !literal && !assign.element && property.kind === 'int' ? assign.nameAt : (assign.valueAt || assign.nameAt)
                report(2, at, 'Invalid property assignment: "' + assign.name + '" is a read-only property')
                continue
            }
            if(literal){
                let message = literalError(property.kind, literal)
                if(message) report(2, assign.valueAt || assign.nameAt, message)
            }
            if(assign.element && META_TYPE[property.kind]){
                report(2, assign.nameAt, 'Cannot assign value of type "' + assign.element.type + '" to property "' + assign.name + '", expecting "' + META_TYPE[property.kind] + '"')
            }
        }
    }

    // ---- phase 3: the creation of the document ------------------------------------
    if(creation){
        for(let def of file.root.propdefs){
            if(def.required && !def.value && !def.elementNode) report(3, def.declAt, 'Required property ' + def.name + ' was not initialized')
        }
        for(let el of file.elements){
            if(el === file.root || el.scope.isDelegate) continue
            for(let name of requiredOf(el.typeRef)){
                let set = el.assigns.some(a => a.name === name) || el.propdefs.some(d => d.name === name && (d.value || d.elementNode))
                if(!set) report(3, { line: el.line, col: el.col }, 'Required property ' + name + ' was not initialized')
            }
        }
    }
}

// the required properties of a type: its QML files, and $qmlRequired of a compiled class
function requiredOf(typeRef){
    let names = []
    for(let type = typeRef; type; type = type.kind === 'qml' ? type.file.root.typeRef : null){
        if(type.kind === 'qml') for(let def of type.file.root.propdefs) if(def.required && !def.value && !def.elementNode) names.push(def.name)
        if(type.kind === 'builtin' && type.Class.$qmlRequired) names.push(...type.Class.$qmlRequired)
    }
    return names
}

module.exports = { checkFile }
