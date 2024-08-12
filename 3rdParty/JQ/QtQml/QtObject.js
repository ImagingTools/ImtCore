const QObject = require("./QObject")
const Var = require("./Var")
const Int = require("./Int")
const List = require("./List")
const Signal = require("./Signal")
const QtFunctions = require("../Qt/functions")

class QtObject extends QObject {
    static meta = Object.assign({}, QObject.meta, {
        model: {type:Var, auto: true, value:undefined, signalName:'modelChanged'},
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

        JQAbstractModel: {type:Var, value:undefined, signalName:'JQAbstractModelChanged'},
        JQAbstractModelChanged: {type:Signal, slotName:'onJQAbstractModelChanged', args:[]},
    })  

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)

        proxy.JQAbstractModel = QtFunctions.binding(()=>{return proxy.parent.JQAbstractModel})

        if(model){
            proxy.JQAbstractModel = model
        }

        return proxy
    }

    __complete(){
        if(this.__completed) return

        this.__completed = true
        for(let i = this.data.length-1; i >= 0; i--){
            this.data[i].__complete()
        }
        
        this['Component.completed']()
    }

    onJQAbstractModelChanged(){
        if(this.__self.constructor.meta.model.auto){
            this.model = this.JQAbstractModel
        }
    }

    __removeChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index >= 0) this.data.splice(index, 1)

        index = this.resources.indexOf(child)
        if(index >= 0) this.resources.splice(index, 1)
    }

    __addChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index < 0) this.data.push(child)

        index = this.resources.indexOf(child)
        if(index < 0) this.resources.push(child)
    }

    onParentChanged(){
        if(this.__parent){
            this.__parent.__removeChild(this)
        }

        this.__parent = this.parent

        if(this.__parent) {
            this.__parent.__addChild(this)
        }
    }
}

module.exports = QtObject