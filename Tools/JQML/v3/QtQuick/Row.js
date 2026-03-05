const Item = require("./Item")
const Repeater = require("./Repeater")
const Real = require("../QtQml/Real")
const Geometry = require("../QtQml/Geometry")
const Signal = require("../QtQml/Signal")

class Row extends Item {
    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, },
        padding: {type: Real, value:0, },
        leftPadding: {type: Real, value:0, },
        topPadding: {type: Real, value:0, },
        rightPadding: {type: Real, value:0, },
        bottomPadding: {type: Real, value:0, },
        
        spacingChanged: {type:Signal, args:[]},
        paddingChanged: {type:Signal, args:[]},
        leftPaddingChanged: {type:Signal, args:[]},
        topPaddingChanged: {type:Signal, args:[]},
        rightPaddingChanged: {type:Signal, args:[]},
        bottomPaddingChanged: {type:Signal, args:[]},

        positioningComplete: {type:Signal, args:[]},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('Row')

        return obj
    }

    SLOT_paddingChanged(oldValue, newValue){
        this.leftPadding = this.padding
        this.topPadding = this.padding
        this.rightPadding = this.padding
        this.bottomPadding = this.padding
    }

    SLOT_leftPaddingChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_topPaddingChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_rightPaddingChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_bottomPaddingChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_spacingChanged(oldValue, newValue){
        this.__setDOMStyle({
            gap: newValue + 'px'
        })
        JQApplication.updateLater(this)
    }

    SLOT_visibleChanged(oldValue, newValue){
        JQApplication.beginUpdate()
        super.SLOT_visibleChanged(oldValue, newValue)
        JQApplication.endUpdate()
    }

    __updateGeometry(){
        if(!this.__proxy.visible) return
        
        let children = this.children

        let width = 0
        let height = 0
        let count = 0

        for(let i = 0; i < children.length; i++){
            if(!(children[i] instanceof Repeater) && children[i].visible && children[i].width > 0 && children[i].height > 0){
                if(children[i].__destroying) continue

                height = Math.max(height, children[i].height)
                width += children[i].width
                count += 1
            }
        }

        if(count > 0) width += (count-1) * this.spacing

        this.__setDOMStyle({
            paddingLeft: this.leftPadding+'px',
            paddingTop: this.topPadding+'px',
            paddingRight: this.rightPadding+'px',
            paddingBottom: this.bottomPadding+'px',
        })

        Geometry.setAuto(this.__self, 'width', width + this.leftPadding + this.rightPadding, this.__self.constructor.meta.width)
        Geometry.setAuto(this.__self, 'height', height + this.topPadding + this.bottomPadding, this.__self.constructor.meta.height)

        this.__proxy.positioningComplete()
    }

    __endUpdate(){
        this.__updateGeometry()
        super.__endUpdate()
    }

    forceLayout(){
        this.__updateGeometry()
    }
}



module.exports = Row