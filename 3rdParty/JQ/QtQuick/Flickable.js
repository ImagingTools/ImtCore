const Item = require("./Item")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const Signal = require("../QtQml/Signal")
const QtFunctions = require("../Qt/functions")

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
        contentItem: {type: Item, signalName:'contentItemChanged'},
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
        // proxy.width = QtFunctions.binding(()=>{return proxy.conte})
        return proxy
    }

    onContentXChanged(){
        this.contentItem.x = -this.contentX
    }

    onContentYChanged(){
        this.contentItem.y = -this.contentY
    }

    onContentWidthChanged(){
        this.contentItem.width = this.contentWidth
    }

    onContentHeightChanged(){
        this.contentItem.height = this.contentHeight
    }

    __onMouseMove(mouse){
        if(mouse.wasDrag) {
            if(mouse.target) mouse.target.__onMouseCanceled()
            mouse.target = this
        }

        if(mouse.target === this){
            if(this.flickableDirection === Flickable.AutoFlickDirection || this.flickableDirection === Flickable.AutoFlickIfNeeded){
                if(this.contentWidth > 0){
                    if(mouse.moveX > 0){
                        if(this.contentX + mouse.moveX < this.contentWidth - this.width) {
                            this.contentX += mouse.moveX
                        } else {
                            this.contentX = this.contentWidth - this.width
                        }
                    } else {
                        if(this.contentX + mouse.moveX >= 0) {
                            this.contentX += mouse.moveX
                        } else {
                            this.contentX = 0
                        }
                    }
                }
                if(this.contentHeight > 0){
                    if(mouse.moveY > 0){
                        if(this.contentY + mouse.moveY < this.contentHeight - this.height) {
                            this.contentY += mouse.moveY
                        } else {
                            this.contentY = this.contentHeight - this.height
                        }
                    } else {
                        if(this.contentY + mouse.moveY >= 0) {
                            this.contentY += mouse.moveY
                        } else {
                            this.contentY = 0
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
    __onMouseClick(mouse){}
    __onMouseDblClick(mouse){}
}

module.exports = Flickable