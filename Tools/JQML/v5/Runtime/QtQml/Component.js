const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- Component: a creation function bound to the scope it was declared in ----------

class Component extends QtObject {
    constructor(parent = null, build = null){
        super(parent)
        this.$build = build
    }

    errorString(){ return this.$errorString || '' }

    createObject(parent = null, properties = null, context = null){
        if(!this.$build) return null
        let obj = null
        let previous = engine.beginCreate()
        try {
            obj = this.$build(parent, context)
            if(properties){
                for(let key in properties) obj[key] = properties[key]
            }
            engine.creatingRoot(obj)
        } finally {
            engine.endCreate(previous)
        }
        return obj
    }
}

defineType(Component, {
    props: {
        status: { kind: 'int', initial: 1, readonly: true },
        progress: { kind: 'real', initial: 1, readonly: true },
        url: { kind: 'string', readonly: true },
    },
})
Object.assign(Component, { Null: 0, Ready: 1, Loading: 2, Error: 3, PreferSynchronous: 0, Asynchronous: 1 })

module.exports = { Component }
