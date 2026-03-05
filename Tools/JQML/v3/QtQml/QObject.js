const QBaseObject = require("../QtBase/QBaseObject")
const Var = require("../QtQml/Var")
const String = require("./String")
const Signal = require("./Signal")

class QObject extends QBaseObject {
    static meta = {
        parent: {type:Var, },
        objectName: {type:String, value:''},

        parentChanged: {type:Signal, args:[]},
        objectNameChanged: {type:Signal, args:[]},

        JQDestruction: {type:Signal, args:[]},
    }
    
    __addObjectName(){}
    __removeObjectName(){}

    static create(parent = null, properties = {}){
        global.objectsCount++
        let obj = super.create(parent, properties)
        obj.__addObjectName()

        if(!parent && !this.singleton) {
            obj.__smart = true
            obj.__links = 0
        }
 
        if(parent) {
            if(parent instanceof JQModules.QtQuick.Flickable){
                if(parent.__children.length === 0){
                    parent.__children.push(obj)
                    obj.setParent(parent)
                } else {
                    parent.contentItem.__children.push(obj)

                    obj.JQDestruction.connect(()=>{
                        let index = parent.contentItem.__children.indexOf(obj)
                        if(index >= 0) parent.contentItem.__children.splice(index, 1)
                    })

                    obj.setParent(parent.contentItem)
                }
            } else {
                parent.__children.push(obj)

                obj.JQDestruction.connect(()=>{
                    let index = parent.__children.indexOf(obj)
                    if(index >= 0) parent.__children.splice(index, 1)
                })

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

    setParent(parent){
        this.__proxy.parent = parent
    }

    __init(){
        if(this.__dynamic){
            delete this.__dynamic
            this.__updatePrimaryProperties()
            this.__updateProperties()
            this.__complete()
            this.__completeProperties()
        }
    }

    __addLink(){
        if(this.__smart){
            this.__links++
        }
    }

    __removeLink(){
        if(this.__smart){
            this.__links--
            if(this.__links === 0){
                this.destroy()
            }
        }
    }

    __updatePrimaryProperties(){
        for(let i = this.__children.length-1; i >= 0; i--){
            this.__children[i].__updatePrimaryProperties()
        }
    }


    __updateProperty(propName){
        if(!(propName in this.__properties)) return

        let path = propName.split('.')
        let value = this.__properties[propName]

        if(value === 'JQObject'){
            return
        }

        delete this.__properties[propName]

        if(value instanceof QObject){
            value.__updatePrimaryProperties()
            value.__updateProperties()
            value.__complete()
            value.__completeProperties()
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

        for(let i = this.__children.length-1; i >= 0; i--){
            this.__children[i].__updateProperties()
        }

        JQApplication.endUpdate()

        for(let propName in this.__properties){
            this.__updateProperty(propName)
        } 
    }

    __completeProperty(propName){
        let path = propName.split('.')
        let value = this.__properties[propName]

        if(value === 'JQObject'){
            delete this.__properties[propName]

            let obj = this.__proxy[path[0]]
            obj.__updatePrimaryProperties()
            obj.__updateProperties()
            obj.__complete()
            obj.__completeProperties()
        }
    }

    __completeProperties(){
        for(let propName in this.__properties){
            this.__completeProperty(propName)
        } 

        for(let child of this.__children){
            child.__completeProperties()
        }
    }

    __destroy(){
        global.objectsCount--
        this.__removeObjectName()
        super.__destroy()
        this.JQDestruction()
    }

    // for compatibility reasons, this will be removed in the future
    $complete(){

    }
}



module.exports = QObject