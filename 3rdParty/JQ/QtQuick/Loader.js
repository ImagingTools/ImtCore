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
        item: {type:Var, value:undefined, signalName:'itemChanged'},
        source: {type:String, value:'', signalName:'sourceChanged'},
        sourceComponent: {type:Variant, typeTarget:Component, value:undefined, signalName:'sourceComponentChanged'},
        status: {type:Real, value:Loader.Null, signalName:'statusChanged'},
        progress: {type:Real, value:0, signalName:'progressChanged'},
        asynchronous: {type:Bool, value:false, signalName:'asynchronousChanged'},

        itemChanged: {type:Signal, slotName:'onItemChanged', args:[]},
        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
        sourceComponentChanged: {type:Signal, slotName:'onSourceComponentChanged', args:[]},
        statusChanged: {type:Signal, slotName:'onStatusChanged', args:[]},
        progressChanged: {type:Signal, slotName:'onProgressChanged', args:[]},
        asynchronousChanged: {type:Signal, slotName:'onAsynchronousChanged', args:[]},

        loaded: {type:Signal, slotName:'onLoaded', args:[]},
    })

    SLOT_sourceComponentChanged(oldValue, newValue){
        if(this.item) this.item.destroy()
        if(!this.sourceComponent) return

        let item = this.sourceComponent.createObject(this, {}, true)
        this.item = item

        this.item.widthChanged.connect(()=>{
            Geometry.setAuto(this.__self, 'width', this.item.width, this.__self.constructor.meta.width)
        })
        this.item.heightChanged.connect(()=>{
            Geometry.setAuto(this.__self, 'height', this.item.height, this.__self.constructor.meta.height)
        })

        Geometry.setAuto(this.__self, 'width', this.item.width, this.__self.constructor.meta.width)
        Geometry.setAuto(this.__self, 'height', this.item.height, this.__self.constructor.meta.height)
        
        // Geometry.setAuto(this.item.__self, 'width', this.width, this.item.__self.constructor.meta.width)
        // Geometry.setAuto(this.item.__self, 'height', this.height, this.item.__self.constructor.meta.height)

        this.loaded()
    }

    SLOT_sourceChanged(oldValue, newValue){
        if(this.item) this.item.destroy()
        if(!this.source) return

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

        let item = cls.create(this)
        this.item = item

        this.item.widthChanged.connect(()=>{
            Geometry.setAuto(this.__self, 'width', this.item.width, this.__self.constructor.meta.width)
        })
        this.item.heightChanged.connect(()=>{
            Geometry.setAuto(this.__self, 'height', this.item.height, this.__self.constructor.meta.height)
        })

        Geometry.setAuto(this.__self, 'width', this.item.width, this.__self.constructor.meta.width)
        Geometry.setAuto(this.__self, 'height', this.item.height, this.__self.constructor.meta.height)

        // Geometry.setAuto(this.item.__self, 'width', this.width, this.item.__self.constructor.meta.width)
        // Geometry.setAuto(this.item.__self, 'height', this.height, this.item.__self.constructor.meta.height)

        this.loaded()
    }

    // SLOT_widthChanged(oldValue, newValue){
    //     super.SLOT_widthChanged()

    //     if(this.item){
    //         Geometry.setAuto(this.item.__self, 'width', newValue, this.item.__self.constructor.meta.width)
    //     }
    // }

    // SLOT_heightChanged(oldValue, newValue){
    //     super.SLOT_heightChanged()
        
    //     if(this.item){
    //         Geometry.setAuto(this.item.__self, 'height', newValue, this.item.__self.constructor.meta.height)
    //     }
    // }
}



module.exports = Loader