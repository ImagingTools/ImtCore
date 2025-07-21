const QQmlComponent = require("./QQmlComponent");
const JQContext = require("../core/JQContext");

class Component extends QQmlComponent {
    static create(parent = null, context = {}, component = null){
        let obj = super.create(parent, {})

        obj.__context = context
        obj.__component = component
        return obj
    } 

    __context = {}

    createObject(parent = null, properties = {}, forceUpdate = false){
        if(!forceUpdate){
            let obj = this.__component.create(parent, properties, this.__context, false)
            obj.__updateAliases()
            obj.__updateSimpleProperties()
            JQApplication.initLater(obj)
            return obj
        } else {
            return this.__component.create(parent, properties, this.__context)
        }
    }

    __destroy(){
        if(this.__destroyed || !this.__destroying) return

        JQApplication.MemoryController.delete(this)
        super.__destroy()

        for(let key in this){

            delete this[key]
        }

        this.__destroyed = true
    }
}



module.exports = Component