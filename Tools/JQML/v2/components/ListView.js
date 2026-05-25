const { Item } = require('./Item')
const { Flickable } = require('./Flickable')
const { QVar, QReal, QBool, QAutoGeometry } = require('../utils/properties')
const { ListModel } = require('./ListModel')
const { PropertyAnimation } = require('./PropertyAnimation')
const { DelegateChooser } = require('./DelegateChooser')



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
        currentItem: { type: QVar, value: undefined },
        snapMode: { type: QReal, value: ListView.NoSnap, changed: '$snapModeChanged' },
        cacheBuffer: { type: QReal, value: 320, changed: '$cacheBufferChanged' },
        count: { type: QReal, value: 0 },
        contentWidth: { type: QAutoGeometry, value: 0, changed: '$contentWidthChanged' },
        contentHeight: { type: QAutoGeometry, value: 0, changed: '$contentHeightChanged' },
        reuseItems: { type: QBool, value: false },
        highlight: { type: QVar, changed: '$highlightChanged' },
        highlightFollowsCurrentItem: { type: QBool, value: true },
        highlightMoveDuration: { type: QReal, value: 150 },
        header: { type: QVar, changed: '$headerChanged' },
        footer: { type: QVar, changed: '$footerChanged' },
        headerItem: { type: QVar, value: null },
        footerItem: { type: QVar, value: null },
        keyNavigationEnabled: { type: QBool, value: true },
        displayMarginBeginning: { type: QReal, value: 0 },
        displayMarginEnd: { type: QReal, value: 0 },
        highlightRangeMode: { type: QReal, value: 0 },
        preferredHighlightBegin: { type: QReal, value: 0 },
        preferredHighlightEnd: { type: QReal, value: 0 },
        section: { type: QVar },
        add: { type: QVar },
        remove: { type: QVar },
        displaced: { type: QVar },
        addDisplaced: { type: QVar },
        removeDisplaced: { type: QVar },
        moveDisplaced: { type: QVar },
        populate: { type: QVar },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx

        // Qt ListView clips by default
        this.getProperty('clip').reset(true)

        this.middleWidth = 0
        this.middleHeight = 0

        this.$items = []

        this.$cache = []

        this.$animation = new PropertyAnimation()
        this.$animation.target = this
        // this.$animation.duration = 600

        // Keyboard navigation
        if (this.$dom){
            this.$dom.setAttribute('tabindex', '0')
            this.$dom.style.outline = 'none'
            this.$dom.addEventListener('keydown', (e) => {
                if (!this.getPropertyValue('keyNavigationEnabled')) return
                let orientation = this.getPropertyValue('orientation')
                let handled = false
                if (orientation === ListView.Vertical){
                    if (e.key === 'ArrowDown'){
                        this.incrementCurrentIndex()
                        handled = true
                    } else if (e.key === 'ArrowUp'){
                        this.decrementCurrentIndex()
                        handled = true
                    }
                } else {
                    if (e.key === 'ArrowRight'){
                        this.incrementCurrentIndex()
                        handled = true
                    } else if (e.key === 'ArrowLeft'){
                        this.decrementCurrentIndex()
                        handled = true
                    }
                }
                if (handled){
                    e.preventDefault()
                    e.stopPropagation()
                    // Ensure current item is visible
                    let idx = this.getPropertyValue('currentIndex')
                    if (idx >= 0) this.positionViewAtIndex(idx, ListView.Contain)
                }
            })
        }
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
        this.$anchorsUpdate()
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

    // Convert content coordinates to model index (-1 if no delegate at that position)
    indexAt(x, y){
        for (let i = 0; i < this.$items.length; i++){
            let item = this.$items[i]
            if (!item) continue
            if (this.getPropertyValue('orientation') === ListView.Horizontal){
                let ix = item.getPropertyValue('x')
                let iw = item.getPropertyValue('width')
                if (x >= ix && x < ix + iw) return i
            } else {
                let iy = item.getPropertyValue('y')
                let ih = item.getPropertyValue('height')
                if (y >= iy && y < iy + ih) return i
            }
        }
        return -1
    }

    // Force immediate layout update (Qt API)
    forceLayout(){
        this.$updateView()
        this.$updateGeometry()
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

    $isModelTransacting(){
        let model = this.getPropertyValue('model')
        return model && global.TransactionController && global.TransactionController.level > 0 && global.TransactionController.objects.has(model)
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
                        // Update index statements for shifted items
                        for(let i = bottomRight; i < this.$items.length; i++){
                            if(this.$items[i]){
                                let newIndex = i
                                this.$items[i].getStatement('index').setCompute(()=>{return newIndex})
                                this.$items[i].getStatement('index').update()
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

                        let firstRemoved = removed.find(item => item !== undefined)
                        if(this.$items[leftTop] && firstRemoved){
                            this.$items[leftTop].getProperty('x').reset(firstRemoved.getPropertyValue('x'))
                            this.$items[leftTop].getProperty('y').reset(firstRemoved.getPropertyValue('y'))
                        }
    
                        for(let item of removed){
                            this.$toCache(item)
                        }

                        // Update index statements for shifted items
                        for(let i = leftTop; i < this.$items.length; i++){
                            if(this.$items[i]){
                                let newIndex = i
                                this.$items[i].getStatement('index').setCompute(()=>{return newIndex})
                                this.$items[i].getStatement('index').update()
                            }
                        }
                    }
                }
                if(roles === 'move'){
                    let data = model.getPropertyValue('data')
                    for(let i = leftTop; i < bottomRight; i++){
                        let item = this.$items[i]
                        if(item && data[i]){
                            item.getStatement('model').reset(data[i])
                            item.getStatement('model_').reset(data[i])
                            let newIndex = i
                            item.getStatement('index').setCompute(()=>{return newIndex})
                            item.getStatement('index').update()
                        }
                    }
                }
                if(roles === 'update'){
                    let data = model.getPropertyValue('data')
                    for(let i = leftTop; i < bottomRight; i++){
                        let item = this.$items[i]
                        if(item && data[i]){
                            item.getStatement('model').reset(data[i])
                            item.getStatement('model_').reset(data[i])
                        }
                    }
                }
                if(roles === 'reset'){
                    for(let i = 0; i < this.$items.length; i++){
                        if(this.$items[i]) this.$toCache(this.$items[i])
                    }
                    this.$items = []
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
		this.$clear(true)

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

        let model = this.getPropertyValue('model')
        let length = 0
        if(model instanceof ListModel){
            length = model.getPropertyValue('count')
        } else if(Array.isArray(model)){
            length = model.length
        } else if(typeof model === 'number'){
            length = model
        }
        this.getProperty('count').value = length

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
        if(this.$inUpdateView) {
            this.$pendingUpdateView = true
            return
        }
        this.$inUpdateView = true
        this.$pendingUpdateView = false

        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) {
            this.$inUpdateView = false
            return
        }

        if(!this.$ready && !this.$properties.model.$isReset){
            this.$needUpdate = true
            this.$inUpdateView = false
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
            this.$inUpdateView = false
            return
        }

        if(length === 0) {
            this.$inUpdateView = false
            return
        }

        let firstIndex = -1
        let lastIndex = -1
        
        for(let i = 0; i < length; i++){
            if(this.$items[i]){
                if(firstIndex < 0) firstIndex = i
                lastIndex = i
            }
        }

        if(firstIndex < 0) firstIndex = 0
        if(lastIndex < 0) lastIndex = 0

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

            if(approximateMiddleIndex >= length) approximateMiddleIndex = length - 1
            if(approximateMiddleIndex < 0) approximateMiddleIndex = 0

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

        this.$inUpdateView = false
        if(this.$pendingUpdateView) {
            this.$pendingUpdateView = false
            this.$updateView()
        }
    }

    $updateGeometry(){
        if(!this.$items.length) {
            // Preserve middleHeight/middleWidth for subsequent $updateView calculations.
            // But reset contentHeight/contentWidth and origin to 0 for empty model.
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                let contentWidthProperty = this.getProperty('contentWidth')
                if(contentWidthProperty.value != 0){
                    contentWidthProperty.value = 0
                    contentWidthProperty.getNotify()()
                }
                this.getStatement('originX').reset(0)
            } else {
                let contentHeightProperty = this.getProperty('contentHeight')
                if(contentHeightProperty.value != 0){
                    contentHeightProperty.value = 0
                    contentHeightProperty.getNotify()()
                }
                this.getStatement('originY').reset(0)
            }
            return
        }

        let model = this.getPropertyValue('model')
        let length = 0
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
            let contentWidth = visibleContentWidth + Math.round(middleWidth)*(length-visibleCount) + this.getPropertyValue('spacing') * (length-1)

            let contentWidthProperty = this.getProperty('contentWidth')
            if(contentWidthProperty.value != contentWidth){
                contentWidthProperty.value = contentWidth
                contentWidthProperty.getNotify()()
            }

            let originX = (minX - firstIndex*(Math.round(middleWidth+this.getPropertyValue('spacing'))))
            if(originX !== Infinity && originX !== -Infinity) this.getStatement('originX').reset(originX)

            // this.getProperty('contentHeight').reset(maxHeight)
        } else {
            let contentHeight = visibleContentHeight + Math.round(middleHeight)*(length-visibleCount) + this.getPropertyValue('spacing') * (length-1)

            let contentHeightProperty = this.getProperty('contentHeight')
            if(contentHeightProperty.value != contentHeight){
                contentHeightProperty.value = contentHeight
                contentHeightProperty.getNotify()()
            }

            let originY = (minY - firstIndex*(Math.round(middleHeight+this.getPropertyValue('spacing'))))
            if(originY !== Infinity && originY !== -Infinity) this.getStatement('originY').reset(originY)

            // this.getProperty('contentWidth').reset(maxWidth)
        }  
    }

    $orientationChanged(){
        if(this.getPropertyValue('orientation') === ListView.Horizontal){
            this.getProperty('originY').reset(0)

            let contentHeightProperty = this.getProperty('contentHeight')
            if(contentHeightProperty.value != -1){
                contentHeightProperty.value = -1
                contentHeightProperty.getNotify()()
            }
        } else {
            this.getProperty('originX').reset(0)

            let contentWidthProperty = this.getProperty('contentWidth')
            if(contentWidthProperty.value != -1){
                contentWidthProperty.value = -1
                contentWidthProperty.getNotify()()
            }
        }

        let temp = this.$items
        this.$items = []

        for(let i = 0; i < temp.length; i++){
            let info = this.$getItemInfo(i)

            if(info.inner){
                if(this.getPropertyValue('orientation') === ListView.Horizontal){
    
                } else {

                }

                temp[i].getProperty('x').reset(info.x)
                temp[i].getProperty('y').reset(info.y)
            }
            
        }

        this.$items = temp

        this.$updateView()
        this.$updateGeometry()
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

        let contentWidth = this.getProperty('contentWidth')
        if(contentWidth && contentWidth.subscribers && contentWidth.subscribers.indexOf(this.getProperty('width')) >= 0){
            this.getProperty('width').updating = false
            this.getProperty('width').update()
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

        let contentHeight = this.getProperty('contentHeight')
        if(contentHeight && contentHeight.subscribers && contentHeight.subscribers.indexOf(this.getProperty('height')) >= 0){
            this.getProperty('height').updating = false
            this.getProperty('height').update()
        }
    }
    $widthChanged(){
        super.$widthChanged()

        if(this.$isModelTransacting()) return
        this.$updateView()
    }
    $heightChanged(){
        super.$heightChanged()

        if(this.$isModelTransacting()) return
        this.$updateView()
    }
    $cacheBufferChanged(){
        this.$updateView()
    }

    $currentIndexChanged(){
        let idx = this.getPropertyValue('currentIndex')
        let item = (idx >= 0 && idx < this.$items.length) ? this.$items[idx] : undefined
        this.getProperty('currentItem').reset(item || undefined)
        // Update ListView.isCurrentItem attached property on delegates
        this.$updateIsCurrentItem()
        // Move highlight to follow current item
        if (this.getPropertyValue('highlightFollowsCurrentItem')){
            this.$updateHighlightPosition()
        }
    }

    $updateIsCurrentItem(){
        let curIdx = this.getPropertyValue('currentIndex')
        for (let i = 0; i < this.$items.length; i++){
            let item = this.$items[i]
            if (item && item.getStatement){
                let st = item.getStatement('ListView.isCurrentItem')
                if (st) st.reset(i === curIdx)
            }
        }
    }

    $highlightChanged(){
        // Recreate highlight item when delegate changes
        if (this.$highlightItem){
            this.$highlightItem.destroy()
            this.$highlightItem = null
        }
        this.$createHighlightItem()
    }

    $createHighlightItem(){
        let highlightDelegate = this.getPropertyValue('highlight')
        if (!highlightDelegate) return
        let ctx = new ContextController(highlightDelegate.$exCtx, this.$exCtx)
        let createObject = highlightDelegate.createObject
        let cls = highlightDelegate.constructor
        let contentItem = this.getProperty('contentItem').get()
        this.$highlightItem = createObject ? createObject(contentItem, ctx, {}, false) : new cls(contentItem, ctx, {})
        this.$highlightItem.$complete()
        // Position behind delegates (z-order)
        if (this.$highlightItem.$dom && contentItem.$dom){
            contentItem.$dom.insertBefore(this.$highlightItem.$dom, contentItem.$dom.firstChild)
        }
        this.$updateHighlightPosition()
    }

    $updateHighlightPosition(){
        if (!this.$highlightItem) return
        let idx = this.getPropertyValue('currentIndex')
        let item = (idx >= 0 && idx < this.$items.length) ? this.$items[idx] : null
        if (!item){
            this.$highlightItem.getProperty('visible').reset(false)
            return
        }
        this.$highlightItem.getProperty('visible').reset(true)
        this.$highlightItem.getProperty('x').reset(item.getPropertyValue('x'))
        this.$highlightItem.getProperty('y').reset(item.getPropertyValue('y'))
        this.$highlightItem.getProperty('width').reset(item.getPropertyValue('width'))
        this.$highlightItem.getProperty('height').reset(item.getPropertyValue('height'))
    }

    $headerChanged(){
        if (this.$headerInstance){
            this.$headerInstance.destroy()
            this.$headerInstance = null
        }
        let headerDelegate = this.getPropertyValue('header')
        if (!headerDelegate) return
        let ctx = new ContextController(headerDelegate.$exCtx, this.$exCtx)
        let createObject = headerDelegate.createObject
        let cls = headerDelegate.constructor
        let contentItem = this.getProperty('contentItem').get()
        this.$headerInstance = createObject ? createObject(contentItem, ctx, {}, false) : new cls(contentItem, ctx, {})
        this.$headerInstance.$complete()
        this.getProperty('headerItem').reset(this.$headerInstance)
        // Header is placed before list content — shift content down/right
        this.$updateView()
    }

    $footerChanged(){
        if (this.$footerInstance){
            this.$footerInstance.destroy()
            this.$footerInstance = null
        }
        let footerDelegate = this.getPropertyValue('footer')
        if (!footerDelegate) return
        let ctx = new ContextController(footerDelegate.$exCtx, this.$exCtx)
        let createObject = footerDelegate.createObject
        let cls = footerDelegate.constructor
        let contentItem = this.getProperty('contentItem').get()
        this.$footerInstance = createObject ? createObject(contentItem, ctx, {}, false) : new cls(contentItem, ctx, {})
        this.$footerInstance.$complete()
        this.getProperty('footerItem').reset(this.$footerInstance)
        this.$updateView()
    }

    $snapModeChanged(){
        // Snap mode change doesn't need immediate action; handled in $moveEnd
    }

    incrementCurrentIndex(){
        let count = this.getPropertyValue('count')
        let idx = this.getPropertyValue('currentIndex')
        if (idx < count - 1){
            this.getProperty('currentIndex').reset(idx + 1)
        }
    }

    decrementCurrentIndex(){
        let idx = this.getPropertyValue('currentIndex')
        if (idx > 0){
            this.getProperty('currentIndex').reset(idx - 1)
        }
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

        // If this was the currentItem, invalidate it
        if (this.getPropertyValue('currentItem') === item){
            this.getProperty('currentItem').reset(undefined)
        }

        if(this.getPropertyValue('reuseItems')){
            if(item instanceof Item) {
                this.getPropertyValue('contentItem').$dom.removeChild(item.$dom)
            }

            this.$cache.push(item)

            // Clear attached property — delegate no longer represents any index
            if (item.getStatement){
                let st = item.getStatement('ListView.isCurrentItem')
                if (st) st.reset(false)
            }

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
                    if(obj.getStatement('modelData')) obj.getStatement('modelData').reset(exModel['$modelData'])
                } else {
                    let keys = Object.keys(exModel)
                    if(keys.length === 1){
                        obj.getStatement('modelData_').reset(exModel[keys[0]])
                        if(obj.getStatement('modelData')) obj.getStatement('modelData').reset(exModel[keys[0]])
                    }
                }

                obj.getStatement('model').reset(exModel)
                obj.getStatement('model_').reset(exModel)
                obj.getStatement('index').setCompute(()=>{return exModel.index})
                obj.getStatement('index').update()
            }

            // Update ListView attached properties on reuse
            if (obj.getStatement){
                let curIdx = this.getPropertyValue('currentIndex')
                let st = obj.getStatement('ListView.isCurrentItem')
                if (st) st.reset(index === curIdx)
                let vst = obj.getStatement('ListView.view')
                if (vst) vst.reset(this)
            }

            if(obj.$signals['ListView.reused']) obj.$signals['ListView.reused']()

            if(obj instanceof Item) {
                if(obj.getPropertyValue('width') <= 0 || obj.getPropertyValue('height') <= 0) {
                    obj.setStyle({ visibility: 'hidden' })
                } else {
                    obj.setStyle({ visibility: 'visible' })
                }
            }
        } else {
            let delegateValue = this.getProperty('delegate').get()

            // Resolve DelegateChooser: pick per-item delegate based on model data
            if(delegateValue instanceof DelegateChooser){
                let exModel = null
                if(Array.isArray(this.getPropertyValue('model'))){
                    let val = this.getPropertyValue('model')[index]
                    exModel = (typeof val === 'object' && val !== null) ? val : { '$modelData': val, index: index }
                } else if(typeof this.getPropertyValue('model') === 'number'){
                    exModel = { index: index }
                } else {
                    exModel = this.getPropertyValue('model').getPropertyValue('data')[index]
                }
                delegateValue = delegateValue.$chooseDelegate(index, -1, exModel)
                if(!delegateValue) return false
            }

            let ctx = new ContextController(delegateValue.$exCtx, this.$exCtx)
            let createObject = delegateValue.createObject
            let cls = delegateValue.constructor
            
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

        // Set ListView attached properties on delegate
        if (obj.getStatement){
            let curIdx = this.getPropertyValue('currentIndex')
            let itemIndex = this.$items.indexOf(obj)
            if (obj.getStatement('ListView.isCurrentItem')){
                obj.getStatement('ListView.isCurrentItem').reset(itemIndex === curIdx)
            }
            if (obj.getStatement('ListView.view')){
                obj.getStatement('ListView.view').reset(this)
            }
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
        if (this.$highlightItem){ this.$highlightItem.destroy(); this.$highlightItem = null }
        if (this.$headerInstance){ this.$headerInstance.destroy(); this.$headerInstance = null }
        if (this.$footerInstance){ this.$footerInstance.destroy(); this.$footerInstance = null }
        
        super.destroy()
    }
}

module.exports.ListView = ListView
