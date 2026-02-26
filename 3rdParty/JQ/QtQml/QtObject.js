const QObject = require("./QObject")
const PropertyAuto = require("./PropertyAuto")
const Var = require("./Var")
const Int = require("./Int")
const List = require("./List")
const ProxyProperty = require("./ProxyProperty")
const Signal = require("./Signal")
const QtFunctions = require("../Qt/functions")

class QtObject extends QObject {
    static meta = Object.assign({}, QObject.meta, {
        model: {type:Var, auto: true, value:undefined, },
        modelData: { type: Var, auto: true, value: undefined },
        index: {type:ProxyProperty, },
        children: {type:ProxyProperty, },
        resources: {type:ProxyProperty, },
        data: {type:List, },
        
        modelChanged: {type:Signal, args:[]},
        childrenChanged: {type:Signal, args:[]},
        resourcesChanged: {type:Signal, args:[]},
        dataChanged: {type:Signal, args:[]},
        indexChanged: {type:Signal, args:[]},

        'Component.completed': {type:Signal, args:[]},
        'Component.destruction': {type:Signal, args:[]},

        JQAbstractModelData: {type:PropertyAuto, value:undefined, },
        JQAbstractModel: {type:PropertyAuto, value:undefined, },

        JQAbstractModelDataChanged: {type:Signal, args:[]},
        JQAbstractModelChanged: {type:Signal, args:[]},
    })  

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        let isModelChanged = properties.model || properties.modelData

        if(properties.model){
            let keys = Object.keys(properties.model)
            if(properties.modelData){
                obj.JQAbstractModelData = properties.modelData
            } else if(keys.length === 1){
                obj.JQAbstractModelData = properties.model[keys[0]]
            }
            obj.JQAbstractModel = properties.model
            delete properties.model
        } else if(!properties.JQAbstractModel){
            // if(!(this.isAssignableFrom(JQModules.QtQuick.Repeater) || this.isAssignableFrom(JQModules.QtQuick.ListView) || 
            //     this.isAssignableFrom(JQModules.QtQuick.GridView) || this.isAssignableFrom(JQModules.QtQuick.MapItemView))){
            //         obj.JQAbstractModel = ()=>{return obj.parent ? obj.parent.JQAbstractModel : undefined}
            //     }
        }

        if(properties.modelData){
            obj.JQAbstractModelData = properties.modelData
            delete properties.modelData
        } else {
            // obj.JQAbstractModelData = ()=>{return obj.parent ? obj.parent.JQAbstractModelData : undefined}
        }

        if(obj.parent && !isModelChanged){
            PropertyAuto.setAuto(obj.__self, 'JQAbstractModel', obj.parent.JQAbstractModel, obj.constructor.meta.JQAbstractModel)
            PropertyAuto.setAuto(obj.__self, 'JQAbstractModelData', obj.parent.JQAbstractModelData, obj.constructor.meta.JQAbstractModelData)
        }

        if(this.meta.modelData.auto){
            // obj.modelData = ()=>{return obj.JQAbstractModelData}
        }
        
        if(this.meta.model.auto){
            // obj.model = ()=>{return obj.JQAbstractModel}
        }

        // obj.index = ()=>{return obj.JQAbstractModel ? obj.JQAbstractModel.index : -1}

