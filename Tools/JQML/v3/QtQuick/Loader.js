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

        itemChanged: {type:Signal, args:[]},
        sourceChanged: {type:Signal, args:[]},
        sourceComponentChanged: {type:Signal, args:[]},
        statusChanged: {type:Signal, args:[]},
        progressChanged: {type:Signal, args:[]},
        asynchronousChanged: {type:Signal, args:[]},

        loaded: {type:Signal, args:[]},
    })

    __updatePrimaryProperties(){
        super.__updatePrimaryProperties()
        this.__updateProperty('sourceComponent')
    }

    __complete(){
        super.__complete()
    }

    'SLOT_Component.completed'(){
        if(this.__lazyItem){
            this.item = this.__lazyItem
            this.status = Loader.Ready
            this.loaded()
        }
        
    }

    SLOT_itemChanged(oldValue, newValue){
        if(oldValue) oldValue.destroy()

        if(newValue instanceof Item){
            // newValue.widthChanged.connect(()=>{
            //     Geometry.setAuto(this.__self, 'width', newValue.width, this.__self.constructor.meta.width)
            // })
            // newValue.heightChanged.connect(()=>{
            //     Geometry.setAuto(this.__self, 'height', newValue.height, this.__self.constructor.meta.height)
            // })


            if(this.__self.width__prevent || this.__self.AWidth__prevent){
                newValue.width = this.width
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
        this.status = Loader.Loading

        if(newValue){
            this.__updateProperty('visible')
            let item = this.sourceComponent.createObject(this, {}, true)

            if(item){
                if(this.__completed){
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
        this.status = Loader.Loading
        if(this.item) this.item.destroy()

        if(newValue){
            let path = this.source.replaceAll('qrc:/', '').replaceAll('.qml', '').split('/')
            let className = path[path.length-1]

            let cls = null
            try {
                cls = eval(className)
            } catch (error) {
                while(path.length){
                    if(cls){
                        let name = path.shift()
                        if(name in cls){
                            cls = cls[name]
                        }
                    } else {
                        cls = JQModules[path.shift()]
                    }
                }
            }

            // let source = this.source.split('/').pop().replaceAll('.qml', '')

            // let cls = eval(source)

            this.__updateProperty('visible')
            let item = cls.create(this)
            
            if(item){
                if(this.__completed){
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