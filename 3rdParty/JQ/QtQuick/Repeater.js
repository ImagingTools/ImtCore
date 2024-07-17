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

    itemAt(index){

    }
}

module.exports = Repeater