const { Item } = require('./Item')
const { QBool, QReal, QVar, QAutoGeometry, QPositiveReal } = require('../utils/properties')

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

    static defaultProperties = {
        contentWidth: { type: QAutoGeometry, value: -1, changed: '$contentWidthChanged' },
        contentHeight: { type: QAutoGeometry, value: -1, changed: '$contentHeightChanged' },
        contentX: { type: QPositiveReal, value: 0, changed: '$contentXChanged' },
        contentY: { type: QPositiveReal, value: 0, changed: '$contentYChanged' },
        originX: { type: QReal, value: 0 },
        originY: { type: QReal, value: 0 },
        interactive: { type: QBool, value: true },
        contentItem: { type: QVar, value: null },
        flickableDirection: { type: QReal, value: Flickable.AutoFlickDirection, changed: '$flickableDirectionChanged' },
        boundsBehavior: { type: QReal, value: Flickable.DragAndOvershootBounds, changed: '$boundsBehaviorChanged' },
    }

    static defaultSignals = {
        flickEnded: { params: [] },
		flickStarted: { params: [] },
		movementEnded: { params: [] },
		movementStarted: { params: [] },
    }

    $flickable = true

    constructor(parent){
        super(parent)
        let contentItem = new Item(this)
        this.getStatement('contentItem').reset(contentItem)
        contentItem.getProperty('width').setCompute(()=>{return this.width})
        contentItem.getProperty('height').setCompute(()=>{return this.height})
        contentItem.getProperty('width').update()
        contentItem.getProperty('height').update()
        MouseController.add(this)
    }

    $contentXChanged(){
        this.getStatement('contentItem').get().getStatement('x').reset(-this.getStatement('contentX').get())
    }

    $contentYChanged(){
        this.getStatement('contentItem').get().getStatement('y').reset(-this.getStatement('contentY').get())
    }


    $contentWidthChanged(){
        this.getStatement('contentItem').get().getStatement('width').set(this.getStatement('contentWidth').get())
        if(this.getPropertyValue('contentX') > this.getPropertyValue('contentWidth') - this.getPropertyValue('width')){
            this.getProperty('contentX').reset(this.getPropertyValue('contentWidth') - this.getPropertyValue('width'))
        }
    }

    $contentHeightChanged(){
        this.getStatement('contentItem').get().getStatement('height').set(this.getStatement('contentHeight').get())
        if(this.getPropertyValue('contentY') > this.getPropertyValue('contentHeight') - this.getPropertyValue('height')){
            this.getProperty('contentY').reset(this.getPropertyValue('contentHeight') - this.getPropertyValue('height'))
        }
    }
  
    $flickableDirectionChanged(){

    }

    onDoubleClick(x, y, button){

    }

    onMouseDown(x, y, button){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')) {
            this.$mouseX = x
            this.$mouseY = y
        } else {
            return true
        }
    }
    onMouseUp(x, y, button){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')) {
            delete this.$mouseX
            delete this.$mouseY
        } else {
            return true
        }
    }
    onMouseMove(x, y){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')) {
            if(this.getPropertyValue('contentX') + (this.$mouseX - x) > 0 && this.getPropertyValue('contentX') + (this.$mouseX - x) < this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width')){
                this.getStatement('contentX').reset(this.getPropertyValue('contentX') + (this.$mouseX - x))
            } else {
                if(this.getPropertyValue('contentX') + (this.$mouseX - x) <= 0) this.getStatement('contentX').reset(0)
                if(this.getPropertyValue('contentX') + (this.$mouseX - x) >= this.getPropertyValue('contentItem').getPropertyValue('width')) this.getStatement('contentX').reset(this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width'))
            }

            if(this.getPropertyValue('contentY') + (this.$mouseY - y) > 0 && this.getPropertyValue('contentY') + (this.$mouseY - y) < this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height')){
                this.getStatement('contentY').reset(this.getPropertyValue('contentY') + (this.$mouseY - y))
            } else {
                if(this.getPropertyValue('contentY') + (this.$mouseY - y) <= 0) this.getStatement('contentY').reset(0)
                if(this.getPropertyValue('contentY') + (this.$mouseY - y) >= this.getPropertyValue('contentItem').getPropertyValue('height')) this.getStatement('contentY').reset(this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height'))
            }
            this.$mouseX = x
            this.$mouseY = y
        } else {
            return true
        }
    }

    onWheel(x, y, deltaX, deltaY){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')){
            if(this.getPropertyValue('contentX') + (deltaX) > 0 && this.getPropertyValue('contentX') + (deltaX) < this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width')){
                this.getStatement('contentX').reset(this.getPropertyValue('contentX') + (deltaX))
            } else {
                if(this.getPropertyValue('contentX') + (deltaX) <= 0) this.getStatement('contentX').reset(0)
                if(this.getPropertyValue('contentX') + (deltaX) >= this.getPropertyValue('contentItem').getPropertyValue('width')) this.getStatement('contentX').reset(this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width'))
            }

            if(this.getPropertyValue('contentY') + (deltaY) > 0 && this.getPropertyValue('contentY') + (deltaY) < this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height')){
                this.getStatement('contentY').reset(this.getPropertyValue('contentY') + (deltaY))
                return false
            } else {
                if(this.getPropertyValue('contentY') + (deltaY) <= 0) this.getStatement('contentY').reset(0)
                if(this.getPropertyValue('contentY') + (deltaY) >= this.getPropertyValue('contentItem').getPropertyValue('height')) this.getStatement('contentY').reset(this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height'))
                return true
            }
        } else {
            return true
        }
    }

    $destroy(){
        super.$destroy()
        MouseController.remove(this)
    }
}

module.exports.Flickable = Flickable