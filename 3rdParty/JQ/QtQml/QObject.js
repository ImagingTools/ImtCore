const Property = require("./Property")
const GroupProperty = require("./GroupProperty")
const BaseObject = require("../QtBase/BaseObject")
const Var = require("../QtQml/Var")
const String = require("./String")
const Signal = require("./Signal")

class QObject extends BaseObject {
    static meta = {
        parent: {type:Var, signalName:'parentChanged'},
        objectName: {type:String, value:''},

        parentChanged: {type:Signal, slotName:'onParentChanged', args:[]},
    }

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.setParent(parent)

        return proxy
    }

    __toPrimitive(hint){
        return this
    }
    
    __has(key){
        if(key in this){
            return true
        } else if(key in this.constructor.meta){
            let meta = this.constructor.meta[key]
            this[key] = meta.type.create(this.__proxy, meta)

            return true
        }

        return false
    }

    __get(key){
        if(this.__has(key)){
            if(this[key] instanceof Property){
                let caller = Property.queueLink[Property.queueLink.length-1]
                if(caller) caller.__subscribe(this[key])
                return this[key].__get()
            }
            return this[key]
        }
        return undefined
    }

    __set(key, value){
        if(this.__has(key)){
            if(this[key] instanceof Property){
                this[key].__self.__reset(value)
                return true
            } else if(this[key] instanceof GroupProperty){
                this[key].__self.__assign(value)
                return true
            }
        }
        this[key] = value
        return true
    }

    __update(sender, options){
        if(!this.__updating){
            this.__onUpdate(sender, options)
        } else {
            this.__updateList.push({
                sender: sender,
                options: options,
            })
        }
    }

    __updateParent(options){
        if(this.parent){
            this.parent.__update(this, options)
        }
    }

    __onUpdate(sender, options){

    }

    __onUpdateAll(list){

    }

    __beginUpdate(){
        this.__updating = true
        this.__updateList = []
    }
    __endUpdate(){
        delete this.__updating
        this.__onUpdateAll(this.__updateList)
        delete this.__updateList
    }

    __beginUpdateParent(){
        if(this.parent){
            this.parent.__beginUpdate()
        }
    }

    __endUpdateParent(options){
        if(this.parent){
            this.parent.__endUpdate()
        }
    }

    setParent(parent){
        this.parent = parent
    }
}

module.exports = QObject