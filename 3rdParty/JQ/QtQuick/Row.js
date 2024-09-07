const Item = require("./Item")
const Repeater = require("./Repeater")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class Row extends Item {
    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('Row')

        return proxy
    }

    onSpacingChanged(){
        this.__setDOMStyle({
            gap: this.spacing + 'px'
        })
        this.__updateGeometry()
    }

    __updateGeometry(){
        let children = this.children

        let width = 0
        let height = 0
        let count = 0

        for(let i = 0; i < children.length; i++){
            if(!(children[i] instanceof Repeater) && children[i].visible && children[i].width > 0 && children[i].height > 0){
                height = Math.max(height, children[i].height)
                width += children[i].width
                count += 1
            }
        }

        if(count > 0) width += (count-1) * this.spacing

        this.__getObject('width').__setAuto(width)
        this.__getObject('height').__setAuto(height)
    }

    __endUpdate(...args){
        this.__updateGeometry()
    }
}

module.exports = Row