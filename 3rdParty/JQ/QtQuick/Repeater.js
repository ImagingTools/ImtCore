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

    itemAt(index){

    }

    onModelChanged(){
        this.__clear()
        
        if(typeof this.model === 'object'){
            this.model.__addViewListener(this)
        }

        this.__updateView()
    }

    onDelegateChanged(){
        this.__clear()
        this.__updateView()
    }

    __clear(){
        for(let i = 0; i < this.__items.length; i++){
            this.__items[i].__destroy()
        }
        this.__items = []
    }

    __updateView(changeSet){
        if(this.delegate && this.model){
            JQApplication.beginUpdate()
            JQApplication.updateLater(this.parent)

            let countChanged = false

            if(typeof this.model === 'number'){
                if(this.count !== this.model){
                    countChanged = true
                    this.__getObject('count').__value = this.model
                }
                
                for(let i = 0; i < this.model; i++){
                    let item = this.delegate.createObject(this.parent, {index: i})
                    this.__items.push(item)
                    // item.__complete()
                    this.itemAdded(item)
                }
            } else {
                if(this.count !== this.model.data.length){
                    countChanged = true
                    this.__getObject('count').__value = this.model.data.length
                }

                for(let i = 0; i < this.model.data.length; i++){
                    let item = this.delegate.createObject(this.parent, this.model.data[i])
                    this.__items.push(item)
                    // item.__complete()
                    this.itemAdded(item)
                }
            }

            if(countChanged) this.countChanged()

            JQApplication.endUpdate()
        }
    }
}

module.exports = Repeater