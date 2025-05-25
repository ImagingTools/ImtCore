const Item = require("./Item")
const Repeater = require("./Repeater")
const Real = require("../QtQml/Real")
const Geometry = require("../QtQml/Geometry")
const Signal = require("../QtQml/Signal")

class Flow extends Item {
    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        padding: {type: Real, value:0, signalName:'paddingChanged'},
        leftPadding: {type: Real, value:0, signalName:'leftPaddingChanged'},
        topPadding: {type: Real, value:0, signalName:'topPaddingChanged'},
        rightPadding: {type: Real, value:0, signalName:'rightPaddingChanged'},
        bottomPadding: {type: Real, value:0, signalName:'bottomPaddingChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
        paddingChanged: {type:Signal, slotName:'onPaddingChanged', args:[]},
        leftPaddingChanged: {type:Signal, slotName:'onLeftPaddingChanged', args:[]},
        topPaddingChanged: {type:Signal, slotName:'onTopPaddingChanged', args:[]},
        rightPaddingChanged: {type:Signal, slotName:'onRightPaddingChanged', args:[]},
        bottomPaddingChanged: {type:Signal, slotName:'onBottomPaddingChanged', args:[]},

        positioningComplete: {type:Signal, slotName:'onPositioningComplete', args:[]},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('Flow')

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
            gap: this.spacing + 'px'
        })
        JQApplication.updateLater(this)
    }

    __updateGeometry(){
        let children = this.children

        let width = 0
        let height = 0
        let count = 0

        for(let i = 0; i < children.length; i++){
            if(!(children[i] instanceof Repeater) && children[i].visible && children[i].width > 0 && children[i].height > 0){
                if(children[i].__destroying) continue
                
                width = Math.max(width, children[i].width)
                height += children[i].height
                count += 1
            }
        }

        if(count > 0) height += (count-1) * this.spacing

        this.__setDOMStyle({
            paddingLeft: this.leftPadding+'px',
            paddingTop: this.topPadding+'px',
            paddingRight: this.rightPadding+'px',
            paddingBottom: this.bottomPadding+'px',
        })

        Geometry.setAuto(this.__self, 'width', width + this.leftPadding + this.rightPadding, this.__self.constructor.meta.width)
        Geometry.setAuto(this.__self, 'height', height + this.topPadding + this.bottomPadding, this.__self.constructor.meta.height)

        this.positioningComplete()
    }

    __endUpdate(){
        this.__updateGeometry()
        super.__endUpdate()
    }

    forceLayout(){
        this.__updateGeometry()
    }
}



module.exports = Flow