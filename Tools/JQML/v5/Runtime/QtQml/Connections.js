const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// "If this property is not set, the target defaults to the parent of the Connection."
// "If set to null, no connection is made and any signal handlers are ignored until the target is not null."
class Connections extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$handlers = []      // { signal, fn, scope }
        this.$links = []
        this.$targetSet = false
        engine.scheduleComponentComplete(this)
    }

    // signal handler written in the Connections element, compiled by compiler2
    $handle(signal, fn, scope, declaredFunction = false){
        this.$handlers.push({ signal, fn, scope, declaredFunction })
    }

    $assign(id, value){
        if(id === Connections.TARGET) this.$targetSet = true
        super.$assign(id, value)
    }

    $bind(id, fn, scope, self){
        if(id === Connections.TARGET) this.$targetSet = true
        return super.$bind(id, fn, scope, self)
    }

    $componentComplete(){
        if(!this.$targetSet) this.$write(Connections.TARGET, this.$parent)
        super.$componentComplete()
        this.$connect()
    }

    $connect(){
        for(let link of this.$links) engine.unsubscribe(link)
        this.$links = []
        let target = this.$v[Connections.TARGET]
        if(!target || !(this.$f & engine.COMPLETED) || !target.constructor || !target.constructor.$type) return
        let type = target.constructor.$type
        for(let handler of this.$handlers){
            let id = type.signalByName.get(handler.signal)
            if(id === undefined){
                if(!this.$v[Connections.IGNOREUNKNOWNSIGNALS]){
                    // the qmlWarning of QQmlConnections::connectSignals / connectSignalsToBindings
                    let name = 'on' + handler.signal[0].toUpperCase() + handler.signal.slice(1)
                    console.warn('Connections: ' + (handler.declaredFunction
                        ? 'Detected function "' + name + '" in Connections element. This is probably intended to be a signal handler but no signal of the target matches the name.'
                        : 'Cannot assign to non-existent property "' + name + '"'))
                }
                continue
            }
            let connections = this
            let link = engine.connect(target, id, function(...args){
                if(connections.$v[Connections.ENABLED] && !(handler.scope && engine.scopeCleared(handler.scope))) handler.fn(connections, handler.scope, ...args)
            }, null)
            this.$links.push(link)
        }
    }

    $disposeNative(){
        for(let link of this.$links) engine.unsubscribe(link)
        this.$links = []
    }
}

defineType(Connections, {
    props: {
        target: { kind: 'var', effect: (c) => c.$connect() },
        enabled: { kind: 'bool', initial: true },
        ignoreUnknownSignals: 'bool',
    },
})

module.exports = { Connections }