        return obj
    }

    PROXY__get__index(){
        return this.JQAbstractModel ? this.JQAbstractModel.index : -1
    }

    PROXY__get__children(){
        return this.__proxy.data.filter(obj => obj instanceof JQModules.QtQuick.Item)
    }
    PROXY__set__children(newValue){
        let oldValue = this.__proxy.children

        for(let child of oldValue){
            if(newValue.indexOf(child) < 0) this.__proxy.__removeChild(child)
        }

        for(let child of newValue){
            child.setParent(this.__proxy)
        }
    }
    PROXY__get__resources(){
        return this.__proxy.data.filter(obj => !(obj instanceof JQModules.QtQuick.Item))
    }
    PROXY__set__resources(newValue){
        let oldValue = this.__proxy.resources

        for(let child of oldValue){
            if(newValue.indexOf(child) < 0) this.__proxy.__removeChild(child)
        }

        for(let child of newValue){
            child.setParent(this.__proxy)
        }
    }


    SLOT_JQAbstractModelChanged(oldValue, newValue){
        if(this.constructor.meta.model.auto){
            this.model = newValue
        }

        // this.index = newValue.index

        for(let child of this.data){
            PropertyAuto.setAuto(child.__self, 'JQAbstractModel', newValue, child.constructor.meta.JQAbstractModel)
            // child.JQAbstractModel = newValue
        }
    }

    SLOT_JQAbstractModelDataChanged(oldValue, newValue){
        if(this.constructor.meta.modelData.auto){
            this.modelData = newValue
        }

        for(let child of this.data){
            PropertyAuto.setAuto(child.__self, 'JQAbstractModelData', newValue, child.constructor.meta.JQAbstractModelData)
            // child.JQAbstractModelData = newValue
        }
    }
    
    __complete(){
        if(this.__completed) return

        this.__completed = true
        this['Component.completed']()

        for(let i = this.__children.length-1; i >= 0; i--){
            this.__children[i].__complete()
        }
    }

    __removeChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index >= 0) this.data.__splice(index, 1)

        // index = this.resources.indexOf(child)
        // if(index >= 0) this.resources.__splice(index, 1)
    }

    __addChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index < 0) this.data.__push(child)

        // index = this.resources.indexOf(child)
        // if(index < 0) this.resources.__push(child)
    }

    // SLOT_childrenChanged(...args){
    //     if(args.length === 3){
    //         let leftTop = args[0]
    //         let bottonRight = args[1]
    //         let roles = args[2]

    //         if(roles === 'append'){
    //             for(let i = leftTop; i < bottonRight; i++){
    //                 this.children[i].setParent(this)
    //             }
    //         }
    //     } else {
    //         let oldValue = args[0]
    //         let newValue = args[1]

    //         for(let child of oldValue){
    //             if(newValue.indexOf(child) < 0) this.__removeChild(child)
    //         }

    //         for(let child of newValue){
    //             child.setParent(this)
    //         }
    //     }
    // }

    // SLOT_resourcesChanged(leftTop, bottonRight, roles){
    //     if(args.length === 3){
    //         let leftTop = args[0]
    //         let bottonRight = args[1]
    //         let roles = args[2]

    //         if(roles === 'append'){
    //             for(let i = leftTop; i < bottonRight; i++){
    //                 this.resources[i].setParent(this)
    //             }
    //         }
    //     } else {
    //         let oldValue = args[0]
    //         let newValue = args[1]

    //         for(let child of oldValue){
    //             if(newValue.indexOf(child) < 0) this.__removeChild(child)
    //         }

    //         for(let child of newValue){
    //             child.setParent(this)
    //         }
    //     }
    // }

    SLOT_dataChanged(leftTop, bottonRight, roles){
        if(args.length === 3){
            let leftTop = args[0]
            let bottonRight = args[1]
            let roles = args[2]

            if(roles === 'append'){
                for(let i = leftTop; i < bottonRight; i++){
                    this.data[i].setParent(this)
                }
            }
        } else {
            let oldValue = args[0]
            let newValue = args[1]

            for(let child of oldValue){
                if(newValue.indexOf(child) < 0) this.__removeChild(child)
            }

            for(let child of newValue){
                child.setParent(this)
            }
        }
    }

    SLOT_parentChanged(oldValue, newValue){
        if(oldValue){
            oldValue.__removeChild(this)
        }
        
        if(newValue) {
            newValue.__addChild(this)

            
            // this.JQAbstractModel = newValue.JQAbstractModel
            // this.JQAbstractModelData = newValue.JQAbstractModelData
        }
    }

    __destroy(){
        if(this.__destroyed || !this.__destroying) return

        JQApplication.MemoryController.delete(this)
        super.__destroy()

        this.blockSignals(true)

        if(this.__children)
        for(let i = this.__children.length-1; i >= 0; i--){
            this.__children[i].destroy()
        }

        this.blockSignals(false)
        this['Component.destruction']()
        this.blockSignals(true)

        if(this.parent && !this.parent.__destroyed) this.parent.__removeChild(this)

        for(let key in this.__depends){
            if(this.__depends[key]){
                for(let connectionObj of this.__depends[key]){
                    Signal.removeConnection(connectionObj)
                }
                delete this.__depends[key]
            }
        }

        

        for(let key in this){
            if(key in this.constructor.meta && this.constructor.meta[key].type !== JQModules.QtQml.alias){
                if(typeof this[key] === 'object'){
                    if(this[key] instanceof QObject){
                        this[key].__removeLink()
                    } else {
                        // if(Array.isArray(this[key])){
                            
                        // } else {

                        // }
                    }
                }
            }
            delete this[key]
        }

        this.__destroyed = true
    }
}



module.exports = QtObject