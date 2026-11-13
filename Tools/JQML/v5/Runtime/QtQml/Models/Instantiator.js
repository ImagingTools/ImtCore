const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { ListModel } = require('./ListModel')
const { ModelContext, modelRows } = require('./ModelContext')

class Instantiator extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$objects = []
        this.$contexts = []
        this.$model = null
        engine.scheduleComponentComplete(this)
    }

    objectAt(index){ return this.$objects[index] || null }

    $componentComplete(){
        super.$componentComplete()
        this.$sync()
    }

    $sync(){
        let model = this.$v[Instantiator.MODEL]
        if(model !== this.$model){
            if(this.$model instanceof ListModel) this.$model.$removeView(this)
            this.$model = model
            if(model instanceof ListModel) model.$addView(this)
        }
        this.$regenerate()
    }

    $clear(){
        for(let i = this.$objects.length - 1; i >= 0; i--){
            let obj = this.$objects[i]
            engine.emitArgs(this, Instantiator.SIGNAL_objectRemoved, [i, obj])
            if(obj) obj.$releaseLater()
        }
        this.$objects = []
        this.$contexts = []
    }

    $regenerate(){
        this.$clear()
        let delegate = this.$v[Instantiator.DELEGATE]
        if(!delegate || !this.$v[Instantiator.ACTIVE] || this.$f & engine.DESTROYED){
            this.$updateCount()
            return
        }
        let rows = modelRows(this.$model)
        for(let i = 0; i < rows.count; i++) this.$create(i, rows)
        this.$updateCount()
        for(let i = 0; i < this.$objects.length; i++) engine.emitArgs(this, Instantiator.SIGNAL_objectAdded, [i, this.$objects[i]])
    }

    $create(index, rows){
        let ctx = new ModelContext(rows.row(index), index, rows.data(index))
        ctx.$view = this
        let obj = this.$v[Instantiator.DELEGATE].createObject(null, null, ctx)
        this.$objects.splice(index, 0, obj)
        this.$contexts.splice(index, 0, ctx)
        return obj
    }

    $updateCount(){
        this.$write(Instantiator.COUNT, this.$objects.length)
        this.$write(Instantiator.OBJECT, this.$objects[0] || null)
    }

    $modelChanged(kind, index, count, roles = null){
        if(!this.$v[Instantiator.DELEGATE] || !this.$v[Instantiator.ACTIVE]) return
        let rows = modelRows(this.$model)
        if(kind === 'insert'){
            for(let i = 0; i < count; i++) this.$create(index + i, rows)
            this.$reindex()
            this.$updateCount()
            for(let i = 0; i < count; i++) engine.emitArgs(this, Instantiator.SIGNAL_objectAdded, [index + i, this.$objects[index + i]])
        } else if(kind === 'remove'){
            let removed = this.$objects.splice(index, count)
            this.$contexts.splice(index, count)
            this.$reindex()
            this.$updateCount()
            for(let i = 0; i < removed.length; i++){
                engine.emitArgs(this, Instantiator.SIGNAL_objectRemoved, [index + i, removed[i]])
                if(removed[i]) removed[i].$releaseLater()
            }
        } else if(kind === 'change'){
            for(let i = index; i < index + count; i++){
                let ctx = this.$contexts[i]
                if(!ctx) continue
                ctx.$row = rows.row(i)
                ctx.$write(ModelContext.MODELDATA, rows.data(i))
                ctx.$rowChanged(roles)
            }
        } else {
            this.$regenerate()
        }
    }

    $reindex(){
        for(let i = 0; i < this.$contexts.length; i++) this.$contexts[i].$write(ModelContext.INDEX, i)
    }

    $disposeNative(){
        if(this.$model instanceof ListModel) this.$model.$removeView(this)
        this.$clear()
        super.$disposeNative()
    }
}

const resync = (inst) => { if(inst.$f & engine.COMPLETED) inst.$sync() }
defineType(Instantiator, {
    props: {
        model: { kind: 'var', initial: 1, effect: resync },
        delegate: { kind: 'component', effect: resync },
        active: { kind: 'bool', initial: true, effect: resync },
        asynchronous: 'bool',
        count: { kind: 'int', readonly: true },
        object: { kind: 'var', readonly: true },
    },
    signals: ['objectAdded(index, object)', 'objectRemoved(index, object)'],
})

module.exports = { Instantiator }
