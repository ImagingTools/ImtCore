const Item = require("./Item")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const Variant = require("../QtQml/Variant")
const Signal = require("../QtQml/Signal")
const QtFunctions = require("../Qt/functions")
const MouseArea = require("./MouseArea")
const JQApplication = require("../core/JQApplication")

class Flickable extends Item {
    static AutoFlickDirection = 0
    static AutoFlickIfNeeded = 1
    static HorizontalFlick = 2
    static VerticalFlick = 3
    static HorizontalAndVerticalFlick = 4

    static StopAtBounds = 0
    static DragOverBounds = 1
    static OvershootBounds = 2
    static DragAndOvershootBounds = 3

    static meta = Object.assign({}, Item.meta, {
        contentWidth: {type: Real, value:-1, signalName:'contentWidthChanged'},
        contentHeight: {type: Real, value:-1, signalName:'contentHeightChanged'},
        contentX: {type: Real, value:0, signalName:'contentXChanged'},
        contentY: {type: Real, value:0, signalName:'contentYChanged'},
        originX: {type: Real, value:0, signalName:'originXChanged'},
        originY: {type: Real, value:0, signalName:'originYChanged'},
        interactive: {type: Bool, value:true, signalName:'interactiveChanged'},
        contentItem: {type: Variant, typeTarget: Item, signalName:'contentItemChanged'},
        flickableDirection: {type: Real, value:Flickable.AutoFlickDirection, signalName:'flickableDirectionChanged'},
        boundsBehavior: {type: Real, value:0, signalName:'boundsBehaviorChanged'},
        
        contentWidthChanged: {type:Signal, slotName:'onContentWidthChanged', args:[]},
        contentHeightChanged: {type:Signal, slotName:'onContentHeightChanged', args:[]},
        contentXChanged: {type:Signal, slotName:'onContentXChanged', args:[]},
        contentYChanged: {type:Signal, slotName:'onContentYChanged', args:[]},
        originXChanged: {type:Signal, slotName:'onOriginXChanged', args:[]},
        originYChanged: {type:Signal, slotName:'onOriginYChanged', args:[]},
        interactiveChanged: {type:Signal, slotName:'onInteractiveChanged', args:[]},
        contentItemChanged: {type:Signal, slotName:'onContentItemChanged', args:[]},
        flickableDirectionChanged: {type:Signal, slotName:'onFlickableDirectionChanged', args:[]},
        boundsBehaviorChanged: {type:Signal, slotName:'onBoundsBehaviorChanged', args:[]},

        flickEnded: {type:Signal, slotName:'onFlickEnded', args:[]},
        flickStarted: {type:Signal, slotName:'onFlickStarted', args:[]},
        movementEnded: {type:Signal, slotName:'onMovementEnded', args:[]},
        movementStarted: {type:Signal, slotName:'onMovementStarted', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('Flickable')
        proxy.contentItem = Item.create()
        proxy.contentItem.parent = proxy
        JQApplication.MouseController.add(proxy)
        return proxy
    }

    __resolve(){
        return this.contentItem
    }

    onContentXChanged(){
        this.contentItem.x = -this.contentX
    }

    onContentYChanged(){
        this.contentItem.y = -this.contentY
    }

    onWidthChanged(){
        super.onWidthChanged()
        if(this.contentWidth - this.width > 0){
            if(this.contentX < 0){
                this.contentX = 0
            }
            if(this.contentX > this.contentWidth - this.width){
                this.contentX = this.contentWidth - this.width
            }
        } else {
            this.contentX = 0
        }
    }

    onHeightChanged(){
        super.onHeightChanged()
        if(this.contentHeight - this.height > 0){
            if(this.contentY < 0){
                this.contentY = 0
            }
            if(this.contentY > this.contentHeight - this.height){
                this.contentY = this.contentHeight - this.height
            }
        } else {
            this.contentY = 0
        }
    }

    onContentWidthChanged(){
        this.contentItem.width = this.contentWidth
        if(this.contentWidth - this.width > 0){
            if(this.contentX < 0){
                this.contentX = 0
            }
            if(this.contentX > this.contentWidth - this.width){
                this.contentX = this.contentWidth - this.width
            }
        } else {
            this.contentX = 0
        }
    }

    onContentHeightChanged(){
        this.contentItem.height = this.contentHeight
        if(this.contentHeight - this.height > 0){
            if(this.contentY < 0){
                this.contentY = 0
            }
            if(this.contentY > this.contentHeight - this.height){
                this.contentY = this.contentHeight - this.height
            }
        } else {
            this.contentY = 0
        }
    }

    __onMouseMove(mouse){
        if(mouse.wasDrag) {
            if(mouse.target) {
                if(mouse.target instanceof MouseArea && mouse.target.preventStealing === false) mouse.target = this
                mouse.target.__onMouseCanceled()
            } else {
                mouse.target = this
            }
            
        }

        if(mouse.target === this){
            if(this.flickableDirection === Flickable.AutoFlickDirection || this.flickableDirection === Flickable.AutoFlickIfNeeded){
                if(this.contentWidth > 0 && this.contentWidth > this.width){
                    if(mouse.moveX > 0){
                        if(this.contentX + mouse.moveX < this.contentWidth - this.width) {
                            this.contentX += mouse.moveX
                        } else {
                            this.contentX = this.contentWidth - this.width
                            mouse.target = null
                        }
                    } else {
                        if(this.contentX + mouse.moveX >= 0) {
                            this.contentX += mouse.moveX
                        } else {
                            this.contentX = 0
                            mouse.target = null
                        }
                    }
                }
                if(this.contentHeight > 0 && this.contentHeight > this.height){
                    if(mouse.moveY > 0){
                        if(this.contentY + mouse.moveY < this.contentHeight - this.height) {
                            this.contentY += mouse.moveY
                        } else {
                            this.contentY = this.contentHeight - this.height
                            mouse.target = null
                        }
                    } else {
                        if(this.contentY + mouse.moveY >= 0) {
                            this.contentY += mouse.moveY
                        } else {
                            this.contentY = 0
                            mouse.target = null
                        }
                    }
                }
                
            }
        }
    }
    __onMouseDown(mouse){
        if(!mouse.target){
            mouse.target = this
        }
    }
    __onMouseUp(mouse){
        if(mouse.target === this){
            mouse.target = null
        }
    }

    __onWheel(wheel){
        if(wheel.target === this){
            let deltaX = wheel.angleDelta.x * 4
            let deltaY = wheel.angleDelta.y * 4

            if(this.flickableDirection === Flickable.AutoFlickDirection || this.flickableDirection === Flickable.AutoFlickIfNeeded){
                if(this.contentWidth > 0){
                    if(deltaX > 0){
                        if(this.contentX + deltaX < this.contentWidth - this.width) {
                            this.contentX += deltaX
                        } else {
                            this.contentX = this.contentWidth - this.width
                            wheel.target = null
                        }
                    } else {
                        if(this.contentX + deltaX >= 0) {
                            this.contentX += deltaX
                        } else {
                            this.contentX = 0
                            wheel.target = null
                        }
                    }
                }
                if(this.contentHeight > 0){
                    if(deltaY > 0){
                        if(this.contentY + deltaY < this.contentHeight - this.height) {
                            this.contentY += deltaY
                        } else {
                            this.contentY = this.contentHeight - this.height
                            wheel.target = null
                        }
                    } else {
                        if(this.contentY + deltaY >= 0) {
                            this.contentY += deltaY
                        } else {
                            this.contentY = 0
                            wheel.target = null
                        }
                    }
                }
                
            }
        }
    }

    __destroy(){
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}

module.exports = Flickable