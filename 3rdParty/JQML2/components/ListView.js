const { Item } = require('./Item')
const { Flickable } = require('./Flickable')
const { QVar, QReal, QBool, QAutoGeometry } = require('../utils/properties')
const { ListModel } = require('./ListModel')
const { PropertyAnimation } = require('./PropertyAnimation')



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
        reuseItems: { type: QBool, value: false },
    }

    // static defaultSignals = {
    //     pooled: { params: [] },
	// 	reused: { params: [] },
    // }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx

        this.middleWidth = 0
        this.middleHeight = 0

        this.$items = []

        this.$cache = []

        this.$animation = new PropertyAnimation()
        this.$animation.target = this
        // this.$animation.duration = 600
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
        let length = 0 
        let model = this.getPropertyValue('model')
        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(typeof model === 'number'){
            length = model
        } else {
            return undefined
        }

        return index >= 0 && index < length ? this.$items[index] : undefined
    }
    positionViewAtBeginning(){
        this.positionViewAtIndex(0, ListView.Beginning)
    }
    positionViewAtEnd(){
        let length = 0 
        let model = this.getPropertyValue('model')
        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(typeof model === 'object' && Array.isArray(model)){
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        } else {
            return
        }

        this.positionViewAtIndex(length-1, ListView.Beginning)
    }
    positionViewAtIndex(index, mode = ListView.Beginning){
        let length = 0 
        let model = this.getPropertyValue('model')
        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(typeof model === 'object' && Array.isArray(model)){
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        } else {
            return
        }

        if(index < 0 || index >= length) return
        if(this.getPropertyValue('orientation') === ListView.Horizontal){
            if(this.getPropertyValue('contentWidth') <= this.getPropertyValue('width')) return
        } else {
            if(this.getPropertyValue('contentHeight') <= this.getPropertyValue('height')) return
        }

        let itemInfo = null

        while(!itemInfo || !itemInfo.exist){
            itemInfo = this.$getItemInfo(index)
            let x = 0
            let y = 0

            switch(mode){
                case ListView.Beginning: {
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        x = itemInfo.x
                    } else {
                        y = itemInfo.y
                    }

                    break
                }
                case ListView.Center: {
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        x = itemInfo.x - this.getPropertyValue('width') / 2 + itemInfo.width / 2
                    } else {
                        y = itemInfo.y - this.getPropertyValue('height') / 2 + itemInfo.height / 2
                    }

                    break
                }
                case ListView.End: {
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        x = itemInfo.x - this.getPropertyValue('width') + itemInfo.width
                    } else {
                        y = itemInfo.y - this.getPropertyValue('height') + itemInfo.height
                    }

                    break
                }
                case ListView.Visible: {
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        if(itemInfo.x + itemInfo.width < this.getPropertyValue('contentX') || itemInfo.x > this.getPropertyValue('contentX') + this.getPropertyValue('width')){
                            if(Math.abs(itemInfo.x - this.getPropertyValue('contentX')) < Math.abs((itemInfo.x - this.getPropertyValue('width') + itemInfo.width) - this.getPropertyValue('contentX'))){
                                x = itemInfo.x
                            } else {
                                x = itemInfo.x - this.getPropertyValue('width') + itemInfo.width
                            }
                        } else {
                            x = this.getPropertyValue('contentX')
                        }
                    } else {
                        if(itemInfo.y + itemInfo.height < this.getPropertyValue('contentY') || itemInfo.y > this.getPropertyValue('contentY') + this.getPropertyValue('height')){
                            if(Math.abs(itemInfo.y - this.getPropertyValue('contentY')) < Math.abs((itemInfo.y - this.getPropertyValue('height') + itemInfo.height) - this.getPropertyValue('contentY'))){
                                y = itemInfo.y
                            } else {
                                y = itemInfo.y - this.getPropertyValue('height') + itemInfo.height
                            }
                        } else {
                            y = this.getPropertyValue('contentY')
                        }
                    }

                    break
                }
                case ListView.Contain: {
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        if(itemInfo.width >= this.getPropertyValue('width')){
                            x = itemInfo.x
                        } else if(itemInfo.x < this.getPropertyValue('contentX') || itemInfo.x + itemInfo.width > this.getPropertyValue('contentX') + this.getPropertyValue('width')){
                            if(Math.abs(itemInfo.x - this.getPropertyValue('contentX')) < Math.abs((itemInfo.x - this.getPropertyValue('width') + itemInfo.width) - this.getPropertyValue('contentX'))){
                                x = itemInfo.x
                            } else {
                                x = itemInfo.x - this.getPropertyValue('width') + itemInfo.width
                            }
                        } else {
                            x = this.getPropertyValue('contentX')
                        }
                    } else {
                        if(itemInfo.height >= this.getPropertyValue('height')){
                            y = itemInfo.y
                        } else if(itemInfo.y < this.getPropertyValue('contentY') || itemInfo.y + itemInfo.height > this.getPropertyValue('contentY') + this.getPropertyValue('height')){
                            if(Math.abs(itemInfo.y - this.getPropertyValue('contentY')) < Math.abs((itemInfo.y - this.getPropertyValue('height') + itemInfo.height) - this.getPropertyValue('contentY'))){
                                y = itemInfo.y
                            } else {
                                y = itemInfo.y - this.getPropertyValue('height') + itemInfo.height
                            }
                        } else {
                            y = this.getPropertyValue('contentY')
                        }
                    }

                    break
                }
            }

            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(x < this.getPropertyValue('originX')){
                    this.getProperty('contentX').reset(this.getPropertyValue('originX'))
                } else if(x > this.getPropertyValue('originX') + this.getPropertyValue('contentWidth') - this.getPropertyValue('width')){
                    this.getProperty('contentX').reset(this.getPropertyValue('originX') + this.getPropertyValue('contentWidth') - this.getPropertyValue('width'))
                } else {
                    this.getProperty('contentX').reset(x)
                }
            } else {
                if(y < this.getPropertyValue('originY')){
                    this.getProperty('contentY').reset(this.getPropertyValue('originY'))
                } else if(y > this.getPropertyValue('originY') + this.getPropertyValue('contentHeight') - this.getPropertyValue('height')){
                    this.getProperty('contentY').reset(this.getPropertyValue('originY') + this.getPropertyValue('contentHeight') - this.getPropertyValue('height'))
                } else {
                    this.getProperty('contentY').reset(y)
                }
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
        if(changeset && sender === this.getPropertyValue('model')){
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
            // this.getProperty('count').reset(length)

            for(let change of changeset){
                let leftTop = change[0]
                let bottomRight = change[1]
                let roles = change[2]

                if(countChanged){
                    if(roles === 'append'){

                    } else if(roles === 'insert'){
                        for(let i = leftTop; i < bottomRight; i++){
                            this.$items.splice(i, 0, undefined)
                            let info = this.$getItemInfo(i)
                            if(info.inner && !info.exist){
                                this.$createElement(i, info)
                            }
                        }
                        for(let i = bottomRight; i < this.$items.length; i++){
                            let info = this.$getItemInfo(i)
                            if(!info.inner && info.exist){
                                let removed = this.$items.splice(i, this.$items.length - i)
                                for(let item of removed){
                                    if(item) {
                                        this.$toCache(item)
                                    }
                                } 
                                break
                            }
                        }
                    } else if(roles === 'remove'){
                        let removed = this.$items.splice(leftTop, bottomRight - leftTop)
    
                        if(this.$items[leftTop] && removed.length){
                            this.$items[leftTop].getProperty('x').reset(removed[0].getPropertyValue('x'))
                            this.$items[leftTop].getProperty('y').reset(removed[0].getPropertyValue('y'))
                        }
    
                        for(let item of removed){
                            this.$toCache(item)
                        } 
                    }
                }
                if(roles === 'move'){
                    for(let i = leftTop; i < bottomRight; i++){
                        let from = i
                        let to = this.$items[i].index
        
                        let fromItem = this.$items[from]
                        let toItem = this.$items[to]
        
                        let toModel = this.getPropertyValue('model').getPropertyValue('data')[from]
                        let fromModel = this.getPropertyValue('model').getPropertyValue('data')[to]
        
                        fromItem.model_ = toModel
                        if(fromItem.model instanceof QModelData) fromItem.model = toModel
                        toItem.model_ = fromModel
                        if(toItem.model instanceof QModelData) toItem.model = fromModel
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

    $clear(destroyCache = false){
        if(destroyCache)
        while(this.$cache.length){
            let obj = this.$cache.shift()
            if(obj) obj.destroy()
        }

        while(this.$items.length){
            if(destroyCache){
                let obj = this.$items.shift()
                if(obj) obj.destroy()
            } else {
                let obj = this.$items.shift()
                this.$toCache(obj)
            }
            
        }

        this.getProperty('originX').value = 0
        this.getProperty('originY').value = 0
        // this.getProperty('contentX').value = 0
        // this.getProperty('contentY').value = 0

        // this.getPropertyValue('contentItem').getProperty('x').reset(-this.getPropertyValue('contentX'))
        // this.getPropertyValue('contentItem').getProperty('y').reset(-this.getPropertyValue('contentY'))

        this.getProperty('count').value = 0
    }

    $modelChanged(){
        this.$disconnectModel()
        this.$clear()

        let model = this.getPropertyValue('model')
        let length = 0 

        if(model instanceof ListModel){     
            this.$connectModel(model)
            length = model.getPropertyValue('count')
        } else if(Array.isArray(model)){
            this.getProperty('model').value = model.slice()
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        }

        let countChanged = this.getPropertyValue('count') !== length
        this.getProperty('count').value = length
        // this.getProperty('count').reset(length)

        this.$updateView()

        if(countChanged && this.getProperty('count').notify){
            this.getProperty('count').notify()
        }
    }

    $delegateChanged(){
        this.$clear(true)
        this.$updateView()
    }

    $spacingChanged(){
        for(let i = 0; i < this.$items.length - 1; i++){
            if(this.$items[i] && this.$items[i + 1]){
                if(this.getPropertyValue('orientation') === ListView.Horizontal){
                    this.$items[i + 1].getProperty('x').reset(this.$items[i].getPropertyValue('x')+this.$items[i].getPropertyValue('width')+this.getPropertyValue('spacing'))
                } else {
                    this.$items[i + 1].getProperty('y').reset(this.$items[i].getPropertyValue('y')+this.$items[i].getPropertyValue('height')+this.getPropertyValue('spacing'))
                }
                break
            }
        }
        this.$updateView()
    }

    $updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return

        if(!this.$ready && !this.$properties.model.$isReset){
            this.$needUpdate = true
            return
        }
        
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

        let firstIndex = 0
        let lastIndex = 0
        
        for(let i = 0; i < length; i++){
            if(this.$items[i] && !this.$items[i-1] && !firstIndex) firstIndex = i
            if(this.$items[i] && !this.$items[i+1] && !lastIndex) lastIndex = i
        }

        let _firstIndex = -1
        let _lastIndex = -1
        
        for(let i = firstIndex; i <= lastIndex; i++){
            let info = this.$getItemInfo(i)

            if(info.inner){
                if(_firstIndex < 0) {
                    _firstIndex = i
                    _lastIndex = i
                } else {
                    _lastIndex = i
                }
            } else if(info.exist){
                this.$toCache(this.$items[i])
                this.$items[i] = undefined
            }
        }

        if(_firstIndex >= 0){
            for(let i = _firstIndex; i >= 0; i--){
                let info = this.$getItemInfo(i)
                if(info.inner){
                    if(!info.exist){
                        if(this.$createElement(i, info)) this.$updateGeometry()
                    }
                } else {
                    break
                }
            }
        }
        
        if(_lastIndex >= 0){
            for(let i = _lastIndex; i < length; i++){
                let info = this.$getItemInfo(i)
                if(info.inner){
                    if(!info.exist){
                        if(this.$createElement(i, info)) this.$updateGeometry()
                    }
                } else {
                    break
                }
            }
        }
        
        if(_firstIndex < 0 && _lastIndex < 0){
            let approximateMiddleIndex = -1

            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if((this.middleWidth + this.getPropertyValue('spacing')) > 0){
                    approximateMiddleIndex = Math.trunc((this.getPropertyValue('contentX') + this.getPropertyValue('width') / 2 - this.getPropertyValue('originX')) / (this.middleWidth + this.getPropertyValue('spacing')))
                } else {
                    approximateMiddleIndex = 0
                }
            } else {
                if((this.middleHeight + this.getPropertyValue('spacing')) > 0){
                    approximateMiddleIndex = Math.trunc((this.getPropertyValue('contentY') + this.getPropertyValue('height') / 2 - this.getPropertyValue('originY')) / (this.middleHeight + this.getPropertyValue('spacing')))
                } else {
                    approximateMiddleIndex = 0
                }
            }

            for(let i = approximateMiddleIndex; i >= 0; i--){
                let info = this.$getItemInfo(i)
                if(info.inner){
                    if(!info.exist){
                        if(this.$createElement(i, info)) this.$updateGeometry()
                    }
                } else {
                    break
                }
            }

            for(let i = approximateMiddleIndex + 1; i < length; i++){
                let info = this.$getItemInfo(i)
                if(info.inner){
                    if(!info.exist){
                        if(this.$createElement(i, info)) this.$updateGeometry()
                    }
                } else {
                    break
                }
            }

        }
    }

    $updateGeometry(){
        if(!this.$items.length) {
            return
        }

        let model = this.getPropertyValue('model')
        if(model instanceof ListModel){     
            length = model.getPropertyValue('count')
        } else if(Array.isArray(model)){
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        } else {
            return
        }
        
        let lastIndex = 0
        let firstIndex = this.$items.length-1
        let minX = Infinity
        let minY = Infinity

        let visibleCount = 0
        let visibleContentWidth = 0
        let visibleContentHeight = 0

        let maxWidth = this.getPropertyValue('width')
        let maxHeight = this.getPropertyValue('height')

        for(let i = 0; i < this.$items.length; i++){
            if(this.$items[i]){
                visibleCount++
                visibleContentWidth += this.$items[i].getPropertyValue('width')
                visibleContentHeight += this.$items[i].getPropertyValue('height')

                maxWidth = Math.max(maxWidth, this.$items[i].getPropertyValue('width'))
                maxHeight = Math.max(maxHeight, this.$items[i].getPropertyValue('height'))

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
            this.getStatement('contentWidth').reset(visibleContentWidth + Math.round(middleWidth)*(length-visibleCount) + this.getPropertyValue('spacing') * (length-1))
            let originX = (minX - firstIndex*(Math.round(middleWidth+this.getPropertyValue('spacing'))))
            if(originX !== Infinity && originX !== -Infinity) this.getStatement('originX').reset(originX)

            this.getProperty('contentHeight').reset(maxHeight)
        } else {
            this.getStatement('contentHeight').reset(visibleContentHeight + Math.round(middleHeight)*(length-visibleCount) + this.getPropertyValue('spacing') * (length-1))
            let originY = (minY - firstIndex*(Math.round(middleHeight+this.getPropertyValue('spacing'))))
            if(originY !== Infinity && originY !== -Infinity) this.getStatement('originY').reset(originY)

            this.getProperty('contentWidth').reset(maxWidth)
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
        if(this.UID) this.getPropertyValue('contentItem').getProperty('x').reset(-this.getPropertyValue('contentX'))
        this.$updateView()
    }

    $contentYChanged(){
        if(this.UID) this.getPropertyValue('contentItem').getProperty('y').reset(-this.getPropertyValue('contentY'))
        this.$updateView()
    }

    $contentWidthChanged(){
        if(this.UID) this.getPropertyValue('contentItem').getProperty('width').reset(this.getPropertyValue('contentWidth'))
        if(this.getPropertyValue('contentWidth') < this.getPropertyValue('width')){
            if(this.$items[0]){
                this.getProperty('contentX').reset(this.$items[0].getPropertyValue('x'))
            } else {
                this.getProperty('contentX').reset(this.getPropertyValue('originX'))
            }
        }
    }

    $contentHeightChanged(){
        if(this.UID) this.getPropertyValue('contentItem').getProperty('height').reset(this.getPropertyValue('contentHeight'))
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

        this.$updateView()
    }
    $heightChanged(){
        super.$heightChanged()

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

    $toCache(item){
        if(!item) return

        if(this.getPropertyValue('reuseItems')){
            if(item instanceof Item) {
                this.getPropertyValue('contentItem').$dom.removeChild(item.$dom)
            }

            this.$cache.push(item)

            if(item.$signals['ListView.pooled']) item.$signals['ListView.pooled']()
        } else {
            item.destroy()
        }
    }

    $fromCache(){
        let item = this.$cache.pop()
        if(item instanceof Item) {
            this.getPropertyValue('contentItem').$dom.appendChild(item.$dom)
        }

        return item
    }

    $createElement(index, info){ 
        if(this.$items[index]) return this.$items[index]

        let obj = undefined

        if(this.$cache.length){
            obj = this.$fromCache()

            this.$items[index] = obj

            let exModel = null

            if(Array.isArray(this.getPropertyValue('model'))){
                exModel = {'$modelData': this.getPropertyValue('model')[index], index: index}
            } else if(typeof this.getPropertyValue('model') === 'number'){
                exModel = {index: index}
            } else {
                let model = this.getPropertyValue('model').getPropertyValue('data')[index]
                exModel =  model   
            }

            if(this.$items[index - 1]){
                if(this.getPropertyValue('orientation') === ListView.Horizontal){
                    obj.getProperty('x').reset(this.$items[index - 1].getPropertyValue('x') + this.$items[index - 1].getPropertyValue('width') + this.getPropertyValue('spacing'))
                    obj.getProperty('y').reset(info.y)
                } else {
                    obj.getProperty('x').reset(info.x)
                    obj.getProperty('y').reset(this.$items[index - 1].getPropertyValue('y') + this.$items[index - 1].getPropertyValue('height') + this.getPropertyValue('spacing'))
                }
            } else if(this.$items[index + 1]){
                if(this.getPropertyValue('orientation') === ListView.Horizontal){
                    obj.getProperty('x').reset(this.$items[index + 1].getPropertyValue('x') - obj.getPropertyValue('width') - this.getPropertyValue('spacing'))
                    obj.getProperty('y').reset(info.y)
                } else {
                    obj.getProperty('x').reset(info.x)
                    obj.getProperty('y').reset(this.$items[index + 1].getPropertyValue('y') - obj.getPropertyValue('height') - this.getPropertyValue('spacing'))
                }
            } else {
                obj.getProperty('x').reset(info.x)
                obj.getProperty('y').reset(info.y)
            }
            
            
            if(exModel){
                if('$modelData' in exModel){
                    obj.getStatement('modelData_').reset(exModel['$modelData'])
                } else {
                    let keys = Object.keys(exModel)
                    if(keys.length === 1){
                        obj.getStatement('modelData_').reset(exModel[keys[0]])
                    }
                }

                obj.getStatement('model').reset(exModel)
                obj.getStatement('model_').reset(exModel)
                obj.getStatement('index').setCompute(()=>{return exModel.index})
                obj.getStatement('index').update()
            }

            if(obj.$signals['ListView.reused']) obj.$signals['ListView.reused']()
        } else {
            let ctx = new ContextController(this.getProperty('delegate').get().$exCtx, this.$exCtx)
            let createObject = this.getProperty('delegate').get().createObject
            let cls = this.getProperty('delegate').get().constructor
            
            if(Array.isArray(this.getPropertyValue('model'))){
                obj = createObject ? createObject(this.getProperty('contentItem').get(),ctx, {'$modelData': this.getPropertyValue('model')[index], index: index}, false) : new cls(this.getProperty('contentItem').get(),ctx, {'$modelData': this.getPropertyValue('model')[index], index: index})

                this.$items[index] = obj
            } else if(typeof this.getPropertyValue('model') === 'number'){
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
                        this.$items[index+1].getProperty('x').reset(this.$items[index].getPropertyValue('x')+this.$items[index].getPropertyValue('width')+this.getPropertyValue('spacing'))
                    }
                }
                this.$updateGeometry()
            })
            obj.getProperty('y').getNotify().connect(()=>{
                if(this.getPropertyValue('orientation') === ListView.Vertical){
                    let index = this.$items.indexOf(obj)
                    if(index >= 0 && this.$items[index+1]){
                        this.$items[index+1].getProperty('y').reset(this.$items[index].getPropertyValue('y')+this.$items[index].getPropertyValue('height')+this.getPropertyValue('spacing'))
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
        }
   
        obj.$complete()
        
        return obj
    }

    $moveStart(){
        this.$mouseX = this.contentX
        this.$mouseY = this.contentY
    }

    $moveEnd(){
        if(this.getPropertyValue('snapMode') === ListView.SnapToItem) {
            let targetIndex = 0

            for(let i = 0; i < this.$items.length; i++){
                if(this.$items[i]){
                    if(this.getPropertyValue('orientation') === ListView.Vertical){
                        if(this.$items[i].getPropertyValue('y') <= this.getPropertyValue('contentY') && this.$items[i].getPropertyValue('y') + this.$items[i].getPropertyValue('height') > this.getPropertyValue('contentY')){
                            targetIndex = i
                            break
                        }
                    } else {
                        if(this.$items[i].getPropertyValue('x') <= this.getPropertyValue('contentX') && this.$items[i].getPropertyValue('x') + this.$items[i].getPropertyValue('width') > this.getPropertyValue('contentX')){
                            targetIndex = i
                            break
                        }
                    }
                }
                
            }

            if(this.getPropertyValue('orientation') === ListView.Vertical){
                this.$animation.property = 'contentY'
                this.$animation.from = this.getPropertyValue('contentY')
                this.$animation.to = this.$items[targetIndex].getPropertyValue('y')
                this.$animation.start()
            } else {
                this.$animation.property = 'contentX'
                this.$animation.from = this.getPropertyValue('contentX')
                this.$animation.to = this.$items[targetIndex].getPropertyValue('x')
                this.$animation.start()
            }
        } else if(this.getPropertyValue('snapMode') === ListView.SnapOneItem){
            let currentIndex = 0

            for(let i = 0; i < this.$items.length; i++){
                if(this.$items[i]){
                    if(this.getPropertyValue('orientation') === ListView.Vertical){
                        if(this.$items[i].getPropertyValue('y') <= this.$mouseY && this.$items[i].getPropertyValue('y') + this.$items[i].getPropertyValue('height') > this.$mouseY){
                            currentIndex = i
                            break
                        }
                    } else {
                        if(this.$items[i].getPropertyValue('x') <= this.$mouseX && this.$items[i].getPropertyValue('x') + this.$items[i].getPropertyValue('width') > this.$mouseX){
                            currentIndex = i
                            break
                        }
                    }
                }
                
            }
            
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                this.$animation.property = 'contentY'
                this.$animation.from = this.getPropertyValue('contentY')
                
                if(this.contentY > this.$mouseY){
                    this.$animation.to = this.$items[currentIndex + 1].getPropertyValue('y')
                    this.$animation.start()
                } else if(this.contentY < this.$mouseY){
                    this.$animation.to = this.$items[currentIndex - 1].getPropertyValue('y')
                    this.$animation.start()
                }
            } else {
                this.$animation.property = 'contentX'
                this.$animation.from = this.getPropertyValue('contentX')

                if(this.contentX > this.$mouseX){
                    this.$animation.to = this.$items[currentIndex + 1].getPropertyValue('x')
                    this.$animation.start()
                } else if(this.contentX < this.$mouseX){
                    this.$animation.to = this.$items[currentIndex - 1].getPropertyValue('x')
                    this.$animation.start()
                }
            }
        }
    }

    destroy(){
        this.$disconnectModel()
        this.$clear(true)
        
        super.destroy()
    }
}

module.exports.ListView = ListView