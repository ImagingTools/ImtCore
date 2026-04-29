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

        this.$items = []

        this.setStyle({
            maxWidth: '0px',
            maxHeight: '0px',
        })
    }

    itemAt(index){
        return index >= 0 && index < this.$items.length ? this.$items[index] : undefined
    }

    $disconnectModel(){
        if(this.$model && this.$model instanceof ListModel && this.$model.UID){
            this.$model.$removeListener(this)
        }
    }
    $connectModel(model){
        if(model && model instanceof ListModel){
            this.$model = model
            model.$addListener(this)
        }
    }

    $clear(){
        let _items = this.$items
        this.$items = []
        for(let i = 0; i < _items.length; i++){
            _items[i].destroy()
            if(this.$signals.itemRemoved) this.$signals.itemRemoved(i, _items[i])
        }
    }

    $modelChanged(){
        this.$disconnectModel()
        this.$clear()
        if(!this.UID) return
        let model = this.getPropertyValue('model')

        if(typeof model === 'number'){     

        } else if(Array.isArray(model)){
            this.getProperty('model').value = model.slice()
        } else {
            this.$connectModel(model)
        }

        this.updateView()
    }

    $visibleChanged(){

    }

    $delegateChanged(){
        this.$clear()
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

    updateView(topLeft, bottomRight, roles){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return
        let model = this.getPropertyValue('model')
        let length = 0 

        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(Array.isArray(model)){
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        }

        let countChanged = this.getPropertyValue('count') !== length
        this.getProperty('count').value = length
        
        this.parent.preventAutoUpdateGeometry = true

        if(roles === 'append'){
            for(let i = topLeft; i < bottomRight; i++){
                let item = this.createElement(i)
                if(this.$signals.itemAdded) this.$signals.itemAdded(i, item)
            }
        } else if(roles === 'insert'){
            for(let i = topLeft; i < bottomRight; i++){
                this.$items.splice(i, 0, null)
                let item = this.createElement(i)
                if(this.$signals.itemAdded) this.$signals.itemAdded(i, item)
            }
        } else if(roles === 'remove'){
            for(let i = topLeft; i < bottomRight; i++){
                let item = this.$items[i]
                this.$items.splice(i, 1)
                if(item){
                    item.destroy()
                    if(this.$signals.itemRemoved) this.$signals.itemRemoved(i, item)
                }
                
            }
        } else if(roles === 'move'){
            let from = topLeft[0]
            let to = topLeft[1]
            let n = bottomRight
            for(let i = 0; i < n; i++){
                let temp = this.$items[from + i]
                this.$items[from + i] = this.$items[to + i]
                this.$items[to + i] = temp
            }
        } else {
            for(let i = 0; i < length; i++){
                if(!this.$items[i]){
                    this.createElement(i)
                    if(this.$signals.itemAdded) this.$signals.itemAdded(i, this.$items[i])
                }
            }
        }

        delete this.parent.preventAutoUpdateGeometry
        if(this.parent.updateGeometry) this.parent.updateGeometry()

        if(countChanged && this.getProperty('count').notify){
            this.getProperty('count').notify()
        }
    }


    destroy(){
        this.$disconnectModel()
        this.$clear()
        
        super.destroy()
    }
}

module.exports.Repeater = Repeater