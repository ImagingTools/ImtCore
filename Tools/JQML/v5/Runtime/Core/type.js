// Type layout: properties and signals get numbers once per class (inherited numbering),
// accessors are defined on the prototype; values live in the per-object array $v.
const kinds = require('./values')
const engine = require('./engine')

class TypeInfo {
    constructor(base){
        this.props = base ? base.props.slice() : []
        this.propByName = new Map(base ? base.propByName : [])
        this.signals = base ? base.signals.slice() : []
        this.signalByName = new Map(base ? base.signalByName : [])
        this.handlers = base ? base.handlers.slice() : []
        this.signalArgs = base ? base.signalArgs.slice() : []
        this.initial = base ? base.initial.slice() : []
    }

    addSignal(name, args = []){
        let id = this.signals.length
        this.signals.push(name)
        this.signalByName.set(name, id)
        this.handlers.push(null)
        this.signalArgs.push(args)
        return id
    }
}

// spec.props: { name: kindName | { kind, initial, effect, get, set, group } }
// spec.signals: [names]; spec.handlers: { signalName: (obj, a, b) => {} } — chained after the base handler
// obj.done(1, 'x') emits, obj.done.connect(fn) / disconnect(fn) subscribe; the callable is made on first use
function defineSignalAccessor(Class, name, id){
    if(Object.getOwnPropertyDescriptor(Class.prototype, name)) return
    Object.defineProperty(Class.prototype, name, {
        get(){
            let facades = this.$facades || (this.$facades = new Map())
            let facade = facades.get(id)
            if(!facade){
                let obj = this
                facade = function(...args){ engine.emitArgs(obj, id, args) }
                facade.connect = (receiver, fn) => {
                    if(typeof receiver === 'function') return engine.connect(obj, id, receiver, null)
                    return engine.connect(obj, id, typeof fn === 'string' ? receiver[fn] : fn, receiver)
                }
                facade.disconnect = (receiver, fn) => {
                    if(typeof receiver === 'function') return engine.disconnect(obj, id, receiver, null)
                    return engine.disconnect(obj, id, typeof fn === 'string' ? receiver[fn] : fn, receiver)
                }
                facades.set(id, facade)
            }
            return facade
        },
        configurable: true,
    })
}

function defineType(Class, spec = {}){
    let base = Object.getPrototypeOf(Class).$type || null
    let type = new TypeInfo(base)

    for(let name of Object.keys(spec.props || {})){
        let def = spec.props[name]
        if(typeof def === 'string') def = { kind: def }
        let isGroup = 'group' in def
        let kind = isGroup ? null : kinds[def.kind]
        if(!isGroup && !kind) throw new Error('unknown property kind ' + def.kind + ' of ' + Class.name + '.' + name)

        let id = type.props.length
        let signal = type.addSignal(name + 'Changed')
        defineSignalAccessor(Class, name + 'Changed', signal)
        Class['SIGNAL_' + name + 'Changed'] = signal
        let prop = {
            id: id,
            name: name,
            signal: signal,
            kind: isGroup ? 'group' : def.kind,
            coerce: kind ? kind.coerce : null,
            effect: def.effect || null,
            group: def.group || null,
        }
        type.props.push(prop)
        type.propByName.set(name, prop)
        type.initial.push(isGroup ? null : ('initial' in def ? def.initial : kind.initial))

        if(isGroup){
            let Group = def.group
            Object.defineProperty(Class.prototype, name, {
                get(){ return this.$v[id] || (this.$v[id] = new Group(this)) },
                set(value){
                    let group = this.$v[id] || (this.$v[id] = new Group(this))
                    for(let key in value) group[key] = value[key]
                },
                configurable: true,
            })
        } else {
            Object.defineProperty(Class.prototype, name, {
                get: def.get || function(){
                    if(this.$pending) engine.flushPending(this, id)
                    engine.track(this, signal)
                    return this.$v[id]
                },
                set: def.set || (def.readonly
                    ? function(){ throw new TypeError('Cannot assign to read-only property "' + name + '"') }
                    : function(value){ this.$assign(id, value) }),
                configurable: true,
            })
        }
        Class[name.toUpperCase()] = id
    }

    // 'clicked' or 'clicked(mouse)'
    for(let decl of spec.signals || []){
        let match = /^([\w.]+)(?:\((.*)\))?$/.exec(decl)
        let name = match[1]
        let args = match[2] ? match[2].split(',').map(a => a.trim()).filter(Boolean) : []
        let id = type.addSignal(name, args)
        Class['SIGNAL_' + name] = id
        if(name.indexOf('.') < 0) defineSignalAccessor(Class, name, id)
    }

    for(let name of Object.keys(spec.handlers || {})){
        let id = type.signalByName.get(name)
        if(id === undefined) throw new Error('unknown signal ' + name + ' of ' + Class.name)
        let own = spec.handlers[name]
        let inherited = type.handlers[id]
        type.handlers[id] = inherited ? (obj, a, b) => { inherited(obj, a, b); own(obj, a, b) } : own
    }

    Class.$type = type
    if(typeof Class.$defined === 'function') Class.$defined(Class, type, Object.keys(spec.props || {}))
    return Class
}

module.exports = { defineType, TypeInfo }
