const QBaseObject = require("../QtBase/QBaseObject")
const Var = require("../QtQml/Var")
const String = require("./String")
const Signal = require("./Signal")

class QObject extends QBaseObject {
    static meta = {
        parent: {type:Var, signalName:'parentChanged'},
        objectName: {type:String, value:''},

        parentChanged: {type:Signal, slotName:'onParentChanged', args:[]},
    }

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
 
        if(parent) {
            if(parent instanceof JQModules.QtQuick.Flickable){
                if(parent.__children.length === 0){
                    parent.__children.push(obj)
                    obj.setParent(parent)
                } else {
                    parent.contentItem.__children.push(obj)
                    obj.setParent(parent.contentItem)
                }
            } else {
                parent.__children.push(obj)
                obj.setParent(parent)
            }
            
        } else {
            try {
                if(!this.singleton) JQApplication.MemoryController.observe(obj)
            } catch (error) {
                
            } 
        }

        return obj
    }

    __children = []
    __simpleProperties = []
    __aliases = []

    setParent(parent){
        this.__proxy.parent = parent
    }

    __init(){
        if(this.__dynamic){
            delete this.__dynamic
            this.__updateAliases()
            this.__updateSimpleProperties()
            this.__updateProperties()
            this.__complete()
        }
    }

    __updateAliases(){
        if(!this.__aliases) return
        
        for(let func of this.__aliases){
            func.call(this)
        }

        delete this.__aliases

        for(let child of this.__children){
            child.__updateAliases()
        }
    }

    __updateSimpleProperties(){
        for(let child of this.__children){
            child.__updateSimpleProperties()
        }

        if(!this.__simpleProperties) return
        
        for(let func of this.__simpleProperties){
            func.call(this)
        }

        delete this.__simpleProperties
    }

    __updateProperty(propName){
        let path = propName.split('.')
        let value = this.__properties[propName]

        delete this.__properties[propName]

        if(value instanceof QObject){
            value.__updateAliases()
            value.__updateSimpleProperties()
            value.__updateProperties()
            value.__complete()
        }

        if(path.length === 2){
            this.__proxy[path[0]][path[1]] = value
        } else {
            if(value === 'JQGroup'){
                this.__proxy[path[0]].__updateProperties()
            } else {
                this.__proxy[path[0]] = value
            }
        }
    }
  
    __updateProperties(){
        JQApplication.beginUpdate()

        for(let child of this.__children){
            child.__updateProperties()
        }

        JQApplication.endUpdate()

        for(let propName in this.__properties){
            this.__updateProperty(propName)
        } 
    }
}



module.exports = QObject