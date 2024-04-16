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

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        let contentItem = new Item(this)
        this.getStatement('contentItem').reset(contentItem)
        contentItem.getProperty('width').setCompute(()=>{return this.width})
        contentItem.getProperty('height').setCompute(()=>{return this.height})
        contentItem.getProperty('width').update()
        contentItem.getProperty('height').update()
        this.setStyle({
            pointerEvents: 'auto',
            touchAction: 'auto',
        })
        MouseController.add(this)
    }

    $contentXChanged(){
        let x = this.getStatement('contentX').get()

        if(x > this.getPropertyValue('contentWidth') - this.getPropertyValue('width')) x = this.getPropertyValue('contentWidth') - this.getPropertyValue('width')
        if(x < 0) x = 0
        
        this.getStatement('contentX').value = x
        this.getStatement('contentItem').get().getStatement('x').reset(-x)
    }

    $contentYChanged(){
        let y = this.getStatement('contentY').get()

        if(y > this.getPropertyValue('contentHeight') - this.getPropertyValue('height')) y = this.getPropertyValue('contentHeight') - this.getPropertyValue('height')
        if(y < 0) y = 0
        
        this.getStatement('contentY').value = y
        this.getStatement('contentItem').get().getStatement('y').reset(-y)
    }


    $contentWidthChanged(){
        this.getStatement('contentItem').get().getStatement('width').reset(this.getStatement('contentWidth').get())

        if(this.getPropertyValue('contentWidth') - this.getPropertyValue('width') <= 0){
            this.getProperty('contentX').reset(0)
        } else if(this.getPropertyValue('contentX') > this.getPropertyValue('contentWidth') - this.getPropertyValue('width')){
            this.getProperty('contentX').reset(this.getPropertyValue('contentWidth') - this.getPropertyValue('width'))
        }
    }

    $contentHeightChanged(){
        this.getStatement('contentItem').get().getStatement('height').reset(this.getStatement('contentHeight').get())
        if(this.getPropertyValue('contentHeight') - this.getPropertyValue('height') <= 0){
            this.getProperty('contentY').reset(0)
        } else if(this.getPropertyValue('contentY') > this.getPropertyValue('contentHeight') - this.getPropertyValue('height')){
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
            if(this.getPropertyValue('contentX') + (x) > 0 && this.getPropertyValue('contentX') + (x) < this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width')){
                this.getStatement('contentX').reset(this.getPropertyValue('contentX') + (x))
            } else {
                if(this.getPropertyValue('contentX') + (x) <= 0) this.getStatement('contentX').reset(0)
                if(this.getPropertyValue('contentX') + (x) >= this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width')) this.getStatement('contentX').reset(this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width'))
            }

            if(this.getPropertyValue('contentY') + (y) > 0 && this.getPropertyValue('contentY') + (y) < this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height')){
                this.getStatement('contentY').reset(this.getPropertyValue('contentY') + (y))
            } else {
                if(this.getPropertyValue('contentY') + (y) <= 0) this.getStatement('contentY').reset(0)
                if(this.getPropertyValue('contentY') + (y) >= this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height')) this.getStatement('contentY').reset(this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height'))
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
                if(this.getPropertyValue('contentX') + (deltaX) >= this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width')) this.getStatement('contentX').reset(this.getPropertyValue('contentItem').getPropertyValue('width') - this.getPropertyValue('width'))
            }

            if(this.getPropertyValue('contentY') + (deltaY) > 0 && this.getPropertyValue('contentY') + (deltaY) < this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height')){
                this.getStatement('contentY').reset(this.getPropertyValue('contentY') + (deltaY))
                return false
            } else {
                if(this.getPropertyValue('contentY') + (deltaY) <= 0) this.getStatement('contentY').reset(0)
                if(this.getPropertyValue('contentY') + (deltaY) >= this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height')) this.getStatement('contentY').reset(this.getPropertyValue('contentItem').getPropertyValue('height') - this.getPropertyValue('height'))
                return true
            }
        } else {
            return true
        }
    }

    destroy(){
        MouseController.remove(this)
        super.destroy()
    }

}

module.exports.Flickable = Flickable