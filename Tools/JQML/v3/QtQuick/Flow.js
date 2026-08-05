const Item = require("./Item")
const Repeater = require("./Repeater")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const Geometry = require("../QtQml/Geometry")
const Signal = require("../QtQml/Signal")
const QtEnums = require("../Qt/enums")

class Flow extends Item {
    static LeftToRight = 0
    static TopToBottom = 1

    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, },
        padding: {type: Real, value:0, },
        leftPadding: {type: Real, value:0, },
        topPadding: {type: Real, value:0, },
        rightPadding: {type: Real, value:0, },
        bottomPadding: {type: Real, value:0, },
        flow: {type: Int, value:Flow.LeftToRight },
        layoutDirection: {type: Int, value:QtEnums.LeftToRight },
        
        spacingChanged: {type:Signal, args:[]},
        paddingChanged: {type:Signal, args:[]},
        leftPaddingChanged: {type:Signal, args:[]},
        topPaddingChanged: {type:Signal, args:[]},
        rightPaddingChanged: {type:Signal, args:[]},
        bottomPaddingChanged: {type:Signal, args:[]},
        flowChanged: {type:Signal, args:[]},
        layoutDirectionChanged: {type:Signal, args:[]},

        positioningComplete: {type:Signal, args:[]},
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

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)
        JQApplication.updateLater(this)
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)
        JQApplication.updateLater(this)
    }

    SLOT_flowChanged(oldValue, newValue){
        this.__updateFlow()
    }

    SLOT_layoutDirectionChanged(oldValue, newValue){
        this.__updateFlow()
    }

    SLOT_spacingChanged(oldValue, newValue){
        this.__updateFlow()
    }

    __updateFlow(){
        if(this.flow === Flow.LeftToRight){
            this.__setDOMStyle({
                flexDirection: this.layoutDirection === QtEnums.LeftToRight ? 'row' : 'row-reverse',
                gap: this.spacing + 'px'
            })
        } else {
            this.__setDOMStyle({
                flexDirection: this.layoutDirection === QtEnums.RightToLeft ? 'column' : 'column-reverse',
                gap: this.spacing + 'px'
            })
        }

        JQApplication.updateLater(this)
    }

    __updateGeometry(){
        let width = this.width__prevent ? this.width - (this.leftPadding + this.rightPadding) : 0
        let height = this.height__prevent ? this.height - (this.topPadding + this.bottomPadding) : 0

        this.__setDOMStyle({
            minWidth: width + 'px',
            minHeight: height + 'px',
            width: width + 'px',
            height: height + 'px',

            paddingLeft: this.leftPadding + 'px',
            paddingTop: this.topPadding + 'px',
            paddingRight: this.rightPadding + 'px',
            paddingBottom: this.bottomPadding + 'px',
        })

        width = this.__DOM.scrollWidth - (this.leftPadding + this.rightPadding)
        height = this.__DOM.scrollHeight - (this.topPadding + this.bottomPadding)

        this.__setDOMStyle({
            minWidth: width + 'px',
            minHeight: height + 'px',
            width: width + 'px',
            height: height + 'px',

            paddingLeft: this.leftPadding + 'px',
            paddingTop: this.topPadding + 'px',
            paddingRight: this.rightPadding + 'px',
            paddingBottom: this.bottomPadding + 'px',
        })

        Geometry.setAuto(this.__self, 'width', width + this.leftPadding + this.rightPadding, this.__self.constructor.meta.width)
        Geometry.setAuto(this.__self, 'height', height + this.topPadding + this.bottomPadding, this.__self.constructor.meta.height)
        this.implicitWidth = width + this.leftPadding + this.rightPadding
        this.implicitHeight = height + this.topPadding + this.bottomPadding
        this.positioningComplete()
    }

    __endUpdate(){
        this.__updateGeometry()
        super.__endUpdate()
    }

    __removeChild(child){
        super.__removeChild(child)

        JQApplication.updateLater(this)
    }

    __addChild(child){
        super.__addChild(child)

        JQApplication.updateLater(this)
    }

    forceLayout(){
        this.__updateGeometry()
    }
}



module.exports = Flow