const Item = require("./Item")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const Signal = require("../QtQml/Signal")

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
}

module.exports = Flickable