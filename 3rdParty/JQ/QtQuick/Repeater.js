const Item = require("./Item")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class Repeater extends Item {
    static meta = Object.assign({}, Item.meta, {
        model: {type: Var, value:undefined, signalName:'modelChanged'},
        delegate: {type: Var, value:undefined, signalName:'delegateChanged'},
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
        this.__updateView()
    }

    onDelegateChanged(){
        this.__updateView()
    }

    __updateView(){
        if(this.delegate && this.model){
            this.__beginUpdateParent()
            for(let i = 0; i < this.model; i++){
                let item = this.delegate.createObject(this.parent)
                this.__items.push(item)
                item.__complete()
                this.itemAdded(item)
            }
            this.__endUpdateParent()
        }
    }
}

module.exports = Repeater