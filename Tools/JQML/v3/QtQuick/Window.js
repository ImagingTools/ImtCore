const Item = require("../QtQuick/Item")
const String = require("../QtQml/String")
const Geometry = require("../QtQml/Geometry")
const LinkedBool = require("../QtQml/LinkedBool")
const Signal = require("../QtQml/Signal")

class Window extends Item {
    static meta = Object.assign({}, Item.meta, {
        title: {type:String, value:0, },

        titleChanged: {type:Signal, args:[]},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        let dom = obj.__getDOM()
        dom.classList.add('Window')

        window.addEventListener('resize', ()=>{
            obj.width = window.innerWidth
            obj.height = window.innerHeight
        })

        window.addEventListener('load', ()=>{
            obj.width = window.innerWidth
            obj.height = window.innerHeight
        })

        dom.qml = obj
        obj.__connectDOM(this.parent)

        return obj
    }

    __complete(){
        this.width = window.innerWidth
        this.height = window.innerHeight

        this.__checkVisibility()
        super.__complete()
    }

    SLOT_titleChanged(oldValue, newValue){
        document.title = newValue
    }

    SLOT_widthChanged(oldValue, newValue){
        this.__checkVisibility()

        JQApplication.updateLater(this.parent)
        Geometry.setAuto(this.__self, 'AWidth', newValue, this.__self.constructor.meta.AWidth)
    }

    SLOT_heightChanged(oldValue, newValue){
        this.__checkVisibility()

        JQApplication.updateLater(this.parent)
        Geometry.setAuto(this.__self, 'AHeight', newValue, this.__self.constructor.meta.AHeight)
    }
}



module.exports = Window