const Item = require("./Item")
const Component = require("../QtQml/Component")
const Var = require("../QtQml/Var")
const Variant = require("../QtQml/Variant")
const Real = require("../QtQml/Real")
const Geometry = require("../QtQml/Geometry")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class Loader extends Item {
    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    static meta = Object.assign({}, Item.meta, {
        item: {type:Var, value:undefined, },
        source: {type:String, value:'', },
        sourceComponent: {type:Variant, typeTarget:Component, value:undefined, },
        status: {type:Real, value:Loader.Null, },
        progress: {type:Real, value:0, },
        asynchronous: {type:Bool, value:false, },
        active: {type:Bool, value:true, },

        itemChanged: {type:Signal, args:[]},
        sourceChanged: {type:Signal, args:[]},
        sourceComponentChanged: {type:Signal, args:[]},
        statusChanged: {type:Signal, args:[]},
        progressChanged: {type:Signal, args:[]},
        asynchronousChanged: {type:Signal, args:[]},
        activeChanged: {type:Signal, args:[]},

        loaded: {type:Signal, args:[]},
    })

    __updatePrimaryProperties(){
        super.__updatePrimaryProperties()
        this.__updateProperty('sourceComponent')
    }

    __loadFromCurrentSource(){
        if(!this.active){
            this.item = null
            this.status = Loader.Null
            this.progress = 0
            return
        }

        if(this.sourceComponent){
            this.SLOT_sourceComponentChanged(this.sourceComponent, this.sourceComponent)
            return
        }

        if(this.source){
            this.SLOT_sourceChanged(this.source, this.source)
            return
        }

        this.item = null
        this.status = Loader.Null
        this.progress = 0
    }

    /**
     * Mirrors QtQuick Loader.setSource(url, properties).
     * Clears the current source when called with null/undefined.
     */
    setSource(source, properties){
        if (source === undefined || source === null){
            this.__sourceProperties = {}
            this.source = ''
            this.sourceComponent = undefined
            return
        }

        this.__sourceProperties = properties === undefined || properties === null ? {} : properties
        this.sourceComponent = undefined
        this.source = source
    }

    __complete(){
        super.__complete()
        if(this.__lazyItem){
            this.item = this.__lazyItem
            delete this.__lazyItem
            this.status = Loader.Ready
            this.loaded()
        }
    }

    __updateProperties(){
        super.__updateProperties()
        if(this.__lazyItem){
            this.item = this.__lazyItem
            delete this.__lazyItem
            this.status = Loader.Ready
            this.loaded()
        }
        this.__propertiesUpdated = true
    }

    SLOT_itemChanged(oldValue, newValue){
        if(oldValue) oldValue.destroy()

        if(newValue instanceof Item){
            if(this.__self.width__prevent || this.__self.AWidth__prevent){
                newValue.width = this.width
                let originWidthSlot = this.SLOT_widthChanged?.bind(this)
                this.SLOT_widthChanged = (o, n) => {
                    newValue.width = n
                    if(originWidthSlot) originWidthSlot(o, n)
                }
            } else {
                Geometry.setAuto(this.__self, 'width', newValue.width, this.__self.constructor.meta.width)

                let originSlot = newValue.SLOT_widthChanged
                newValue.SLOT_widthChanged = (o, n)=>{
                    Geometry.setAuto(this.__self, 'width', n, this.__self.constructor.meta.width)
                    if(originSlot) originSlot.call(newValue, o, n)
                }
            }

            if(this.__self.height__prevent || this.__self.AHeight__prevent){
                newValue.height = this.height
                let originHeightSlot = this.SLOT_heightChanged?.bind(this)
                this.SLOT_heightChanged = (o, n) => {
                    newValue.height = n
                    if(originHeightSlot) originHeightSlot(o, n)
                }
            } else {
                Geometry.setAuto(this.__self, 'height', newValue.height, this.__self.constructor.meta.height)

                let originSlot = newValue.SLOT_heightChanged
                newValue.SLOT_heightChanged = (o, n)=>{
                    Geometry.setAuto(this.__self, 'height', n, this.__self.constructor.meta.height)
                    if(originSlot) originSlot.call(newValue, o, n)
                }
            }
        }
    }

    SLOT_sourceComponentChanged(oldValue, newValue){
        delete this.__lazyItem

        if(!this.active){
            this.item = null
            this.status = Loader.Null
            this.progress = 0
            return
        }

        this.status = Loader.Loading

        if(newValue){
            this.__updateProperty('visible')
            let item = this.sourceComponent.createObject(this, this.__sourceProperties, true)

            if(item){
                if(this.__completed || this.__propertiesUpdated){
                    this.item = item
                    this.status = Loader.Ready
                    this.loaded()
                } else {
                    this.__lazyItem = item
                }
            } else {
                this.item = null
                this.status = Loader.Error
            }

        } else {
            this.item = null
            this.status = Loader.Null
        }

        
    }

    SLOT_sourceChanged(oldValue, newValue){
        delete this.__lazyItem

        if(!this.active){
            this.item = null
            this.status = Loader.Null
            this.progress = 0
            return
        }

        this.status = Loader.Loading
        if(this.item) this.item.destroy()

        if(newValue){
            let path = this.source.replaceAll('qrc:/', '').replaceAll('.qml', '').split('/')
            let className = path[path.length-1]

            // 1. Search in current module by path
            let cls = null
            let candidate = null
            for(let i = 0; i < path.length; i++){
                if(candidate){
                    let name = path[i]
                    if(name in candidate){
                        candidate = candidate[name]
                    } else if(name in JQModules){
                        candidate = JQModules[name]
                    } else {
                        candidate = null
                        break
                    }
                } else {
                    candidate = JQModules[path[i]]
                }
            }
            if(candidate && candidate.isAssignableFrom) cls = candidate

            // 2. Search by className in all modules
            if(!cls){
                for(let key in JQModules){
                    candidate = JQModules[key]
                    if(candidate && className in candidate && candidate[className] && candidate[className].isAssignableFrom){
                        cls = candidate[className]
                        break
                    }
                    let sub = candidate
                    for(let i = 0; i < path.length; i++){
                        if(sub){
                            let name = path[i]
                            if(name in sub){
                                sub = sub[name]
                            } else if(name in JQModules){
                                sub = JQModules[name]
                            } else {
                                sub = null
                                break
                            }
                        } else {
                            sub = JQModules[path[i]]
                        }
                    }
                    if(sub && sub.isAssignableFrom){
                        cls = sub
                        break
                    }
                }
            }

            if(!cls || !cls.isAssignableFrom){
                try {
                    cls = eval(className)
                } catch (error) {
        
                }
            }

            if(!cls){
                console.error(`Loader: "${this.source}" is not founded | className: ${className} | modules:`, Object.keys(JQModules))
                this.item = null
                this.status = Loader.Error
                return
            }

            this.__updateProperty('visible')
            let item = cls.create(this, this.__sourceProperties)

            if(item){
                if(this.__completed){
                    this.item = item
                    this.status = Loader.Ready
                    this.loaded()
                } else {
                    this.__lazyItem = item
                    this.status = Loader.Ready
                }
            } else {
                this.item = null
                this.status = Loader.Error
            }

        } else {
            this.item = null
            this.status = Loader.Null
        }

        
    }

    SLOT_activeChanged(oldValue, newValue){
        if(newValue === false){
            delete this.__lazyItem
            this.item = null
            this.status = Loader.Null
            this.progress = 0
            return
        }

        this.__loadFromCurrentSource()
    }

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)

        if(this.item && (this.__self.width__prevent || this.__self.AWidth__prevent)){
            this.item.width = this.width
        }
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)
        
        if(this.item && (this.__self.height__prevent || this.__self.AHeight__prevent)){
            this.item.height = this.height
        }
    }
}



module.exports = Loader