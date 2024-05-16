const { Flickable } = require('./Flickable')
const { QVar, QReal, QAutoGeometry } = require('../utils/properties')
const { ListModel } = require('./ListModel')

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

    static defaultProperties = {
        model: { type: QVar, value: undefined, changed: '$modelChanged' },
        delegate: { type: QVar, changed: '$delegateChanged' },
        contentX: { type: QReal, value: 0, changed: '$contentXChanged' },
        contentY: { type: QReal, value: 0, changed: '$contentYChanged' },
        orientation: { type: QReal, value: ListView.Vertical, changed: '$orientationChanged' },
        spacing: { type: QReal, value: 0, changed: '$spacingChanged' },
        currentIndex: { type: QReal, value: -1, changed: '$currentIndexChanged' },
        currentItem: { type: QReal, value: undefined },
        snapMode: { type: QReal, value: ListView.NoSnap, changed: '$snapModeChanged' },
        cacheBuffer: { type: QReal, value: 320, changed: '$cacheBufferChanged' },
        count: { type: QReal, value: 0 },
        contentWidth: { type: QAutoGeometry, value: 0, changed: '$contentWidthChanged' },
        contentHeight: { type: QAutoGeometry, value: 0, changed: '$contentHeightChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx

        this.middleWidth = 0
        this.middleHeight = 0

        this.$items = []
    }

    $complete(){
        this.$ready = true
        if(this.$needUpdate) this.$updateView()
        if(this.$completed) return

        this.$completed = true
        if(this.$signals['Component.completed']) this.$signals['Component.completed']()
        let data = this.getProperty('data').get()
        for(let i = data.length - 1; i >= 0; i--){
            if(data[i] instanceof QObject) data[i].$complete()
        }
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

    $disconnectModel(){
        if(this.$model && this.$model instanceof ListModel && this.$model.UID){
            this.$model.getSignal('$transaction').disconnect(this, this.$transaction)
        }
    }

    $connectModel(model){
        if(model && model instanceof ListModel){
            this.$model = model
            model.getSignal('$transaction').connect(this, this.$transaction)
        }
    }

    $transaction(sender, changeset){
        if(changeset && sender === this.getPropertyValue('model') && sender.getPropertyValue('count') !== this.getPropertyValue('count')){
            let model = this.getPropertyValue('model')
            let length = 0 
            if(model instanceof ListModel){     
                length = model.getPropertyValue('count')
            } else if(typeof model === 'number'){
                length = model
            } else {
                return
            }
            let countChanged = this.getPropertyValue('count') !== length
            this.getProperty('count').value = length

            for(let change of changeset){
                let leftTop = change[0]
                let bottomRight = change[1]
                let roles = change[2]

                if(roles === 'append'){

                } else if(roles === 'insert'){
                    for(let i = leftTop; i < bottomRight; i++){
                        this.$items.splice(i, 0, undefined)
                        let info = this.$getItemInfo(i)
                        if(info.inner && !info.exist){
                            this.$createElement(i, info)
                        }
                    }
                } else if(roles === 'remove'){
                    let removed = this.$items.splice(leftTop, bottomRight - leftTop)

                    if(this.$items[leftTop] && removed.length){
                        this.$items[leftTop].getProperty('x').reset(removed[0].getPropertyValue('x'))
                        this.$items[leftTop].getProperty('y').reset(removed[0].getPropertyValue('y'))
                    }

                    for(let item of removed){
                        if(item) item.destroy()
                    } 
                }
            }

            if(countChanged && this.getProperty('count').notify){
                this.getProperty('count').notify()
            }

            this.$updateView()
            this.$updateGeometry()
        }
        
    }

    $clear(){
        while(this.$items.length){
            let obj = this.$items.shift()
            if(obj) obj.destroy()
        }

        this.getProperty('contentX').reset(0)
        this.getProperty('contentY').reset(0)
        this.getProperty('originX').reset(0)
        this.getProperty('originY').reset(0)
        this.getProperty('count').reset(0)
    }

    $modelChanged(){
        this.$disconnectModel()
        this.$clear()

        let model = this.getPropertyValue('model')

        if(model instanceof ListModel){     
            this.$connectModel(model)
        }

        this.$updateView()
    }

    $delegateChanged(){
        this.$clear()
        this.$updateView()
    }

    $spacingChanged(){
        // this.$updateView()
    }

    $updateView(){
        if(!this.$ready){
            this.$needUpdate = true
            return
        }
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return
        
        let model = this.getPropertyValue('model')
        let length = 0 
        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(typeof model === 'number'){
            length = model
        } else {
            return
        }

        if(length === 0) return

        let countChanged = this.getPropertyValue('count') !== length
        this.getProperty('count').value = length

        let firstIndex = 0
        let lastIndex = 0

        for(let i = 0; i < length; i++){
            if(this.$items[i] && !this.$items[i-1] && !firstIndex) firstIndex = i
            if(this.$items[i] && !this.$items[i+1] && !lastIndex) lastIndex = i
        }

        for(let i = firstIndex; i < length; i++){
            let info = this.$getItemInfo(i)
            if(info.inner){
                if(!info.exist){
                    if(this.$createElement(i, info)) this.$updateGeometry()
                }
            } else if(info.exist){
                this.$items[i].destroy()
                this.$items[i] = undefined
            }
            
        }
        for(let i = lastIndex; i >= 0; i--){
            let info = this.$getItemInfo(i)
            if(info.inner){
                if(!info.exist){
                    if(this.$createElement(i, info)) this.$updateGeometry()
                }
            } else if(info.exist){
                this.$items[i].destroy()
                this.$items[i] = undefined
            }
        }

        if(countChanged && this.getProperty('count').notify){
            this.getProperty('count').notify()
        }

    }

    $updateGeometry(){
        if(!this.$items.length) return 
        
        let lastIndex = 0
        let firstIndex = this.$items.length-1
        let minX = Infinity
        let minY = Infinity

        let visibleCount = 0
        let visibleContentWidth = 0
        let visibleContentHeight = 0

        for(let i = 0; i < this.$items.length; i++){
            if(this.$items[i]){
                visibleCount++
                visibleContentWidth += this.$items[i].getPropertyValue('width')
                visibleContentHeight += this.$items[i].getPropertyValue('height')

                let x = this.$items[i].getPropertyValue('x')
                let y = this.$items[i].getPropertyValue('y')

                if(x < minX) minX = x
                if(y < minY) minY = y

                if(i < firstIndex) firstIndex = i
                if(i > lastIndex) lastIndex = i
            }
        }

        let middleWidth = visibleCount ? visibleContentWidth / visibleCount : 0
        let middleHeight = visibleCount ? visibleContentHeight / visibleCount : 0
        this.middleWidth = middleWidth
        this.middleHeight = middleHeight

        if(this.getPropertyValue('orientation') === ListView.Horizontal){
            this.getStatement('contentWidth').reset(visibleContentWidth + Math.round(middleWidth)*(this.getPropertyValue('count')-visibleCount) + this.getPropertyValue('spacing') * (this.getPropertyValue('count')-1))
            let originX = (minX - firstIndex*(Math.round(middleWidth+this.getPropertyValue('spacing'))))
            if(originX !== Infinity && originX !== -Infinity) this.getStatement('originX').reset(originX)

            this.getProperty('contentHeight').setAuto(this.getPropertyValue('height'))
        } else {
            this.getStatement('contentHeight').reset(visibleContentHeight + Math.round(middleHeight)*(this.getPropertyValue('count')-visibleCount) + this.getPropertyValue('spacing') * (this.getPropertyValue('count')-1))
            let originY = (minY - firstIndex*(Math.round(middleHeight+this.getPropertyValue('spacing'))))
            if(originY !== Infinity && originY !== -Infinity) this.getStatement('originY').reset(originY)

            this.getProperty('contentWidth').setAuto(this.getPropertyValue('width'))
        }  
    }

    $orientationChanged(){
        for(let i = 0; i < this.$items.length; i++){
            let info = this.$getItemInfo(i)

            if(info.exist){
                if(this.getPropertyValue('orientation') === ListView.Horizontal){
                    this.getProperty('originY').reset(0)
                } else {
                    this.getProperty('originX').reset(0)
                }

                this.$items[i].getProperty('x').reset(info.x)
                this.$items[i].getProperty('y').reset(info.y)
            }
            
        }
        this.$updateView()
    }

    $contentXChanged(){
        this.getPropertyValue('contentItem').getProperty('x').reset(-this.getPropertyValue('contentX'))
        this.$updateView()
    }

    $contentYChanged(){
        this.getPropertyValue('contentItem').getProperty('y').reset(-this.getPropertyValue('contentY'))
        this.$updateView()
    }

    $contentWidthChanged(){
        this.getPropertyValue('contentItem').getProperty('width').reset(this.getPropertyValue('contentWidth'))
        if(this.getPropertyValue('contentWidth') < this.getPropertyValue('width')){
            if(this.$items[0]){
                this.getProperty('contentX').reset(this.$items[0].getPropertyValue('x'))
            } else {
                this.getProperty('contentX').reset(this.getPropertyValue('originX'))
            }
        }
    }

    $contentHeightChanged(){
        this.getPropertyValue('contentItem').getProperty('height').reset(this.getPropertyValue('contentHeight'))
        if(this.getPropertyValue('contentHeight') < this.getPropertyValue('height')){
            if(this.$items[0]){
                this.getProperty('contentY').reset(this.$items[0].getPropertyValue('y'))
            } else {
                this.getProperty('contentY').reset(this.getPropertyValue('originY'))
            }
        }
    }
    $widthChanged(){
        super.$widthChanged()
        if(this.getPropertyValue('orientation') === ListView.Vertical) this.getProperty('contentWidth').reset(this.getPropertyValue('width'))
        this.$updateView()
    }
    $heightChanged(){
        super.$heightChanged()
        if(this.getPropertyValue('orientation') === ListView.Horizontal) this.getProperty('contentHeight').reset(this.getPropertyValue('height'))
        this.$updateView()
    }
    $cacheBufferChanged(){
        this.$updateView()
    }

    $getItemInfo(index){
        let x = 0
        let y = 0
        let width = 0
        let height = 0
        let exist = false
        let inner = false

        if(this.$items[index]) {
            exist = true

            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                x = this.$items[index].getPropertyValue('x')
                width = this.$items[index].getPropertyValue('width')

                if(x + width < this.getPropertyValue('contentX') - this.getPropertyValue('cacheBuffer') || x > this.getPropertyValue('contentX') + this.getPropertyValue('width') + this.getPropertyValue('cacheBuffer')) {
                    inner = false
                } else {
                    inner = true
                }
            } else {
                y = this.$items[index].getPropertyValue('y')
                height = this.$items[index].getPropertyValue('height')

                if(y + height < this.getPropertyValue('contentY') - this.getPropertyValue('cacheBuffer') || y > this.getPropertyValue('contentY') + this.getPropertyValue('height') + this.getPropertyValue('cacheBuffer')) {
                    inner = false
                } else {
                    inner = true
                }
            }
        } else {
            exist = false

            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                x = 0
                if(this.$items[index-1]) {
                    x = this.$items[index-1].getPropertyValue('x')+this.$items[index-1].getPropertyValue('width')+this.getPropertyValue('spacing')
                } else if(this.$items[index+1]){
                    x = this.$items[index+1].getPropertyValue('x')-this.middleWidth-this.getPropertyValue('spacing')
                } else {
                    if(index === 0){
                        x = this.getPropertyValue('originX')
                    } else {
                        x = this.getPropertyValue('originX') + (this.middleWidth + this.getPropertyValue('spacing')) * index 
                    }
                    
                }
                width = this.middleWidth

                if(x + width < this.getPropertyValue('contentX') - this.getPropertyValue('cacheBuffer') || x > this.getPropertyValue('contentX') + this.getPropertyValue('width') + this.getPropertyValue('cacheBuffer')) {
                    inner = false
                } else {
                    inner = true
                }
            } else {
                y = 0
                if(this.$items[index-1]) {
                    y = this.$items[index-1].getPropertyValue('y')+this.$items[index-1].getPropertyValue('height')+this.getPropertyValue('spacing')
                } else if(this.$items[index+1]){
                    y = this.$items[index+1].getPropertyValue('y')-this.middleHeight-this.getPropertyValue('spacing')
                } else {
                    if(index === 0){
                        y = this.getPropertyValue('originY')
                    } else {
                        y = this.getPropertyValue('originY') + (this.middleHeight + this.getPropertyValue('spacing')) * index 
                    }   
                }
                height = this.middleHeight

                if(y + height < this.getPropertyValue('contentY') - this.getPropertyValue('cacheBuffer') || y > this.getPropertyValue('contentY') + this.getPropertyValue('height') + this.getPropertyValue('cacheBuffer')) {
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

    $createElement(index, info){ 
        if(this.$items[index]) return this.$items[index]

        let obj = undefined

        let ctx = new ContextController(this.getProperty('delegate').get().$exCtx, this.$exCtx)
        let createObject = this.getProperty('delegate').get().createObject
        let cls = this.getProperty('delegate').get().constructor
        
        if(typeof this.getPropertyValue('model') === 'number'){
            obj = createObject ? createObject(this.getProperty('contentItem').get(),ctx, {index: index}, false) : new cls(this.getProperty('contentItem').get(),ctx, {index: index})

            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            obj = createObject ? createObject(this.getProperty('contentItem').get(),ctx, model, false) : new cls(this.getProperty('contentItem').get(),ctx, model)

            this.$items[index] = obj   
        }

        for(let update of updateList.splice(0, updateList.length)){
            update()
        }

        if(obj.getPropertyValue('width') <= 0 || obj.getPropertyValue('height') <= 0) {
            obj.setStyle({
                visibility: 'hidden'
            })
        } else {
            obj.setStyle({
                visibility: 'visible'
            })
        }
    
        obj.getProperty('x').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                let index = this.$items.indexOf(obj)
                if(index >= 0 && this.$items[index+1]){
                    this.$items[index+1].getProperty('x').reset(this.$items[index].getPropertyValue('x')+this.$items[index].getPropertyValue('width'))
                }
            }
            this.$updateGeometry()
        })
        obj.getProperty('y').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                let index = this.$items.indexOf(obj)
                if(index >= 0 && this.$items[index+1]){
                    this.$items[index+1].getProperty('y').reset(this.$items[index].getPropertyValue('y')+this.$items[index].getPropertyValue('height'))
                }
            }
            this.$updateGeometry()
        })
        obj.getProperty('width').getNotify().connect(()=>{
            if(obj.getPropertyValue('width') <= 0 || obj.getPropertyValue('height') <= 0) {
                obj.setStyle({
                    visibility: 'hidden'
                })
            } else {
                obj.setStyle({
                    visibility: 'visible'
                })
            }
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                let index = this.$items.indexOf(obj)
                if(index >= 0 && this.$items[index+1]){
                    this.$items[index+1].getProperty('x').reset(obj.getPropertyValue('x')+obj.getPropertyValue('width')+this.getPropertyValue('spacing'))
                }
            }
            this.$updateGeometry()
        })
        obj.getProperty('height').getNotify().connect(()=>{
            if(obj.getPropertyValue('width') <= 0 || obj.getPropertyValue('height') <= 0) {
                obj.setStyle({
                    visibility: 'hidden'
                })
            } else {
                obj.setStyle({
                    visibility: 'visible'
                })
            }
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                let index = this.$items.indexOf(obj)
                if(index >= 0 && this.$items[index+1]){
                    this.$items[index+1].getProperty('y').reset(obj.getPropertyValue('y')+obj.getPropertyValue('height')+this.getPropertyValue('spacing'))
                }
            }
            this.$updateGeometry()
        })


        obj.getProperty('x').reset(info.x)
        obj.getProperty('y').reset(info.y)

   
        obj.$complete()
        
        return obj
    }


    destroy(){
        this.$disconnectModel()
        this.$clear()
        
        super.destroy()
    }
}

module.exports.ListView = ListView