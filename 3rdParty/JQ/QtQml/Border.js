const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Color = require("./Color")
const Signal = require("./Signal")

class Border extends GroupProperty {
    static meta = {
        width: {type:Real, value:1, },
        color: {type:Color, value:'black', },
        
        widthChanged: {type:Signal, args:[]},
        colorChanged: {type:Signal, args:[]},
    }

    static create(parent = null){
        parent.__setDOMStyle({
            outline: `1px solid black`,
            outlineOffset: `-1px`,
        })

        return super.create(parent)
    }

    SLOT_widthChanged(oldValue, newValue){
        if(this.__parent)
        this.__parent.__setDOMStyle({
            outline: `${newValue}px solid ${this.__proxy.color}`,
            outlineOffset: `-${newValue}px`,
        })
    }

    SLOT_colorChanged(oldValue, newValue){
        if(this.__parent)
        this.__parent.__setDOMStyle({
            outline: `${this.__proxy.width}px solid ${newValue}`,
            outlineOffset: `-${this.__proxy.width}px`,
        })
    }

    // onChanged(){
    //     if(this.__parent)
    //     this.__parent.__setDOMStyle({
    //         outline: `${this.width}px solid ${this.color}`,
    //         outlineOffset: `-${this.width}px`,
    //     })
    // }

    // onWidthChanged(){
    //     this.onChanged()
    // }

    // onColorChanged(){
    //     this.onChanged()
    // }
}



module.exports = Border