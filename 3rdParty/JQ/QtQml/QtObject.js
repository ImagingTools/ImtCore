const QObject = require("./QObject")
const Property = require("./Property")
const GroupProperty = require("./GroupProperty")
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
        resourcesChanged: {type:Signal, slotName:'onResourcesChanged', args:[]},
        dataChanged: {type:Signal, slotName:'onDataChanged', args:[]},

        'Component.completed': {type:Signal, slotName:'Component.onCompleted', args:[]},
        'Component.destruction': {type:Signal, slotName:'Component.onDestruction', args:[]},

        JQAbstractModel: {type:Var, value:undefined, signalName:'JQAbstractModelChanged'},
        JQAbstractModelChanged: {type:Signal, slotName:'onJQAbstractModelChanged', args:[]},
    })  

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)

        proxy.JQAbstractModel = QtFunctions.binding(()=>{return proxy.parent ? proxy.parent.JQAbstractModel : null})

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

    onChildrenChanged(leftTop, bottonRight, roles){
        if(roles === 'append'){
            for(let i = leftTop; i < bottonRight; i++){
                this.children[i].setParent(this)
            }
        } else {
            for(let i = 0; i < this.children.length; i++){
                this.children[i].setParent(this)
            }
        }
    }

    onResourcesChanged(leftTop, bottonRight, roles){
        if(roles === 'append'){
            for(let i = leftTop; i < bottonRight; i++){
                this.resources[i].setParent(this)
            }
        } else {
            for(let i = 0; i < this.resources.length; i++){
                this.resources[i].setParent(this)
            }
        }
    }

    onDataChanged(leftTop, bottonRight, roles){
        if(roles === 'append'){
            for(let i = leftTop; i < bottonRight; i++){
                this.data[i].setParent(this)
            }
        } else {
            for(let i = 0; i < this.data.length; i++){
                this.data[i].setParent(this)
            }
        }
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

    __destroy(){
        super.__destroy()
        this.blockSignals(true)

        let data = this.data.slice()
        for(let i = data.length-1; i >= 0; i--){
            data[i].__destroy()
        }

        this['Component.destruction']()

        if(this.parent) this.parent.__removeChild(this)

        let self = this.__self
        delete self.__proxy
        for(let key in self){
            if(self[key] instanceof Property || self[key] instanceof GroupProperty || self[key] instanceof Signal){
                self[key].__destroy()
            }
            delete self[key]
        }

        self.__destroyed = true
    }
}

module.exports = QtObject