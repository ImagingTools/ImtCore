const { Flickable } = require('./Flickable')
const { QVar, QReal } = require('../utils/properties')

class GridView extends Flickable {
    static Beginning = 0
    static Center = 1
    static End = 2

    static LeftToRight = 0
    static RightToLeft = 1
    static TopToBottom = 2
    static BottomToTop = 3

    static defaultProperties = {
        model: { type: QVar, value: undefined, changed: '$modelChanged' },
        delegate: { type: QVar, changed: '$delegateChanged' },
        layoutDirection: { type: QReal, value: GridView.LeftToRight, changed: '$directionChanged' },
        verticalLayoutDirection: { type: QReal, value: GridView.TopToBottom, changed: '$directionChanged' },
        currentIndex: { type: QReal, value: -1 },
        currentItem: { type: QReal, value: undefined },
        count: { type: QReal, value: 0 },
        cellWidth: { type: QReal, value: 100, changed: '$cellChanged' },
        cellHeight: { type: QReal, value: 100, changed: '$cellChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx
        this.getStatement('contentItem').get().setStyle({
            justifyContent: 'flex-start',
            alignItems: 'flex-start',
            flexWrap: 'wrap'
        })
        this.getStatement('contentItem').get().getProperty('width').setCompute(()=>{this.getStatement('contentItem').get().getProperty('width').subscribe(this.getProperty('width')); return this.getProperty('width').get()})
        this.getStatement('contentItem').get().getProperty('width').update()
        this.$items = []
    }

    itemAtIndex(index){
        return index >= 0 && index < this.$items.length ? this.$items[index] : undefined
    }
    positionViewAtBeginning(){
        this.positionViewAtIndex(0, GridView.Beginning)
    }
    positionViewAtEnd(){
        this.positionViewAtIndex(this.$items.length-1, GridView.Beginning)

        
    }
    positionViewAtIndex(index, mode){
        let pos = 'start'
        switch(mode){
            case GridView.Beginning: pos = 'start'; break;
            case GridView.Center: pos = 'center'; break;
            case GridView.End: pos = 'end'; break;
        }

    }

    $widthChanged(){
        super.$widthChanged()
        this.updateGeometry()
    }

    $heightChanged(){
        super.$heightChanged()
        this.updateGeometry()
    }

    $contentWidthChanged(){
        this.getStatement('contentItem').get().getStatement('width').reset(this.getStatement('contentWidth').get())
        // if(this.getPropertyValue('contentX') > this.getPropertyValue('contentWidth') - this.getPropertyValue('width')){
        //     this.getProperty('contentX').reset(this.getPropertyValue('contentWidth') - this.getPropertyValue('width'))
        // }
    }

    $contentHeightChanged(){
        this.getStatement('contentItem').get().getStatement('height').reset(this.getStatement('contentHeight').get())
        // if(this.getPropertyValue('contentY') > this.getPropertyValue('contentHeight') - this.getPropertyValue('height')){
        //     this.getProperty('contentY').reset(this.getPropertyValue('contentHeight') - this.getPropertyValue('height'))
        // }
    }

    $modelChanged(){
        this.updateView()
    }

    $delegateChanged(){
        this.updateView()
    }

    createElement(index){
        if(this.$items[index]) return this.$items[index]
        let ctx = new ContextController(this.delegate.get().$exCtx,this.$exCtx)
        let createObject = this.getStatement('delegate').get().createObject
        let cls = this.getStatement('delegate').get().constructor

        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = createObject ? createObject(this.contentItem,ctx, {index: index}, false) : new cls(this.contentItem,ctx, {index: index})

            this.$items[index] = obj
        } else if(Array.isArray(this.getPropertyValue('model'))){
            let obj = createObject ? createObject(this.contentItem,ctx, {'$modelData': this.getPropertyValue('model')[index], index: index}, false) : new cls(this.contentItem,ctx, {'$modelData': this.getPropertyValue('model')[index], index: index})

            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = createObject ? createObject(this.contentItem,ctx, model, false) : new cls(this.contentItem,ctx, model)

            this.$items[index] = obj   
        }

        for(let update of updateList.splice(0, updateList.length)){
            update()
        }
        
        this.$items[index].$complete()
        return this.$items[index]
    }

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return

        for(let item of this.$items){
            item.destroy()
        }
        this.$items = []

        let model = this.getPropertyValue('model')
        let length = 0 
        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(typeof model === 'object' && Array.isArray(model)){
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        } else {
            return
        }

        if(length === 0) {
            return
        }

        for(let i = 0; i < length; i++){
            this.createElement(i)
        }

        this.getProperty('count').reset(this.$items.length)
        this.updateGeometry()
    }

    $calcContentGeometry(){
        
        // this.contentItem.dom.style.minHeight = '0px'
        // this.contentItem.dom.style.height = '0px'
        // this.contentHeight = this.contentItem.dom.scrollHeight
        // this.contentItem.dom.style.minHeight = `${this.contentItem.dom.scrollHeight}px`
        // this.contentItem.dom.style.height = `${this.contentItem.dom.scrollHeight}px`

        this.getPropertyValue('contentItem').getProperty('width').setAuto(this.getPropertyValue('width'))
        this.getProperty('contentWidth').reset(-1)
        let childLength = typeof this.getPropertyValue('model') === 'number' ? this.getPropertyValue('model') : this.getPropertyValue('contentItem').getProperty('children').get().length
        let rowLength = this.getPropertyValue('width') / this.getPropertyValue('cellWidth')  <= 1 ? 1 : Math.trunc(this.getPropertyValue('width') / this.getPropertyValue('cellWidth'))
        let contentHeight = Math.ceil(childLength / rowLength) * this.getPropertyValue('cellHeight')

        this.getProperty('contentHeight').reset(contentHeight)
    }

    updateGeometry(){
        // this.getProperty('contentItem').get().setStyle({
        //     // width: `0px`,
        //     // minWidth: `0px`,
        //     height: `0px`,
        //     minHeight: `0px`,
        // })

        // // this.getProperty('width').setAuto(this.getStatement('contentItem').get().getDom().scrollWidth)
        // // this.getProperty('height').setAuto(this.getStatement('contentItem').get().getDom().scrollHeight)

        
        // this.getProperty('contentHeight').setAuto(this.getProperty('contentItem').get().getDom().scrollHeight)

        // this.getProperty('contentItem').get().setStyle({
        //     // width: `${this.getProperty('width').get()}px`,
        //     // minWidth: `${this.getProperty('width').get()}px`,
        //     height: `${this.getProperty('contentItem').get().getDom().scrollHeight}px`,
        //     minHeight: `${this.getProperty('contentItem').get().getDom().scrollHeight}px`,
        // })

        this.$calcContentGeometry()
  
        this.$updateChildren()
    }

    $updateChildren(){
        let x = 0
        let y = 0
        for(let i = 0; i < this.getPropertyValue('contentItem').getProperty('children').get().length; i++){
            let child = this.getPropertyValue('contentItem').getProperty('children').get()[i]

            if(i > 0) {
                x += this.cellWidth
                if(x + this.cellWidth > this.width){
                    x = 0
                    y += this.cellHeight
                }
            }

            child.x = x
            child.y = y
        }
    }

    $cellChanged(){
        this.updateGeometry()
        // for(let child of this.getProperty('contentItem').get().children()){
        //     child.getProperty('width').setCompute(this.getPropertyValue('cellWidth'))
        // }
    }

    $directionChanged(){
        this.updateGeometry()
    }

    destroy(){
        for(let item of this.$items){
            item.destroy()
        }
        super.destroy()
    }

}

module.exports.GridView = GridView