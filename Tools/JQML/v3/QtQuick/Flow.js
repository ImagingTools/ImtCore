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

    __updateProperties(){
        super.__updateProperties()
        this.__updateGeometry()
    }

    'SLOT_Component.completed'(){
        JQApplication.updateLater(this)
    }

    SLOT_paddingChanged(oldValue, newValue){
        this.leftPadding = this.padding
        this.topPadding = this.padding
        this.rightPadding = this.padding
        this.bottomPadding = this.padding
    }

    SLOT_visibleChanged(oldValue, newValue){
        JQApplication.beginUpdate()
        super.SLOT_visibleChanged(oldValue, newValue)
        JQApplication.endUpdate()
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

    __layoutChildren(){
        let children = this.children
        let spacing = this.spacing
        let contentWidth = 0
        let contentHeight = 0
        let lineSize = 0
        let pos = 0
        let count = 0
        let wrapLimit = 0

        if(this.flow === Flow.LeftToRight){
            if(this.width__prevent){
                wrapLimit = this.width - (this.leftPadding + this.rightPadding)
                if(wrapLimit < 0) wrapLimit = 0
            }
        } else if(this.height__prevent){
            wrapLimit = this.height - (this.topPadding + this.bottomPadding)
            if(wrapLimit < 0) wrapLimit = 0
        }

        for(let i = 0; i < children.length; i++){
            let child = children[i]
            if(!child || child instanceof Repeater || child.__destroying) continue

            // Positioners skip only explicitly hidden children. Inherited
            // invisibility (parent.visible === false) must not zero implicit size.
            let explicitlyVisible = child.visible
            if(child.__self && child.__self.visible && typeof child.__self.visible === 'object' && 'value' in child.__self.visible){
                explicitlyVisible = child.__self.visible.value
            }
            if(!explicitlyVisible) continue

            let childWidth = child.width
            let childHeight = child.height
            if(!Number.isFinite(childWidth)) childWidth = 0
            if(!Number.isFinite(childHeight)) childHeight = 0

            if(this.flow === Flow.LeftToRight){
                if(wrapLimit > 0 && pos > 0 && pos + childWidth > wrapLimit){
                    pos = 0
                    contentHeight += lineSize + spacing
                    lineSize = 0
                }

                pos += childWidth
                if(pos > contentWidth) contentWidth = pos
                pos += spacing
                if(childHeight > lineSize) lineSize = childHeight
            } else {
                if(wrapLimit > 0 && pos > 0 && pos + childHeight > wrapLimit){
                    pos = 0
                    contentWidth += lineSize + spacing
                    lineSize = 0
                }

                pos += childHeight
                if(pos > contentHeight) contentHeight = pos
                pos += spacing
                if(childWidth > lineSize) lineSize = childWidth
            }

            count += 1
        }

        if(count > 0){
            if(this.flow === Flow.LeftToRight){
                contentHeight += lineSize
            } else {
                contentWidth += lineSize
            }
        }

        return {
            width: contentWidth,
            height: contentHeight,
        }
    }

    __updateGeometry(){
        let size = this.__layoutChildren()

        this.__setDOMStyle({
            paddingLeft: this.leftPadding + 'px',
            paddingTop: this.topPadding + 'px',
            paddingRight: this.rightPadding + 'px',
            paddingBottom: this.bottomPadding + 'px',
        })

        Geometry.setAuto(this.__self, 'width', size.width + this.leftPadding + this.rightPadding, this.__self.constructor.meta.width)
        Geometry.setAuto(this.__self, 'height', size.height + this.topPadding + this.bottomPadding, this.__self.constructor.meta.height)
        this.implicitWidth = size.width + this.leftPadding + this.rightPadding
        this.implicitHeight = size.height + this.topPadding + this.bottomPadding
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