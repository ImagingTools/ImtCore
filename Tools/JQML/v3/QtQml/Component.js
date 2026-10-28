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

    // Same contract as Qt: the object is completed before return, and the
    // properties map overrides the component defaults before onCompleted.
    // forceUpdate stays in the signature for callers that pass it; completion
    // is synchronous either way. Geometry queued while a QML handler already
    // holds an update layer is flushed with that handler, together with the
    // assignments that follow createObject.
    createObject(parent = null, properties = {}, forceUpdate = false){
        if(!this.__component){
            console.error('Component.createObject: __component is undefined')
            return null
        }

        let initial = {}
        if(properties){
            for(let key in properties){
                initial[key] = properties[key]
            }
        }

        // forceUpdate flushes geometry before return even when a handler
        // already has a layer open (ListView and Loader read the item next).
        let flushNow = forceUpdate && JQApplication.updateLayers.length > 0
        if(flushNow) JQApplication.beginUpdate()

        try {
            let obj = this.__component.create(parent, properties || {}, this.__context, false)
            if(!obj) return null

            if(obj.__properties){
                for(let key in initial){
                    delete obj.__properties[key]
                    obj.__properties[key] = initial[key]
                }
            }

            if(obj.__dynamic) obj.__init()
            else {
                obj.__updatePrimaryProperties()
                obj.__updateProperties()
                obj.__complete()
                obj.__completeProperties()
            }
            return obj
        } finally {
            if(flushNow) JQApplication.endUpdate()
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