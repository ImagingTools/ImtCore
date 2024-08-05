const QObject = require("./QObject")
const Var = require("./Var")
const Int = require("./Int")
const List = require("./List")
const Signal = require("./Signal")

class QtObject extends QObject {
    static meta = Object.assign({}, QObject.meta, {
        model: {type:Var, value:undefined, signalName:'modelChanged'},
        index: {type:Int, value:0, signalName:'indexChanged'},
        children: {type:List, signalName:'childrenChanged'},
        resources: {type:List, signalName:'resourcesChanged'},
        data: {type:List, signalName:'dataChanged'},
        
        modelChanged: {type:Signal, slotName:'onModelChanged', args:[]},
        childrenChanged: {type:Signal, slotName:'onChildrenChanged', args:[]},
        resourcesChanged: {type:Signal, slotName:'onresourcesChanged', args:[]},
        dataChanged: {type:Signal, slotName:'onDataChanged', args:[]},

        'Component.completed': {type:Signal, slotName:'Component.onCompleted', args:[]},
        'Component.destruction': {type:Signal, slotName:'Component.onDestruction', args:[]},
    })  

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)

        if(model){
            proxy.model = model
        }

        if(parent){
            proxy.parent.children.push(proxy)
        }

        return proxy
    }

    __complete(){
        if(this.__completed) return

        this.__completed = true
        for(let i = this.children.length-1; i >= 0; i--){
            this.children[i].__complete()
        }
        
        this['Component.completed']()
    }
}

module.exports = QtObject