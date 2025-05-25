const Item = require("./Item")
const Component = require("../QtQml/Component")
const Variant = require("../QtQml/Variant")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")
const JQApplication = require("../core/JQApplication")

class Repeater extends Item {
    static meta = Object.assign({}, Item.meta, {
        model: {type: Var, value:undefined, signalName:'modelChanged'},
        delegate: {type:Variant, typeTarget:Component, value:undefined, signalName:'delegateChanged'},
        count: {type: Real, value:0, signalName:'countChanged'},
        
        modelChanged: {type:Signal, slotName:'onModelChanged', args:[]},
        delegateChanged: {type:Signal, slotName:'onDelegateChanged', args:[]},
        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},

        itemAdded: {type:Signal, slotName:'onItemAdded', args:['item']},
        itemRemoved: {type:Signal, slotName:'onItemRemoved', args:['item']},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('Repeater')

        return obj
    }

    __items = []

    __complete(){
        this.__initView(true)
        super.__complete()
    }

    itemAt(index){

    }

    SLOT_modelChanged(oldValue, newValue){
        this.__clear()
        
        if(oldValue && typeof oldValue === 'object' && !oldValue.__destroyed){
            oldValue.__removeViewListener(this)
        }

        if(newValue && typeof newValue === 'object'){
            newValue.__addViewListener(this)
        }

        this.__initView(this.__completed)
    }

    SLOT_delegateChanged(oldValue, newValue){
        this.__clear()
        this.__initView(this.__completed)
    }

    __clear(){
        let removed = this.__items
        this.__items = []

        for(let r of removed){
            this.__proxy.itemRemoved(r)
            if(r) r.destroy()
        }
    }

    __createItem(model){
        let item = this.delegate.createObject(this.parent, {model:model}, true)

        return item
    }

    __initView(isCompleted){
        if(this.delegate && this.model && isCompleted){
            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            let countChanged = false

            if(typeof this.model === 'number'){
                if(this.count !== this.model){
                    countChanged = true
                    this.__self.count = this.model
                    this.count = this.model
                }

                for(let i = 0; i < this.model; i++){
                    let item = this.__createItem({index: i})
                    this.__items.push(item)
                    this.__proxy.itemAdded(item)
                }
            } else {
                if(this.count !== this.model.data.length){
                    countChanged = true
                    this.__self.count = this.model.data.length
                    this.count = this.model.data.length
                }

                for(let i = 0; i < this.model.data.length; i++){
                    let item = this.__createItem(this.model.data[i])
                    this.__items.push(item)
                    this.__proxy.itemAdded(item)
                }
            }

            if(countChanged) this.__proxy.countChanged()

            JQApplication.endUpdate()
        }
    }

    __updateView(changeSet){
        if(this.delegate && this.model && this.__completed){
            JQApplication.beginUpdate()
            JQApplication.updateLater(this.parent)

            if(this.model.data.length === this.__items.length) {
                for(let i = 0; i < this.model.data.length; i++){
                    this.__items[i].JQAbstractModel = this.model.data[i]
                }
            } else {
                let countChanged = false

                if(this.count !== this.model.data.length){
                    countChanged = true
                    this.__self.count = this.model.data.length
                }

                for(let change of changeSet){
                    let leftTop = change[0]
                    let bottomRight = change[1]
                    let role = change[2]

                    if(role === 'append'){
                        for(let i = leftTop; i < bottomRight; i++){
                            let item = this.__createItem(this.model.data[i])
                            this.__items[i] = item
                            this.itemAdded(item)
                        }
                    } else if(role === 'insert'){
                        for(let i = leftTop; i < bottomRight; i++){
                            let item = this.__createItem(this.model.data[i])
                            this.__items.splice(i, 0, item)
                            this.itemAdded(item)
                        }
                    } else if(role === 'remove'){
                        let removed = this.__items.splice(leftTop, bottomRight - leftTop)
                        for(let r of removed){
                            this.itemRemoved(r)
                            if(r) r.destroy()
                        }
                    }
                }

                if(countChanged) this.__proxy.countChanged()
            }

            JQApplication.endUpdate()
        }
    }

    __destroy(){
        if(this.__model && typeof this.__model === 'object' && !this.__model.__destroyed){
            this.__model.__removeViewListener(this)
        }
        this.__clear()
        super.__destroy()
    }
}



module.exports = Repeater