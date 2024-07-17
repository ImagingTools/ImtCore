const Flickable = require("./Flickable")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class ListView extends Flickable {
    static Horizontal = 0
    static Vertical = 1

    static Beginning = 0
    static Center = 1
    static End = 2
    static Visible = 3
    static Contain = 4
    static SnapPosition = 5

    static NoSnap = 0
    static SnapToItem = 1
    static SnapOneItem = 2

    static meta = Object.assign({}, Flickable.meta, {
        model: {type: Var, value:undefined, signalName:'modelChanged'},
        delegate: {type: Var, value:undefined, signalName:'delegateChanged'},
        contentX: {type: Real, value:0, signalName:'contentXChanged'},
        contentY: {type: Real, value:0, signalName:'contentYChanged'},
        orientation: {type: Real, value:ListView.Vertical, signalName:'orientationChanged'},
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        currentIndex: {type: Real, value:-1, signalName:'currentIndexChanged'},
        currentItem: {type: Var, value:undefined, signalName:'currentItemChanged'},
        snapMode: {type: Real, value:ListView.NoSnap, signalName:'snapModeChanged'},
        cacheBuffer: {type: Real, value:320, signalName:'cacheBufferChanged'},
        count: {type: Real, value:0, signalName:'countChanged'},
        contentWidth: {type: Real, value:0, signalName:'contentWidthChanged'},
        contentHeight: {type: Real, value:0, signalName:'contentHeightChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
        modelChanged: {type:Signal, slotName:'onModelChanged', args:[]},
        delegateChanged: {type:Signal, slotName:'onDelegateChanged', args:[]},
        contentXChanged: {type:Signal, slotName:'onContentXChanged', args:[]},
        contentYChanged: {type:Signal, slotName:'onContentYChanged', args:[]},
        orientationChanged: {type:Signal, slotName:'onOrientationChanged', args:[]},
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
        currentIndexChanged: {type:Signal, slotName:'onCurrentIndexChanged', args:[]},
        currentItemChanged: {type:Signal, slotName:'onCurrentItemChanged', args:[]},
        snapModeChanged: {type:Signal, slotName:'onSnapModeChanged', args:[]},
        cacheBufferChanged: {type:Signal, slotName:'onCacheBufferChanged', args:[]},
        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},
        contentWidthChanged: {type:Signal, slotName:'onContentWidthChanged', args:[]},
        contentHeightChanged: {type:Signal, slotName:'onContentHeightChanged', args:[]},
    })

    indexAt(x, y){

    }
    itemAt(x, y){

    }

    itemAtIndex(index){
        return index >= 0 && index < this.$items.length.get() ? this.$items[index] : undefined
    }
    positionViewAtBeginning(){
        this.positionViewAtIndex(0, ListView.Beginning)
    }
    positionViewAtEnd(){
        this.positionViewAtIndex(this.$items.length.get()-1, ListView.Beginning)

        
    }
    positionViewAtIndex(index, mode){
        let pos = 'start'
        switch(mode){
            case ListView.Beginning: pos = 'start'; break;
            case ListView.Center: pos = 'center'; break;
            case ListView.End: pos = 'end'; break;
            case ListView.Visible: break;
            case ListView.Contain: {
                if(this.$items[index]){
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        if(this.getPropertyValue('contentWidth') <= this.getPropertyValue('width')) return

                        if(this.$items[index].getPropertyValue('x') <= this.getPropertyValue('contentX')){
                            this.getProperty('contentX').reset(this.$items[index].getPropertyValue('x'))
                        } else if(this.$items[index].getPropertyValue('x') + this.$items[index].getPropertyValue('width') >= this.getPropertyValue('contentX') + this.getPropertyValue('width')){
                            this.getProperty('contentX').reset(this.$items[index].getPropertyValue('x') + this.$items[index].getPropertyValue('width') - this.getPropertyValue('width'))
                        }
                    } else {
                        if(this.getPropertyValue('contentHeight') <= this.getPropertyValue('height')) return

                        if(this.$items[index].getPropertyValue('y') <= this.getPropertyValue('contentY')){
                            this.getProperty('contentY').reset(this.$items[index].getPropertyValue('y'))
                        } else if(this.$items[index].getPropertyValue('y') + this.$items[index].getPropertyValue('height') >= this.getPropertyValue('contentY') + this.getPropertyValue('height')){
                            this.getProperty('contentY').reset(this.$items[index].getPropertyValue('y') + this.$items[index].getPropertyValue('height') - this.getPropertyValue('height'))
                        }
                    }
                }
                break;
            }
        }

    }

}

module.exports = ListView