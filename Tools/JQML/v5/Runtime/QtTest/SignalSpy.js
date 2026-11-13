const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('../QtQuick/Item')
const { TestFailError } = require('./TestCase')

class SignalSpy extends Item {
    constructor(parent = null){
        super(parent)
        this.$link = null
        this.$signals = []
    }

    $connect(){
        if(this.$link){
            engine.unsubscribe(this.$link)
            this.$link = null
        }
        let target = this.$v[SignalSpy.TARGET], name = this.$v[SignalSpy.SIGNALNAME]
        if(!target || !name || !target.constructor.$type) return
        if(/^on[A-Z]/.test(name)) name = name[2].toLowerCase() + name.slice(3)
        let id = target.constructor.$type.signalByName.get(name)
        if(id === undefined){
            this.$write(SignalSpy.VALID, false)
            return
        }
        this.$link = engine.connect(target, id, (...args) => {
            this.$signals.push(args)
            this.$write(SignalSpy.COUNT, this.$signals.length)
        })
        this.$write(SignalSpy.VALID, true)
    }

    get signalArguments(){ return this.$signals.slice() }

    clear(){
        this.$signals = []
        this.$write(SignalSpy.COUNT, 0)
    }

    async wait(timeout = 5000){
        let start = this.$signals.length
        let deadline = Date.now() + timeout
        while(this.$signals.length === start){
            if(Date.now() >= deadline) throw new TestFailError('No signal received')
            await new Promise(resolve => setTimeout(resolve, 10))
        }
    }

    $disposeNative(){
        if(this.$link) engine.unsubscribe(this.$link)
        super.$disposeNative()
    }
}

const reconnect = (spy) => spy.$connect()

defineType(SignalSpy, {
    props: {
        target: { kind: 'var', effect: reconnect },
        signalName: { kind: 'string', effect: reconnect },
        count: { kind: 'int', readonly: true },
        valid: { kind: 'bool', readonly: true },
    },
})

module.exports = { SignalSpy }
