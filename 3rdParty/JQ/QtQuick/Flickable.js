const Item = require("./Item")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const Variant = require("../QtQml/Variant")
const Geometry = require("../QtQml/Geometry")
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
        contentWidth: {type: Geometry, value:-1, signalName:'contentWidthChanged'},
        contentHeight: {type: Geometry, value:-1, signalName:'contentHeightChanged'},
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
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('Flickable')
        obj.contentItem = Item.create(obj)

        // obj.contentItem.AXChanged.blockSignal(true)
        // obj.contentItem.AYChanged.blockSignal(true)
        // obj.contentItem.AWidthChanged.blockSignal(true)
        // obj.contentItem.AHeightChanged.blockSignal(true)

        obj.contentItem.onXChanged=()=>{
            if(-obj.contentItem.x > obj.contentWidth - obj.width){
                obj.contentItem.__setDOMStyle({
                    left: (obj.contentWidth - obj.width)+'px'
                })
            } else if(-obj.contentItem.x < obj.originX){
                obj.contentItem.__setDOMStyle({
                    left: obj.originX+'px'
                })
            } else {
                obj.contentItem.__setDOMStyle({
                    left: obj.contentItem.x+'px'
                })
            }
        }
    
        obj.contentItem.onYChanged=()=>{
            if(-obj.contentItem.y > obj.contentHeight - obj.height){
                obj.contentItem.__setDOMStyle({
                    top: (obj.contentHeight - obj.height)+'px'
                })
            } else if(-obj.contentItem.y < obj.originY){
                obj.contentItem.__setDOMStyle({
                    top: obj.originY+'px'
                })
            } else {
                obj.contentItem.__setDOMStyle({
                    top: obj.contentItem.y+'px'
                })
            }
        }

        JQApplication.MouseController.add(obj)
        return obj
    }

    __resolve(){
        return this.contentItem ? this.contentItem : this
    }

    SLOT_contentXChanged(oldValue, newValue){
        this.contentItem.x = -this.contentX
    }

    SLOT_contentYChanged(oldValue, newValue){
        this.contentItem.y = -this.contentY
    }

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)
        if(this.contentWidth - this.width > 0){
            if(this.contentX < this.originX){
                this.contentX = this.originX
            }
            if(this.contentX > this.contentWidth - this.width){
                this.contentX = this.contentWidth - this.width
            }
        } else {
            this.contentX = this.originX
        }
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)
        if(this.contentHeight - this.height > 0){
            if(this.contentY < this.originY){
                this.contentY = this.originY
            }
            if(this.contentY > this.contentHeight - this.height){
                this.contentY = this.contentHeight - this.height
            }
        } else {
            this.contentY = this.originY
        }
    }

    SLOT_contentWidthChanged(oldValue, newValue){
        this.contentItem.width = this.contentWidth
        if(this.contentWidth - this.width > 0){
            if(this.contentX < this.originX){
                this.contentX = this.originX
            }
            if(this.contentX > this.contentWidth - this.width){
                this.contentX = this.contentWidth - this.width
            }
        } else {
            this.contentX = this.originX
        }
    }

    SLOT_contentHeightChanged(oldValue, newValue){
        this.contentItem.height = this.contentHeight
        if(this.contentHeight - this.height > 0){
            if(this.contentY < this.originY){
                this.contentY = this.originY
            }
            if(this.contentY > this.contentHeight - this.height){
                this.contentY = this.contentHeight - this.height
            }
        } else {
            this.contentY = this.originY
        }
    }

    __onMouseMove(mouse){
        if(!this.interactive || !this.enabled || !this.visible) return

        if((this.contentWidth > 0 && this.contentWidth > this.width && mouse.wasDragX) || this.contentHeight > 0 && this.contentHeight > this.height && mouse.wasDragY){
            if(mouse.target) {
                let parent = mouse.target
                while(parent && parent !== this){
                    parent = parent.parent
                }

                if(parent === this && mouse.target instanceof MouseArea && mouse.target.preventStealing === false) {
                    mouse.target.__onMouseCanceled()
                    mouse.target = this
                }
            } else {
                mouse.target = this
            }
        }

        if(mouse.target === this){
            if(this.flickableDirection === Flickable.AutoFlickDirection || this.flickableDirection === Flickable.AutoFlickIfNeeded){
                if(this.contentWidth > 0 && this.contentWidth > this.width){
                    if(mouse.moveX > 0){
                        if(this.contentX + mouse.moveX < this.contentWidth - this.width + this.originX) {
                            this.contentX += mouse.moveX
                        } else {
                            this.contentX = this.contentWidth - this.width + this.originX
                            mouse.target = null
                        }
                    } else {
                        if(this.contentX + mouse.moveX >= this.originX) {
                            this.contentX += mouse.moveX
                        } else {
                            this.contentX = this.originX
                            mouse.target = null
                        }
                    }
                }
                if(this.contentHeight > 0 && this.contentHeight > this.height){
                    if(mouse.moveY > 0){
                        if(this.contentY + mouse.moveY < this.contentHeight - this.height + this.originY) {
                            this.contentY += mouse.moveY
                        } else {
                            this.contentY = this.contentHeight - this.height + this.originY
                            mouse.target = null
                        }
                    } else {
                        if(this.contentY + mouse.moveY >= this.originY) {
                            this.contentY += mouse.moveY
                        } else {
                            this.contentY = this.originY
                            mouse.target = null
                        }
                    }
                }
                
            }
        }
    }
    __onMouseDown(mouse){
        if(!this.interactive || !this.enabled || !this.visible) return

        if(!mouse.target){
            mouse.target = this
        }
    }
    __onMouseUp(mouse){
        if(!this.interactive || !this.enabled || !this.visible) return

        if(mouse.target === this){
            mouse.target = null
        }
    }

    __onWheel(wheel){
        if(!this.interactive || !this.enabled || !this.visible) return

        if(!wheel.target){
            let deltaX = wheel.angleDelta.x * 4
            let deltaY = wheel.angleDelta.y * 4

            if(this.flickableDirection === Flickable.AutoFlickDirection || this.flickableDirection === Flickable.AutoFlickIfNeeded){
                if(this.contentWidth > 0){
                    if(deltaX > 0){
                        if(this.contentX + deltaX < this.contentWidth - this.width + this.originX) {
                            this.contentX += deltaX
                        } else {
                            this.contentX = this.contentWidth - this.width + this.originX
                            wheel.target = null
                        }
                    } else {
                        if(this.contentX + deltaX >= this.originX) {
                            this.contentX += deltaX
                        } else {
                            this.contentX = this.originX
                            wheel.target = null
                        }
                    }
                }
                if(this.contentHeight > 0){
                    if(deltaY > 0){
                        if(this.contentY + deltaY < this.contentHeight - this.height + this.originY) {
                            this.contentY += deltaY
                        } else {
                            this.contentY = this.contentHeight - this.height + this.originY
                            wheel.target = null
                        }
                    } else {
                        if(this.contentY + deltaY >= this.originY) {
                            this.contentY += deltaY
                        } else {
                            this.contentY = this.originY
                            wheel.target = null
                        }
                    }
                }
                
            }
        }
    }

    __destroy(){
        this.contentItem.destroy()
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}



module.exports = Flickable