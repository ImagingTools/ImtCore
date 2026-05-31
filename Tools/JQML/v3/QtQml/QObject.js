const QBaseObject = require("../QtBase/QBaseObject")
const Var = require("../QtQml/Var")
const String = require("./String")
const Signal = require("./Signal")
const Alias = require("./Alias")

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

    // Replaces every character that is not safe for a DOM attribute value or a
    // CSS selector with an underscore so generated ids stay selector friendly.
    static __sanitizeQmlKey(key){
        return ('' + key).replace(/[^A-Za-z0-9_-]/g, '_')
    }

    // Builds a deterministic, stable and unique identifier for this object and
    // (for visual objects) writes it onto the DOM node as `data-qml-id`.
    // localKey is provided by the compiler and is derived from the QML `id:` or
    // the structural `<Type>_<__elN>` key, both deterministic per source file.
    // The full id is the parent object id joined with the local key, so the same
    // component reused in different places still gets a unique, regeneration
    // stable id.
    __applyQmlId(localKey, explicit){
        // First (inner-most) assignment wins. For a reused component the
        // component's own root assigns the id before its children are created,
        // so the whole subtree shares a single consistent path prefix and the
        // redundant call emitted at the instantiation site becomes a no-op.
        if(this.__qmlIdAssigned) return this.__qmlId

        let key = QObject.__sanitizeQmlKey(localKey)
        let explicitName = explicit ? key : ''
        let parent = this.parent

        // Dynamic delegates (Repeater/ListView/GridView) instantiate the same
        // component many times under one parent. Prefer the model index so the
        // id is deterministic across regenerations and re-renders.
        let index
        if(this.__properties && this.__properties.__qmlIndex !== undefined){
            index = this.__properties.__qmlIndex
            delete this.__properties.__qmlIndex
        }

        // Index ('.') and occurrence-counter ('~') use disjoint separators so a
        // counted duplicate can never collide with an indexed delegate id.
        if(index !== undefined){
            key = key + '.' + index
        } else if(parent){
            if(!parent.__qmlChildKeys) parent.__qmlChildKeys = {}
            if(key in parent.__qmlChildKeys){
                parent.__qmlChildKeys[key]++
                key = key + '~' + parent.__qmlChildKeys[key]
            } else {
                parent.__qmlChildKeys[key] = 0
            }
        }

        let parentPath = (parent && parent.__qmlId) ? parent.__qmlId : ''
        this.__qmlId = parentPath ? parentPath + '/' + key : key
        this.__qmlName = explicitName
        this.__qmlIdAssigned = true

        if(typeof this.__applyQmlIdToDOM === 'function') this.__applyQmlIdToDOM()

        return this.__qmlId
    }

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
    __aliases = []

    hasOwnProperty(key){
		return this.__self.constructor.meta[key] || super.hasOwnProperty(key)
	}

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
        for(let child of this.__children){
            child.__updatePrimaryProperties()
        }
    }

    __prepareAliasProperties(){
        for(let name of this.__aliases){
            let obj = this.__self[name].func()
            Alias.prepare(this, name, obj, this.__self[name].propName)
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

        this.__prepareAliasProperties()

        // Process own bindings FIRST so they're registered before children compute layout
        for(let propName in this.__properties){
            this.__updateProperty(propName)
        } 

        for(let child of this.__children){
            child.__updateProperties()
        }

        JQApplication.endUpdate()
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