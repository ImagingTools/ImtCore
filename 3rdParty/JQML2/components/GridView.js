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

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return

        for(let item of this.$items){
            item.$destroy()
        }
        this.$items = []
        let ctx = new ContextController(this.delegate.get().$exCtx, this.$exCtx)
        if(typeof this.getPropertyValue('model') === 'number'){
            for(let i = 0; i < this.getPropertyValue('model'); i++){
                let obj = this.delegate.get().createObject(this.getStatement('contentItem').get(), ctx, {index: i})
                // obj.setStyle({
                //     position: 'relative'
                // })
                // obj.getProperty('width').setCompute(()=>{obj.getProperty('width').subscribe(this.getProperty('cellWidth')); return this.getProperty('cellWidth').get()})
                // obj.getProperty('height').setCompute(()=>{obj.getProperty('height').subscribe(this.getProperty('cellHeight')); return this.getProperty('cellHeight').get()})
                // obj.getProperty('width').getNotify().connect(()=>{
                //     this.updateGeometry()
                // })
                // obj.getProperty('height').getNotify().connect(()=>{
                //     this.updateGeometry()
                // })
                // obj.getStatement('index').reset(i)
                // obj.getStatement('model').reset({index: i})
                for(let update of updateList.splice(0, updateList.length)){
                    update()
                }
                obj.$complete()
                this.$items.push(obj)
            }
        } else {
            for(let model of this.getPropertyValue('model').getPropertyValue('data')){
                let obj = this.delegate.get().createObject(this.getStatement('contentItem').get(), ctx, model)
                
                // obj.setStyle({
                //     position: 'relative'
                // })
                // obj.getProperty('width').setCompute(()=>{obj.getProperty('width').subscribe(this.getProperty('cellWidth')); return this.getProperty('cellWidth').get()})
                // obj.getProperty('height').setCompute(()=>{obj.getProperty('height').subscribe(this.getProperty('cellHeight')); return this.getProperty('cellHeight').get()})
                // obj.getProperty('width').getNotify().connect(()=>{
                //     this.updateGeometry()
                // })
                // obj.getProperty('height').getNotify().connect(()=>{
                //     this.updateGeometry()
                // })
                // obj.getStatement('index').setCompute(()=>{obj.getStatement('index').subscribe(model.getStatement('index')); return model.getStatement('index').get()})
                // obj.getStatement('index').update()
                // obj.getStatement('model').reset(model)
                for(let update of updateList.splice(0, updateList.length)){
                    update()
                }
                obj.$complete()
                this.$items.push(obj)
            }
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
        let childLength = typeof this.getPropertyValue('model') === 'number' ? this.getPropertyValue('model') : this.getPropertyValue('contentItem').$children.length
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
        for(let i = 0; i < this.getPropertyValue('contentItem').$children.length; i++){
            let child = this.getPropertyValue('contentItem').$children[i]

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

    $destroy(){
        for(let item of this.$items){
            item.$destroy()
        }
        super.$destroy()
    }

}

module.exports.GridView = GridView