const Item = require("./Item")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class Loader extends Item {
    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    static meta = Object.assign({}, Item.meta, {
        item: {type:Var, value:undefined, signalName:'itemChanged'},
        source: {type:String, value:'', signalName:'sourceChanged'},
        sourceComponent: {type:Var, value:undefined, signalName:'sourceComponentChanged'},
        status: {type:Real, value:Loader.Null, signalName:'statusChanged'},
        progress: {type:Real, value:0, signalName:'progressChanged'},
        asynchronous: {type:Bool, value:false, signalName:'asynchronousChanged'},

        itemChanged: {type:Signal, slotName:'onItemChanged', args:[]},
        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
        sourceComponentChanged: {type:Signal, slotName:'onSourceComponentChanged', args:[]},
        statusChanged: {type:Signal, slotName:'onStatusChanged', args:[]},
        progressChanged: {type:Signal, slotName:'onProgressChanged', args:[]},
        asynchronousChanged: {type:Signal, slotName:'onAsynchronousChanged', args:[]},

        loaded: {type:Signal, slotName:'onLoaded', args:[]},
    })

    onSourceComponentChanged(){
        let component = this.sourceComponent
        let item = component.createObject(this)
        this.item = item
    }
}

module.exports = Loader