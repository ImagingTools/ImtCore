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
        model: {type: Var, value:undefined, },
        delegate: {type: Var, value:undefined, },
        layoutDirection: {type: Real, value:GridView.LeftToRight, },
        verticalLayoutDirection: {type: Real, value:GridView.TopToBottom, },
        currentIndex: {type: Real, value:-1, },
        currentItem: {type: Var, value:undefined, },
        count: {type: Real, value:0, },
        cellWidth: {type: Real, value:100, },
        cellHeight: {type: Real, value:100, },
        
        modelChanged: {type:Signal, args:[]},
        delegateChanged: {type:Signal, args:[]},
        layoutDirectionChanged: {type:Signal, args:[]},
        verticalLayoutDirectionChanged: {type:Signal, args:[]},
        currentIndexChanged: {type:Signal, args:[]},
        currentItemChanged: {type:Signal, args:[]},
        countChanged: {type:Signal, args:[]},
        cellWidthChanged: {type:Signal, args:[]},
        cellHeightChanged: {type:Signal, args:[]},
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('GridView')

        obj.contentItem.__endUpdate = ()=>{
            obj.__endUpdate()
        }

        return obj
    }

    __items = []

    __complete(){
        this.__initView(true)
        super.__complete()
    }

    indexAt(x, y){

    }
    itemAt(x, y){

    }

    itemAtIndex(index){
        return index >= 0 && index < this.__items.length.get() ? this.__items[index] : undefined
    }
    positionViewAtBeginning(){
        this.positionViewAtIndex(0, GridView.Beginning)
    }
    positionViewAtEnd(){
        this.positionViewAtIndex(this.__items.length.get()-1, GridView.Beginning)
    }
    positionViewAtIndex(index, mode){
        // let pos = 'start'
        // switch(mode){
        //     case GridView.Beginning: pos = 'start'; break;
        //     case GridView.Center: pos = 'center'; break;
        //     case GridView.End: pos = 'end'; break;
        //     case GridView.Visible: break;
        //     case GridView.Contain: {
        //         if(this.__items[index]){
        //             if(this.orientation') === GridView.Horizontal){
        //                 if(this.contentWidth') <= this.width')) return

        //                 if(this.__items[index].x') <= this.contentX')){
        //                     this.getProperty('contentX').reset(this.__items[index].x'))
        //                 } else if(this.__items[index].x') + this.__items[index].width') >= this.contentX') + this.width')){
        //                     this.getProperty('contentX').reset(this.__items[index].x') + this.__items[index].width') - this.width'))
        //                 }
        //             } else {
        //                 if(this.contentHeight') <= this.height')) return

        //                 if(this.__items[index].y') <= this.contentY')){
        //                     this.getProperty('contentY').reset(this.__items[index].y'))
        //                 } else if(this.__items[index].y') + this.__items[index].height') >= this.contentY') + this.height')){
        //                     this.getProperty('contentY').reset(this.__items[index].y') + this.__items[index].height') - this.height'))
        //                 }
        //             }
        //         }
        //         break;
        //     }
        // }

    }

    SLOT_modelChanged(oldValue, newVlaue) {
        this.__clear()

        if (oldValue && typeof oldValue === 'object' && !Array.isArray(oldValue) && !oldValue.__destroyed) {
            oldValue.__removeViewListener(this)
        }

        if (newVlaue && typeof newVlaue === 'object' && !Array.isArray(newVlaue)) {
            newVlaue.__addViewListener(this)
        }

        this.__initView(this.__completed)
    }

    SLOT_delegateChanged(oldValue, newValue){
        this.__clear()
        this.__initView(this.__completed)
    }

    __clear(){
        this.blockSignals(true)

        let removed = this.__items
        this.__items = []

        for(let r of removed){
            if(r) r.destroy()
        }


        this.originX = 0
        this.originY = 0
        
        this.contentX = 0
        this.contentY = 0

        this.blockSignals(false)
    }

    __createItem(index){
        let properties = {}

        if (Array.isArray(this.model)) {
            properties.modelData = this.model[index]
            properties.model = {index:index}
        } else if(typeof this.model === 'object'){
            properties.model = this.model.data[index]
        } else {
            properties.model = {index:index}
        }

        let item = this.delegate.createObject(this.contentItem, properties)

        this.__items[index] = item

        return item
    }

    __initView(isCompleted){
        if(this.delegate && this.model && isCompleted){
            let length = 0 
            if (Array.isArray(this.model)) {
                length = this.model.length
            } else if(typeof this.model === 'object'){     
                if(this.model.__changeSet.length > 0) return
                length = this.model.count
            } else if(typeof this.model === 'number'){
                length = this.model
            } else {
                return
            }

            if(length === 0) return

            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            let countChanged = false

            if(this.count !== length){
                countChanged = true 
            }

            this.__self.count = length

            for(let i = 0; i < length; i++){
                this.__createItem(i)
            }

            if(countChanged) this.countChanged()

            JQApplication.endUpdate()
        }
    }

    __updateView(changeSet){
        if(this.delegate && this.model && this.__completed){
            let length = 0 
            if (Array.isArray(this.model)) {
                length = this.model.length
            } else if(typeof this.model === 'object'){     
                length = this.model.count
            } else if(typeof this.model === 'number'){
                length = this.model
            } else {
                return
            }

            if(length === 0) return
            
            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            let countChanged = false

            if(this.count !== length){
                countChanged = true 
            }

            this.__self.count = length

            for(let change of changeSet){
                let leftTop = change[0]
                let bottomRight = change[1]
                let role = change[2]

                if(role === 'append'){
                    for(let i = leftTop; i < bottomRight; i++){
                        this.__createItem(i)
                    }
                } else if(role === 'insert'){
                    for(let i = leftTop; i < bottomRight; i++){
                        this.__items.splice(i, 0, undefined)
                        this.__createItem(i)
                    }
                } else if(role === 'remove'){
                    let removed = this.__items.splice(leftTop, bottomRight - leftTop)
                    for(let r of removed){
                        if(r) r.destroy()
                    }
                }
            }

            if(countChanged) this.countChanged()

            JQApplication.endUpdate()
        }
    }

    __updateGrid(){
        let length = 0 
        if (Array.isArray(this.model)) {
            length = this.model.length
        } else if(typeof this.model === 'object'){     
            length = this.model.count
        } else if(typeof this.model === 'number'){
            length = this.model
        } else {
            return
        }

        // if(length === 0) return

        let columns = Math.trunc(this.width / this.cellWidth)
        if(columns <= 0) columns = 1
        // let rows = Math.trunc(length / columns)
        // if(rows <= 0) rows = 1

        let index = 0
        let rowIndex = 0
        let columnIndex = 0

        while(index < length){
            if(!this.__items[index]) {
                index++
                continue
            }

            this.__items[index].x = this.cellWidth*columnIndex
            this.__items[index].y = this.cellHeight*rowIndex

            columnIndex++
            if(columnIndex >= columns){
                columnIndex = 0
                rowIndex++
            }

            index++
        }

    }

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)

        JQApplication.updateLater(this)
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)

        JQApplication.updateLater(this)
    }

    SLOT_cellWidthChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_cellHeightChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_contentWidthChanged(oldValue, newValue){
        
    }

    SLOT_contentHeightChanged(oldValue, newValue){
        
    }

    __updateGeometry(){
        this.contentWidth = this.contentItem.__DOM.scrollWidth
        this.contentHeight = this.contentItem.__DOM.scrollHeight
      
    }

    __endUpdate(){
        this.__updateGrid()
        this.__updateGeometry()
        super.__endUpdate()
    }

    __destroy(){
        if (this.model && typeof this.model === 'object' && !Array.isArray(this.model) && !this.model.__destroyed) {
            this.model.__removeViewListener(this)
        }
        this.__clear()
        super.__destroy()
    }
}



module.exports = GridView