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

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('Repeater')

        return proxy
    }

    __items = []

    __complete(){
        this.__initView(true)
        super.__complete()
    }

    itemAt(index){

    }

    onModelChanged(){
        this.__clear()
        
        if(this.__model && typeof this.__model === 'object' && !this.__model.__destroyed){
            this.__model.__removeViewListener(this)
        }

        if(this.model && typeof this.model === 'object'){
            this.model.__addViewListener(this)
            this.__model = this.model
        }

        this.__initView(this.__completed)
    }

    onDelegateChanged(){
        this.__clear()
        this.__initView(this.__completed)
    }

    __clear(){
        let removed = this.__items
        this.__items = []

        for(let r of removed){
            this.itemRemoved(r)
            if(r) r.destroy()
        }
    }

    __createItem(model){
        let item = this.delegate.createObject(this.parent, model)

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
                    this.__getObject('count').__value = this.model
                }

                for(let i = 0; i < this.model; i++){
                    let item = this.__createItem({index: i})
                    this.__items.push(item)
                    this.itemAdded(item)
                }
            } else {
                if(this.count !== this.model.data.length){
                    countChanged = true
                    this.__getObject('count').__value = this.model.data.length
                }

                for(let i = 0; i < this.model.data.length; i++){
                    let item = this.__createItem(this.model.data[i])
                    this.__items.push(item)
                    this.itemAdded(item)
                }
            }

            if(countChanged) this.countChanged()

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
                    this.__getObject('count').__value = this.model.data.length
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

                if(countChanged) this.countChanged()
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