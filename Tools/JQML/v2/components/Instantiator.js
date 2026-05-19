const { QtObject } = require('./QtObject')
const { QVar, QReal, QBool, QInt } = require('../utils/properties')

class Instantiator extends QtObject {
    static defaultProperties = {
        active: { type: QBool, value: true, changed: '$activeChanged' },
        model: { type: QVar, value: 1, changed: '$modelChanged' },
        delegate: { type: QVar, changed: '$delegateChanged' },
        count: { type: QInt, value: 0 },
        object: { type: QVar, value: null },
    }

    static defaultSignals = {
        objectAdded: { params: ['index', 'object'] },
        objectRemoved: { params: ['index', 'object'] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.$exCtx = exCtx
        this.$objects = []
    }

    objectAt(index){
        return index >= 0 && index < this.$objects.length ? this.$objects[index] : null
    }

    $complete(){
        super.$complete()
        this.$completed = true
        this.$rebuild()
    }

    $activeChanged(){
        if(this.$completed) this.$rebuild()
    }

    $modelChanged(){
        if(this.$completed) this.$rebuild()
    }

    $delegateChanged(){
        if(this.$completed) this.$rebuild()
    }

    $destroyAll(){
        for(let i = this.$objects.length - 1; i >= 0; i--){
            let obj = this.$objects[i]
            if(this.$signals.objectRemoved) this.$signals.objectRemoved(i, obj)
            // Only destroy if not already destroyed by the signal handler
            if(obj.UID !== undefined) obj.destroy()
        }
        this.$objects = []
        this.getProperty('count').reset(0)
        this.getProperty('object').reset(null)
    }

    $rebuild(){
        this.$destroyAll()

        if(!this.getPropertyValue('active')) return
        if(!this.getPropertyValue('delegate')) return

        let model = this.getPropertyValue('model')
        let count = 0

        if(model === undefined || model === null){
            return
        } else if(typeof model === 'number'){
            count = Math.max(0, Math.floor(model))
        } else if(Array.isArray(model)){
            count = model.length
        } else if(model && typeof model.getPropertyValue === 'function' && model.getPropertyValue('data')){
            count = model.getPropertyValue('data').length
        } else {
            return
        }

        let delegate = this.getStatement('delegate').get()
        let createObject = delegate.createObject
        let cls = delegate.constructor

        for(let i = 0; i < count; i++){
            let ctx = new ContextController(delegate.$exCtx, this.$exCtx)
            let obj

            if(typeof model === 'number'){
                obj = createObject ? createObject(this.parent, ctx, {index: i}, false) : new cls(this.parent, ctx, {index: i})
            } else if(Array.isArray(model)){
                obj = createObject ? createObject(this.parent, ctx, {'$modelData': model[i], index: i}, false) : new cls(this.parent, ctx, {'$modelData': model[i], index: i})
            } else {
                let modelData = model.getPropertyValue('data')[i]
                obj = createObject ? createObject(this.parent, ctx, modelData, false) : new cls(this.parent, ctx, modelData)
            }

            for(let update of updateList.splice(0, updateList.length)){
                update()
            }

            obj.$complete()
            this.$objects.push(obj)
            if(this.$signals.objectAdded) this.$signals.objectAdded(i, obj)
        }

        this.getProperty('count').reset(this.$objects.length)
        this.getProperty('object').reset(this.$objects.length > 0 ? this.$objects[0] : null)
    }

    destroy(){
        this.$destroyAll()
        super.destroy()
    }
}

module.exports.Instantiator = Instantiator
