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

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
 
        if(parent) {
            parent.__children.push(obj)
            obj.setParent(parent.__resolve())
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
        this.parent = parent
    }
  
}

QObject.initialize()

module.exports = QObject