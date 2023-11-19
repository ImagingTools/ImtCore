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
        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = this.delegate.get().createObject(this.parent(), this.$exCtx)
            obj.getStatement('index').reset(index)
            obj.getStatement('model').reset({index: index})
            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = this.delegate.get().createObject(this.parent(), this.$exCtx)
            obj.getStatement('index').setCompute(()=>{obj.getStatement('index').subscribe(model.getStatement('index')); return model.getStatement('index').get()})
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
        
        this.parent().preventAutoUpdateGeometry = true
        for(let i = 0; i < this.$items.length.get(); i++){
            this.createElement(i)
        }
        delete this.parent().preventAutoUpdateGeometry
        if(this.parent().updateGeometry) this.parent().updateGeometry()
        this.getProperty('count').reset(this.$items.length)
    }


    $destroy(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.$items.length.unsubscribe()
        super.$destroy()
    }
}

module.exports.Repeater = Repeater