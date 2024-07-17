const QObject = require("./QObject")
const Var = require("./Var")
const Int = require("./Int")
const Signal = require("./Signal")

class QtObject extends QObject {
    static meta = Object.assign({}, QObject.meta, {
        model: {type:Var, value:undefined, signalName:'modelChanged'},
        index: {type:Int, value:0, signalName:'indexChanged'},
        children: {type:Var, value:undefined, signalName:'childrenChanged'},
        resources: {type:Var, value:undefined, signalName:'resourcesChanged'},
        data: {type:Var, value:undefined, signalName:'dataChanged'},
        
        modelChanged: {type:Signal, slotName:'onModelChanged', args:[]},
        childrenChanged: {type:Signal, slotName:'onChildrenChanged', args:[]},
        resourcesChanged: {type:Signal, slotName:'onresourcesChanged', args:[]},
        dataChanged: {type:Signal, slotName:'onDataChanged', args:[]},

        'Component.completed': {type:Signal, slotName:'Component.onCompleted', args:[]},
        'Component.destruction': {type:Signal, slotName:'Component.onDestruction', args:[]},
    })  
}

module.exports = QtObject