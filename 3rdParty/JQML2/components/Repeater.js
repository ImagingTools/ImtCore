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

    $repeater = true
    
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx
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

    itemAt(index){
        return index >= 0 && index < this.$items.length.get() ? this.$items[index] : undefined
    }

    $modelDataChanged(leftTop, bottomRight, roles){
        if(roles === 'remove'){
            for(let i = leftTop; i < bottomRight; i++){
                if(this.$items[i]){
                    let obj = this.$items[i]
                    delete this.$items[i]
                    obj.destroy()
                }
            }
            for(let k = bottomRight; k < this.$items.length.get() + (bottomRight - leftTop); k++){
                if(this.$items[k]){
                    this.$items[k-(bottomRight - leftTop)] = this.$items[k]
                    delete this.$items[k]
                }
            }
        } 
        else if(roles === 'move'){
            for(let i = leftTop; i < bottomRight; i++){
                let from = i
                let to = this.$items[i].index

                let fromItem = this.$items[from]
                let toItem = this.$items[to]

                let toModel = this.getPropertyValue('model').getPropertyValue('data')[from]
                let fromModel = this.getPropertyValue('model').getPropertyValue('data')[to]

                fromItem.model_ = toModel
                if(fromItem.model instanceof QModelData) fromItem.model = toModel
                toItem.model_ = fromModel
                if(toItem.model instanceof QModelData) toItem.model = fromModel
            }
        }
    }
    $disconnectModel(){
        if(this.$model && this.$model instanceof ListModel && this.$model.UID){
            this.$model.getProperty('data').getNotify().disconnect(this, this.$modelDataChanged)
        }
    }
    $connectModel(model){
        if(model && model instanceof ListModel){
            this.$model = model
            model.getProperty('data').getNotify().connect(this, this.$modelDataChanged)
        }
    }

    $modelChanged(){
        this.$disconnectModel()
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].destroy()
                delete this.$items[key]
            }
        }
        if(!this.UID) return
        let model = this.getPropertyValue('model')
        this.$items.length.reset(0)
        if(typeof model === 'number'){     
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getProperty('model')); return model})
        } else if(Array.isArray(model)){
            this.getProperty('model').value = model.slice()
            this.$items.length.reset(model.length)
        } else {
            this.$connectModel(model)
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(model.getProperty('data')); return model.getPropertyValue('data').length}) 
        }
        this.$items.length.update()
    }

    $visibleChanged(){

    }

    $delegateChanged(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].destroy()
                delete this.$items[key]
            }
        }
        this.updateView()
    }

    createElement(index){
        if(this.$items[index]) return this.$items[index]
        let ctx = new ContextController(this.delegate.get().$exCtx,this.$exCtx)
        let createObject = this.getStatement('delegate').get().createObject
        let cls = this.getStatement('delegate').get().constructor

        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = createObject ? createObject(this.parent,ctx, {index: index}, false) : new cls(this.parent,ctx, {index: index})

            this.$items[index] = obj
        } else if(Array.isArray(this.getPropertyValue('model'))){
            let obj = createObject ? createObject(this.parent,ctx, {'$modelData': this.getPropertyValue('model')[index], index: index}, false) : new cls(this.parent,ctx, {'$modelData': this.getPropertyValue('model')[index], index: index})

            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = createObject ? createObject(this.parent,ctx, model, false) : new cls(this.parent,ctx, model)

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
        
        this.parent.preventAutoUpdateGeometry = true
        for(let i = 0; i < this.$items.length.get(); i++){
            this.createElement(i)
            if(this.$signals.itemAdded) this.$signals.itemAdded(i, this.$items[i])
        }
        delete this.parent.preventAutoUpdateGeometry
        if(this.parent.updateGeometry) this.parent.updateGeometry()
        this.getProperty('count').reset(this.$items.length.get())
    }


    destroy(){
        this.$disconnectModel()
        this.$items.length.destroy()
        delete this.$items.length

        for(let key in this.$items){
            if(this.$items[key].UID){
                if(this.$signals.itemRemoved) this.$signals.itemRemoved(key, this.$items[key])
                this.$items[key].destroy()
            }
            
            delete this.$items[key]
            
        }
        
        super.destroy()
    }
}

module.exports.Repeater = Repeater