const Flickable = require("./Flickable")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class GridView extends Flickable {
    static Beginning = 0
    static Center = 1
    static End = 2

    static LeftToRight = 0
    static RightToLeft = 1
    static TopToBottom = 2
    static BottomToTop = 3

    static meta = Object.assign({}, Flickable.meta, {
        model: {type: Var, value:undefined, signalName:'modelChanged'},
        delegate: {type: Var, value:undefined, signalName:'delegateChanged'},
        layoutDirection: {type: Real, value:GridView.LeftToRight, signalName:'layoutDirectionChanged'},
        verticalLayoutDirection: {type: Real, value:GridView.TopToBottom, signalName:'verticalLayoutDirectionChanged'},
        currentIndex: {type: Real, value:-1, signalName:'currentIndexChanged'},
        currentItem: {type: Var, value:undefined, signalName:'currentItemChanged'},
        count: {type: Real, value:0, signalName:'countChanged'},
        cellWidth: {type: Real, value:100, signalName:'cellWidthChanged'},
        cellHeight: {type: Real, value:100, signalName:'cellHeightChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
        modelChanged: {type:Signal, slotName:'onModelChanged', args:[]},
        delegateChanged: {type:Signal, slotName:'onDelegateChanged', args:[]},
        layoutDirectionChanged: {type:Signal, slotName:'onLayoutDirectionChanged', args:[]},
        verticalLayoutDirectionChanged: {type:Signal, slotName:'onVerticalLayoutDirectionChanged', args:[]},
        currentIndexChanged: {type:Signal, slotName:'onCurrentIndexChanged', args:[]},
        currentItemChanged: {type:Signal, slotName:'onCurrentItemChanged', args:[]},
        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},
        cellWidthChanged: {type:Signal, slotName:'onCellWidthChanged', args:[]},
        cellHeightChanged: {type:Signal, slotName:'onCellHeightChanged', args:[]},
    })

    itemAtIndex(index){

    }
    positionViewAtBeginning(){

    }
    positionViewAtEnd(){
        
    }
    positionViewAtIndex(index, mode){

    }
}

GridView.initialize()

module.exports = GridView