const QQmlComponent = require("./QQmlComponent");
const JQContext = require("../core/JQContext");

class Component extends QQmlComponent {
    static create(parent = null, context = {}, component = null){
        let obj = super.create(parent, {})

        // Implicit components are compiled as Component.create(null, ...).
        // That marks them __smart, so the last Loader/Repeater __removeLink
        // destroys the factory while the owner still holds it in a property
        // or context id. A Component is a factory: keep it until the parent
        // is destroyed or destroy() is called explicitly.
        if(obj.__smart){
            obj.__smart = false
        }

        // Non-enumerable so Component.__destroy's for-in delete does not
        // wipe the factory class. createObject must keep working if the
        // engine destroyed a still-referenced Component.
        Object.defineProperty(obj.__self, '__component', {
            value: component,
            writable: true,
            configurable: true,
            enumerable: false
        })
        Object.defineProperty(obj.__self, '__context', {
            value: context,
            writable: true,
            configurable: true,
            enumerable: false
        })
        return obj
    } 

    createObject(parent = null, properties = {}, forceUpdate = false){
        let factory = this.__component
        if(!factory){
            console.error('Component.createObject: __component is undefined')
            return null
        }
        let obj = factory.create(parent, properties, this.__context)
        if(obj){
            let self = this.__self
            if(!self.__objects){
                Object.defineProperty(self, '__objects', {
                    value: [],
                    writable: true,
                    configurable: true,
                    enumerable: false
                })
            }
            self.__objects.push(obj)
            Object.defineProperty(obj.__self, '__incubator', {
                value: self,
                writable: true,
                configurable: true,
                enumerable: false
            })
        }
        return obj
    }

    __destroy(){
        if(this.__destroyed || !this.__destroying) return

        // createObject() instances are often parented elsewhere (overlay,
        // ListView contentItem). Destroy them with this factory when the
        // owner is going away so leftover DOM cannot stay on screen.
        let parent = this.parent
        let ownerDying = !parent || parent.__destroying || parent.__destroyed
        let incubated = this.__self && this.__self.__objects
        if(ownerDying && incubated && incubated.length){
            this.__self.__objects = []
            for(let i = incubated.length - 1; i >= 0; i--){
                let inst = incubated[i]
                if(inst && !inst.__destroyed) inst.destroy()
            }
        }

        JQApplication.MemoryController.delete(this)
        super.__destroy()

        let self = this.__self
        let factory = self.__component
        let context = self.__context

        for(let key in this){
            delete this[key]
        }

        if(self && factory){
            Object.defineProperty(self, '__component', {
                value: factory,
                writable: true,
                configurable: true,
                enumerable: false
            })
        }
        if(self && context){
            Object.defineProperty(self, '__context', {
                value: context,
                writable: true,
                configurable: true,
                enumerable: false
            })
        }

        this.__destroyed = true
    }
}

module.exports = Component