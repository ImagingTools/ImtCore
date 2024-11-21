const Item = require("./Item")
const Component = require("../QtQml/Component")
const Var = require("../QtQml/Var")
const Variant = require("../QtQml/Variant")
const Real = require("../QtQml/Real")
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

    onSourceComponentChanged(){
        if(this.item) this.item.destroy()
        if(!this.sourceComponent) return

        let item = this.sourceComponent.createObject(this)
        this.item = item

        if(this.__getDataQml('width').__auto) {
            this.__getDataQml('width').__setCompute(()=>{return this.item.width})
            this.__getDataQml('width').__update()
        } else {
            this.item.__getDataQml('width').__setCompute(()=>{return this.width})
            this.item.__getDataQml('width').__update()
        }
        if(this.__getDataQml('height').__auto) {
            this.__getDataQml('height').__setCompute(()=>{return this.item.height})
            this.__getDataQml('height').__update()
        } else {
            this.item.__getDataQml('height').__setCompute(()=>{return this.height})
            this.item.__getDataQml('height').__update()
        }

        this.loaded()
    }

    onSourceChanged(){
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

        if(this.__getDataQml('width').__auto) {
            this.__getDataQml('width').__setCompute(()=>{return this.item.width})
            this.__getDataQml('width').__update()
        } else {
            this.item.__getDataQml('width').__setCompute(()=>{return this.width})
            this.item.__getDataQml('width').__update()
        }
        if(this.__getDataQml('height').__auto) {
            this.__getDataQml('height').__setCompute(()=>{return this.item.height})
            this.__getDataQml('height').__update()
        } else {
            this.item.__getDataQml('height').__setCompute(()=>{return this.height})
            this.item.__getDataQml('height').__update()
        }

        this.loaded()
    }

    onWidthChanged(){
        super.onWidthChanged()

        if(this.item){
            this.item.__getDataQml('width').__setAuto(this.width)
        }
    }

    onHeightChanged(){
        super.onHeightChanged()
        
        if(this.item){
            this.item.__getDataQml('height').__setAuto(this.height)
        }
    }
}

Loader.initialize()

module.exports = Loader