const Flickable = require("./Flickable")
const Component = require("../QtQml/Component")
const Variant = require("../QtQml/Variant")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")
const JQApplication = require("../core/JQApplication")

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
        delegate: {type:Variant, typeTarget:Component, value:undefined, signalName:'delegateChanged'},
        contentX: {type: Real, value:0, signalName:'contentXChanged'},
        contentY: {type: Real, value:0, signalName:'contentYChanged'},
        orientation: {type: Real, value:ListView.Vertical, signalName:'orientationChanged'},
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        currentIndex: {type: Real, value:-1, signalName:'currentIndexChanged'},
        currentItem: {type: Var, value:undefined, signalName:'currentItemChanged'},
        snapMode: {type: Real, value:ListView.NoSnap, signalName:'snapModeChanged'},
        cacheBuffer: {type: Real, value:320, signalName:'cacheBufferChanged'},
        count: {type: Real, value:0, signalName:'countChanged' },
        contentWidth: {type: Real, value:0, signalName:'contentWidthChanged'},
        contentHeight: {type: Real, value:0, signalName:'contentHeightChanged'},
        
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

    __items = []

    __middleWidth = 0
    __middleHeight = 0

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
        this.positionViewAtIndex(0, ListView.Beginning)
    }
    positionViewAtEnd(){
        this.positionViewAtIndex(this.__items.length.get()-1, ListView.Beginning)

        
    }
    positionViewAtIndex(index, mode){
        // let pos = 'start'
        // switch(mode){
        //     case ListView.Beginning: pos = 'start'; break;
        //     case ListView.Center: pos = 'center'; break;
        //     case ListView.End: pos = 'end'; break;
        //     case ListView.Visible: break;
        //     case ListView.Contain: {
        //         if(this.__items[index]){
        //             if(this.orientation') === ListView.Horizontal){
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

    onModelChanged(){
        this.__clear()
        
        if(this.__model && typeof this.__model === 'object' && !this.__model.__destroyed){
            this.__model.__removeViewListener(this)
        }

        if(this.model && typeof this.model === 'object'){
            this.model.__addViewListener(this)
            this.__model = this.model
        }

        this.__initView(this.__completed)
    }

    onDelegateChanged(){
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

        // this.__middleWidth = 0
        // this.__middleHeight = 0

        this.originX = 0
        this.originY = 0
        
        this.contentX = 0
        this.contentY = 0

        this.blockSignals(false)
    }

    __getItemInfo(index){
        let x = 0
        let y = 0
        let width = 0
        let height = 0
        let exist = false
        let inner = false

        if(this.__items[index]) {
            exist = true

            if(this.orientation === ListView.Horizontal){
                x = this.__items[index].x
                width = this.__items[index].width

                if(x + width < this.contentX - this.cacheBuffer || x > this.contentX + this.width + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            } else {
                y = this.__items[index].y
                height = this.__items[index].height

                if(y + height < this.contentY - this.cacheBuffer || y > this.contentY + this.height + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            }
        } else {
            exist = false

            if(this.orientation === ListView.Horizontal){
                x = 0
                if(this.__items[index-1]) {
                    x = this.__items[index-1].x+this.__items[index-1].width+this.spacing
                } else if(this.__items[index+1]){
                    x = this.__items[index+1].x-this.__middleWidth-this.spacing
                } else {
                    if(index === 0){
                        x = this.originX
                    } else {
                        x = this.originX + (this.__middleWidth + this.spacing) * index 
                    }
                    
                }
                width = this.__middleWidth

                if(x + width < this.contentX - this.cacheBuffer || x > this.contentX + this.width + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            } else {
                y = 0
                if(this.__items[index-1]) {
                    y = this.__items[index-1].y+this.__items[index-1].height+this.spacing
                } else if(this.__items[index+1]){
                    y = this.__items[index+1].y-this.__middleHeight-this.spacing
                } else {
                    if(index === 0){
                        y = this.originY
                    } else {
                        y = this.originY + (this.__middleHeight + this.spacing) * index 
                    }   
                }
                height = this.__middleHeight

                if(y + height < this.contentY - this.cacheBuffer || y > this.contentY + this.height + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            }
        }

        return {
            x: x,
            y: y,
            width: width,
            height: height,
            exist: exist,
            inner: inner,
        }
    }

    __createItem(index, itemInfo){
        let model

        if(typeof this.model === 'object'){
            model = this.model.data[index]
        } else {
            model = {index:index}
        }

        let item = this.delegate.createObject(this.contentItem, model)

        this.__items[index] = item

        item.xChanged.connect(()=>{
            if(this.orientation === ListView.Horizontal){
                let _index = this.__items.indexOf(item)
                if(_index >= 0 && this.__items[_index+1]){
                    this.__items[_index+1].x = this.__items[_index].x+this.__items[_index].width+this.spacing
                }
            }
            // JQApplication.updateLater(this)
        })
        item.yChanged.connect(()=>{
            if(this.orientation === ListView.Vertical){
                let _index = this.__items.indexOf(item)
                if(_index >= 0 && this.__items[_index+1]){
                    this.__items[_index+1].y = this.__items[_index].y+this.__items[_index].height+this.spacing
                }
            }
            // JQApplication.updateLater(this)
        })
        item.widthChanged.connect(()=>{
            if(this.orientation === ListView.Horizontal){
                let _index = this.__items.indexOf(item)
                if(_index >= 0 && this.__items[_index+1]){
                    this.__items[_index+1].x = this.__items[_index].x+this.__items[_index].width+this.spacing
                }
            }
            // JQApplication.updateLater(this)
        })
        item.heightChanged.connect(()=>{
            if(this.orientation === ListView.Vertical){
                let _index = this.__items.indexOf(item)
                if(_index >= 0 && this.__items[_index+1]){
                    this.__items[_index+1].y = this.__items[_index].y+this.__items[_index].height+this.spacing
                }
            }
            // JQApplication.updateLater(this)
        })
        // item.visibleChanged.connect(()=>{
        //     JQApplication.updateLater(this)
        // })

        item.x = itemInfo.x
        item.y = itemInfo.y 

        return item
    }

    __initView(isCompleted){
        if(this.delegate && this.model && isCompleted){
            let length = 0 
            if(typeof this.model === 'object'){     
                length = this.model.count
            } else if(typeof this.model === 'number'){
                length = this.model
            } else {
                return
            }

            if(length === 0) return

            // JQApplication.beginUpdate()
            // JQApplication.updateLater(this)

            let countChanged = false

            for(let i = 0; i < length; i++){
                let itemInfo = this.__getItemInfo(i)
                if(itemInfo.inner){
                    if(!itemInfo.exist){
                        if(this.__createItem(i, itemInfo)) this.__updateGeometry()
                    }
                }
            }

            if(countChanged) this.countChanged()

            // JQApplication.endUpdate()
        }
    }

    __updateView(changeSet){
        if(this.delegate && this.model && this.__completed){
            let length = 0 
            if(typeof this.model === 'object'){     
                length = this.model.count
            } else if(typeof this.model === 'number'){
                length = this.model
            } else {
                return
            }

            if(length === 0) return
            
            // JQApplication.beginUpdate()
            // JQApplication.updateLater(this)

            let countChanged = false

            if(this.count !== length){
                countChanged = true
                this.__getDataQml('count').__value = length
            }

            for(let change of changeSet){
                let leftTop = change[0]
                let bottomRight = change[1]
                let role = change[2]

                if(role === 'append'){
                    for(let i = leftTop; i < bottomRight; i++){
                        let itemInfo = this.__getItemInfo(i)
                        if(itemInfo.inner){
                            if(!itemInfo.exist){
                                if(this.__createItem(i, itemInfo)) this.__updateGeometry()
                            }
                        }
                    }
                } else if(role === 'insert'){
                    for(let i = leftTop; i < bottomRight; i++){
                        this.__items.splice(i, 0, undefined)
                        let itemInfo = this.__getItemInfo(i)
                        if(itemInfo.inner){
                            if(!itemInfo.exist){
                                if(this.__createItem(i, itemInfo)) this.__updateGeometry()
                            }
                        } 
                    }
                } else if(role === 'remove'){
                    let removed = this.__items.splice(leftTop, bottomRight - leftTop)
                    for(let r of removed){
                        if(r) r.destroy()
                    }
                }
            }

            let firstIndex = 0
            let lastIndex = 0

            for(let i = 0; i < length; i++){
                if(this.__items[i] && !this.__items[i-1] && !firstIndex) firstIndex = i
                if(this.__items[i] && !this.__items[i+1] && !lastIndex) lastIndex = i
            }

            for(let i = firstIndex; i < length; i++){
                let itemInfo = this.__getItemInfo(i)
                if(itemInfo.inner){
                    if(!itemInfo.exist){
                        if(this.__items[i] = this.__createItem(i, itemInfo)) this.__updateGeometry() 
                    }
                } else if(itemInfo.exist){
                    this.__items[i].destroy()
                    this.__items[i] = undefined
                }
                
            }
            for(let i = lastIndex; i >= 0; i--){
                let itemInfo = this.__getItemInfo(i)
                if(itemInfo.inner){
                    if(!itemInfo.exist){
                        if(this.__items[i] = this.__createItem(i, itemInfo)) this.__updateGeometry()
                    }
                } else if(itemInfo.exist){
                    this.__items[i].destroy()
                    this.__items[i] = undefined
                }
            }

            if(countChanged) this.countChanged()

            // JQApplication.endUpdate()
        }
    }

    onCacheBufferChanged(){
        this.__updateView([])
    }

    onContentXChanged(){
        super.onContentXChanged()

        if(this.orientation === ListView.Horizontal){
            this.__updateView([])
        }
    }

    onContentYChanged(){
        super.onContentYChanged()

        if(this.orientation === ListView.Vertical){
            this.__updateView([])
        }
    }

    onWidthChanged(){
        super.onWidthChanged()

        if(this.orientation === ListView.Horizontal){
            this.__updateView([])
        }
    }

    onHeightChanged(){
        super.onHeightChanged()

        if(this.orientation === ListView.Vertical){
            this.__updateView([])
        }
    }

    onSpacingChanged(){
        // JQApplication.updateLater(this)
    }

    onOrientationChanged(){
        for(let i = 0; i < this.__items.length; i++){
            let itemInfo = this.__getItemInfo(i)

            if(itemInfo.exist){
                if(this.orientation === ListView.Horizontal){
                    this.originY = 0
                } else {
                    this.originX = 0
                }

                this.__items[i].x = itemInfo.x
                this.__items[i].y = itemInfo.y
            }
            
        }
        this.__updateView([])
    }

    __updateGeometry(){
        if(!this.__items.length) return 

        let model = this.model
        if(typeof model === 'object'){     
            length = model.count
        } else if(typeof model === 'number'){
            length = model
        } else {
            return
        }
        
        let lastIndex = 0
        let firstIndex = this.__items.length-1
        let minX = Infinity
        let minY = Infinity

        let visibleCount = 0
        let visibleContentWidth = 0
        let visibleContentHeight = 0

        for(let i = 0; i < this.__items.length; i++){
            if(this.__items[i]){
                visibleCount++
                visibleContentWidth += this.__items[i].width
                visibleContentHeight += this.__items[i].height

                let x = this.__items[i].x
                let y = this.__items[i].y

                if(x < minX) minX = x
                if(y < minY) minY = y

                if(i < firstIndex) firstIndex = i
                if(i > lastIndex) lastIndex = i
            }
        }

        let middleWidth = visibleCount ? visibleContentWidth / visibleCount : 0
        let middleHeight = visibleCount ? visibleContentHeight / visibleCount : 0
        this.__middleWidth = middleWidth
        this.__middleHeight = middleHeight

        if(this.orientation === ListView.Horizontal){
            this.contentWidth = visibleContentWidth + Math.round(middleWidth)*(length-visibleCount) + this.spacing * (length-1)
            let originX = (minX - firstIndex*(Math.round(middleWidth+this.spacing)))
            if(originX !== Infinity && originX !== -Infinity) this.originX = originX

            this.__getDataQml('contentHeight').__setAuto(this.height)
        } else {
            this.contentHeight = visibleContentHeight + Math.round(middleHeight)*(length-visibleCount) + this.spacing * (length-1)
            let originY = (minY - firstIndex*(Math.round(middleHeight+this.spacing)))
            if(originY !== Infinity && originY !== -Infinity) this.originY = originY

            this.__getDataQml('contentWidth').__setAuto(this.width)
        }  
    }

    // __endUpdate(){
    //     this.__updateGeometry()
    //     super.__endUpdate()
    // }

    __destroy(){
        if(this.__model && typeof this.__model === 'object' && !this.__model.__destroyed){
            this.__model.__removeViewListener(this)
        }
        this.__clear()
        super.__destroy()
    }
}

ListView.initialize()

module.exports = ListView