const Item = require("./Item")
const Component = require("../QtQml/Component")
const Var = require("../QtQml/Var")
const Variant = require("../QtQml/Variant")
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
        sourceComponent: {type:Variant, typeTarget:Component, value:undefined, signalName:'sourceComponentChanged'},
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
        if(this.item) this.item.__destroy()
        if(!this.sourceComponent) return

        let item = this.sourceComponent.createObject(this)
        this.item = item

        if(this.__getObject('width').__auto) {
            this.__getObject('width').__setCompute(()=>{return this.item.width})
            this.__getObject('width').__update()
        }
        if(this.__getObject('height').__auto) {
            this.__getObject('height').__setCompute(()=>{return this.item.height})
            this.__getObject('height').__update()
        }

        this.loaded()
    }
}

module.exports = Loader