const { Item } = require('./Item')
const { QVar, QReal } = require('../utils/properties')

class Repeater extends Item {
    static defaultProperties = {
        model: { type: QVar, value: undefined, changed: '$modelChanged' },
        delegate: { type: QVar, changed: '$delegateChanged' },
        count: { type: QReal, value: 0 },
    }

    static defaultSignals = {
        itemAdded: { params: ['index', 'item'] },
        itemRemoved: { params: ['index', 'item'] },
    }
    constructor(parent){
        super(parent)
        
        this.setStyle({
            display: 'none'
        })

        this.$items = {
            length: new QInt(0)
        }
        this.$items.length.getNotify().connect(()=>{
            this.updateView()
        })

        this.setStyle({
            maxWidth: '0px',
            maxHeight: '0px',
        })
    }

    $modelChanged(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.$items.length.reset(0)
        if(typeof this.getPropertyValue('model') === 'number'){     
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getProperty('model')); return this.getPropertyValue('model')})
        } else {
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getPropertyValue('model').getProperty('data')); return this.getPropertyValue('model').getPropertyValue('data').length}) 
        }
        this.$items.length.update()
    }

    $visibleChanged(){

    }

    $delegateChanged(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.updateView()
    }

    createElement(index){
        if(this.$items[index]) return this.$items[index]
        let ctx = new ContextController(this.$exCtx, this.delegate.get().$exCtx)
        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = this.delegate.get().createObject(this.parent, ctx)
            obj.getStatement('index').reset(index)
            obj.getStatement('model').reset({index: index})
            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = this.delegate.get().createObject(this.parent, ctx)
            obj.getStatement('index').setCompute(()=>{return model.index})
            obj.getStatement('index').update()
            obj.getStatement('model').reset(model)
            this.$items[index] = obj   
        }
        for(let update of updateList.splice(0, updateList.length)){
            update()
        }
        
        this.$items[index].$complete()
        return this.$items[index]
    }

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return
        
        for(let key in this.$items){
            if(key !== 'length') {
                if(this.$signals.itemRemoved) this.$signals.itemRemoved(key, this.$items[key])
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }

        this.parent.preventAutoUpdateGeometry = true
        for(let i = 0; i < this.$items.length.get(); i++){
            this.createElement(i)
            if(this.$signals.itemAdded) this.$signals.itemAdded(i, this.$items[i])
        }
        delete this.parent.preventAutoUpdateGeometry
        if(this.parent.updateGeometry) this.parent.updateGeometry()
        this.getProperty('count').reset(this.$items.length.get())
    }


    $destroy(){
        // for(let key in this.$items){
        //     if(key !== 'length') {
        //         this.$items[key].$destroy()
        //         delete this.$items[key]
        //     }
        // }
        this.$items.length.unsubscribe()
        super.$destroy()
    }
}

module.exports.Repeater = Repeater