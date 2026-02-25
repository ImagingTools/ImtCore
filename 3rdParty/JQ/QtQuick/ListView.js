const Flickable = require("./Flickable")
const Component = require("../QtQml/Component")
const Variant = require("../QtQml/Variant")
const Geometry = require("../QtQml/Geometry")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
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
        model: { type: Var, value: undefined},
        delegate: { type: Variant, typeTarget: Component, value: undefined},
        contentX: { type: Real, value: 0},
        contentY: { type: Real, value: 0},
        orientation: { type: Real, value: ListView.Vertical},
        spacing: { type: Real, value: 0},
        currentIndex: { type: Real, value: -1},
        currentItem: { type: Var, value: undefined},
        snapMode: { type: Real, value: ListView.NoSnap},
        cacheBuffer: { type: Real, value: 320},
        count: { type: Real, value: 0},
        contentWidth: { type: Real, value: 0},
        contentHeight: { type: Real, value: 0},
        reuseItems: { type: Bool, value: false },

        modelChanged: { type: Signal, args: [] },
        delegateChanged: { type: Signal, args: [] },
        contentXChanged: { type: Signal, args: [] },
        contentYChanged: { type: Signal, args: [] },
        orientationChanged: { type: Signal, args: [] },
        spacingChanged: { type: Signal, args: [] },
        currentIndexChanged: { type: Signal, args: [] },
        currentItemChanged: { type: Signal, args: [] },
        snapModeChanged: { type: Signal, args: [] },
        cacheBufferChanged: { type: Signal, args: [] },
        countChanged: { type: Signal, args: [] },
        contentWidthChanged: { type: Signal, args: [] },
        contentHeightChanged: { type: Signal, args: [] },
    })

    __items = []
    __cache = []

    __middleWidth = 0
    __middleHeight = 0

    __updatePrimaryProperties(){
        super.__updatePrimaryProperties()
        this.__updateProperty('delegate')
        this.__initView(true)
    }

    __complete() {
        this.__initView(true)
        super.__complete()
    }

    indexAt(x, y) {

    }
    itemAt(x, y) {

    }

    itemAtIndex(index) {
        return index >= 0 && index < this.__items.length.get() ? this.__items[index] : undefined
    }
    positionViewAtBeginning() {
        this.positionViewAtIndex(0, ListView.Beginning)
    }
    positionViewAtEnd() {
        this.positionViewAtIndex(this.__items.length.get() - 1, ListView.Beginning)


    }
    positionViewAtIndex(index, mode) {
        let pos = 'start'
        switch(mode){
            case ListView.Beginning: pos = 'start'; break;
            case ListView.Center: pos = 'center'; break;
            case ListView.End: pos = 'end'; break;
            case ListView.Visible: break;
            case ListView.Contain: {
                if(this.__items[index]){
                    if(this.orientation === ListView.Horizontal){
                        if(this.contentWidth <= this.width) return

                        if(this.__items[index].x <= this.contentX){
                            this.contentX = this.__items[index].x
                        } else if(this.__items[index].x + this.__items[index].width >= this.contentX + this.width){
                            this.contentX = this.__items[index].x + this.__items[index].width - this.width
                        }
                    } else {
                        if(this.contentHeight <= this.height) return

                        if(this.__items[index].y <= this.contentY){
                            this.contentY = this.__items[index].y
                        } else if(this.__items[index].y + this.__items[index].height >= this.contentY + this.height){
                            this.contentY = this.__items[index].y + this.__items[index].height - this.height
                        }
                    }
                }
                break;
            }
        }

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

    SLOT_delegateChanged() {
        this.__clear()
        this.__initView(this.__completed)
    }

    __clear() {
        this.blockSignals(true)

        let removed = this.__items
        this.__items = []

        for (let c of this.__cache) {
            if (c) c.destroy()
        }

        this.__cache = []

        for (let r of removed) {
            if (r) r.destroy()
        }

        // this.__middleWidth = 0
        // this.__middleHeight = 0

        this.originX = 0
        this.originY = 0

        this.contentX = 0
        this.contentY = 0

        this.blockSignals(false)

        this.count = 0
    }

    __getItemInfo(index) {
        let x = 0
        let y = 0
        let width = 0
        let height = 0
        let exist = false
        let inner = false

        if (this.__items[index]) {
            exist = true

            if (this.orientation === ListView.Horizontal) {
                x = this.__items[index].x
                width = this.__items[index].width

                if (x + width < this.contentX - this.cacheBuffer || x > this.contentX + this.width + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            } else {
                y = this.__items[index].y
                height = this.__items[index].height

                if (y + height < this.contentY - this.cacheBuffer || y > this.contentY + this.height + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            }
        } else {
            exist = false

            if (this.orientation === ListView.Horizontal) {
                x = 0
                if (this.__items[index - 1]) {
                    x = this.__items[index - 1].x + this.__items[index - 1].width + this.spacing
                } else if (this.__items[index + 1]) {
                    x = this.__items[index + 1].x - this.__middleWidth - this.spacing
                } else {
                    if (index === 0) {
                        x = this.originX
                    } else {
                        x = this.originX + (this.__middleWidth + this.spacing) * index
                    }

                }
                width = this.__middleWidth

                if (x + width < this.contentX - this.cacheBuffer || x > this.contentX + this.width + this.cacheBuffer) {
                    inner = false
                } else {
                    inner = true
                }
            } else {
                y = 0
                if (this.__items[index - 1]) {
                    y = this.__items[index - 1].y + this.__items[index - 1].height + this.spacing
                } else if (this.__items[index + 1]) {
                    y = this.__items[index + 1].y - this.__middleHeight - this.spacing
                } else {
                    if (index === 0) {
                        y = this.originY
                    } else {
                        y = this.originY + (this.__middleHeight + this.spacing) * index
                    }
                }
                height = this.__middleHeight

                if (y + height < this.contentY - this.cacheBuffer || y > this.contentY + this.height + this.cacheBuffer) {
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

    __toCache(item){
        if(!item) return

        if(this.reuseItems){
            this.__cache.push(item) 

            if(item instanceof JQModules.QtQuick.Item) {
                this.contentItem.__getDOM().removeChild(item.__getDOM())

                item['ListView.pooled']()
            }
        } else {
            item.destroy()
        }
    }

    __fromCache(){
        let item = this.__cache.pop()
        if(item instanceof JQModules.QtQuick.Item) {
            this.contentItem.__getDOM().appendChild(item.__getDOM())
        }

        return item
    }

    __createItem(index, itemInfo) {
        let properties = {}

        if (Array.isArray(this.model)) {
            properties.modelData = this.model[index]
            properties.model = { index: index }
        } else if (typeof this.model === 'object') {
            properties.model = this.model.data[index]
        } else {
            properties.model = { index: index }
        }

        let item = null
        
        if(this.__cache.length){
            properties.index = index

            item = this.__fromCache()
            this.__items[index] = item

            // for(let key in properties){
            //     item[key] = properties[key]
            // }

            if(properties.model){
                let keys = Object.keys(properties.model)
                if(keys.length === 1){
                    item.JQAbstractModelData = properties.model[keys[0]]
                }
                item.JQAbstractModel = properties.model
                delete properties.model
            }

            if(properties.modelData){
                item.JQAbstractModelData = properties.modelData
                delete properties.modelData
            }

            item.x = itemInfo.x
            item.y = itemInfo.y

            item['ListView.reused']()
        } else {
            item = this.delegate.createObject(this.contentItem, properties, true)

            this.__items[index] = item

            item.xChanged.connect(() => {
                if (this.orientation === ListView.Horizontal) {
                    let _index = item.JQAbstractModel.index
                    if (_index >= 0 && this.__items[_index + 1]) {
                        this.__items[_index + 1].x = this.__items[_index].x + this.__items[_index].width + this.spacing
                    }
                }
                JQApplication.updateLater(this)
            })
            item.yChanged.connect(() => {
                if (this.orientation === ListView.Vertical) {
                    let _index = item.JQAbstractModel.index
                    if (_index >= 0 && this.__items[_index + 1]) {
                        this.__items[_index + 1].y = this.__items[_index].y + this.__items[_index].height + this.spacing
                    }
                }
                JQApplication.updateLater(this)
            })
            item.widthChanged.connect(() => {
                if (this.orientation === ListView.Horizontal) {
                    let _index = item.JQAbstractModel.index
                    if (_index >= 0 && this.__items[_index + 1]) {
                        this.__items[_index + 1].x = this.__items[_index].x + this.__items[_index].width + this.spacing
                    }
                }
                JQApplication.updateLater(this)
            })
            item.heightChanged.connect(() => {
                if (this.orientation === ListView.Vertical) {
                    let _index = item.JQAbstractModel.index
                    if (_index >= 0 && this.__items[_index + 1]) {
                        this.__items[_index + 1].y = this.__items[_index].y + this.__items[_index].height + this.spacing
                    }
                }
                JQApplication.updateLater(this)
            })
            item.visibleChanged.connect(() => {
                JQApplication.updateLater(this)
            })
            item.indexChanged.connect(() => {
                let _index = item.JQAbstractModel.index
                if (this.orientation === ListView.Horizontal) {
                    if(_index > 0){
                        item.x = this.__items[_index - 1].x + this.__items[_index - 1].width + this.spacing
                    } else {
                        item.x = this.originX
                    }
                } else {
                    if(_index > 0){
                        item.y = this.__items[_index - 1].y + this.__items[_index - 1].height + this.spacing
                    } else {
                        item.y = this.originY
                    }
                }
                JQApplication.updateLater(this)
            })

            item.x = itemInfo.x
            item.y = itemInfo.y
        }

        return item
    }

    __initView(isCompleted) {
        if (this.delegate && this.model && isCompleted) {
            let length = 0
            if (Array.isArray(this.model)) {
                length = this.model.length
            } else if (typeof this.model === 'object') {
                if(this.model.__changeSet.length > 0) return
                length = this.model.count
            } else if (typeof this.model === 'number') {
                length = this.model
            } else {
                return
            }

            if (length === 0) return

            let countChanged = false

            if (this.count !== length) {
                countChanged = true
            }

            this.__self.count = length

            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            for (let i = 0; i < length; i++) {
                let itemInfo = this.__getItemInfo(i)
                if (itemInfo.inner) {
                    if (!itemInfo.exist) {
                        if (this.__createItem(i, itemInfo)) this.__updateGeometry()
                    }
                }
            }

            if (countChanged) this.countChanged()

            JQApplication.endUpdate()
        }
    }

    __updateView(changeSet) {
        if (this.delegate && this.model && this.__completed) {
            this.__updating = true
            let length = 0
            if (Array.isArray(this.model)) {
                length = this.model.length
            } else if (typeof this.model === 'object') {
                length = this.model.count
            } else if (typeof this.model === 'number') {
                length = this.model
            } else {
                return
            }

            if (length === 0 && this.__items.length === 0) return

            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            let countChanged = false

            if (this.count !== length) {
                countChanged = true
            }

            this.__self.count = length

            for (let change of changeSet) {
                let leftTop = change[0]
                let bottomRight = change[1]
                let role = change[2]

                if (role === 'append') {
                    for (let i = leftTop; i < bottomRight; i++) {
                        let itemInfo = this.__getItemInfo(i)
                        if (itemInfo.inner) {
                            if (!itemInfo.exist) {
                                if (this.__createItem(i, itemInfo)) this.__updateGeometry()
                            }
                        }
                    }
                } else if (role === 'insert') {
                    for (let i = leftTop; i < bottomRight; i++) {
                        this.__items.splice(i, 0, undefined)
                        let itemInfo = this.__getItemInfo(i)
                        if (itemInfo.inner) {
                            if (!itemInfo.exist) {
                                if (this.__createItem(i, itemInfo)) this.__updateGeometry()
                            }
                        }
                    }
                } else if (role === 'remove') {
                    let removed = this.__items.splice(leftTop, bottomRight - leftTop)
                    for (let r of removed) {
                        if (r) this.__toCache(r)
                    }
                }
            }

            let firstIndex = 0
            let lastIndex = 0

            for (let i = 0; i < length; i++) {
                if (this.__items[i] && !this.__items[i - 1] && !firstIndex) firstIndex = i
                if (this.__items[i] && !this.__items[i + 1] && !lastIndex) lastIndex = i
            }

            for (let i = firstIndex; i < length; i++) {
                let itemInfo = this.__getItemInfo(i)
                if (itemInfo.inner) {
                    if (!itemInfo.exist) {
                        if (this.__items[i] = this.__createItem(i, itemInfo)) this.__updateGeometry()
                    }
                } else if (itemInfo.exist) {
                    this.__toCache(this.__items[i])
                    this.__items[i] = undefined
                }

            }
            for (let i = lastIndex; i >= 0; i--) {
                let itemInfo = this.__getItemInfo(i)
                if (itemInfo.inner) {
                    if (!itemInfo.exist) {
                        if (this.__items[i] = this.__createItem(i, itemInfo)) this.__updateGeometry()
                    }
                } else if (itemInfo.exist) {
                    this.__toCache(this.__items[i])
                    this.__items[i] = undefined
                }
            }

            if (countChanged) this.countChanged()

            JQApplication.endUpdate()
            delete this.__updating
        }
    }

    SLOT_cacheBufferChanged(oldValue, newValue) {
        JQApplication.updateLater(this)
    }

    SLOT_contentXChanged(oldValue, newValue) {
        super.SLOT_contentXChanged(oldValue, newValue)

        if (this.orientation === ListView.Horizontal) {
            JQApplication.updateLater(this)
        }
    }

    SLOT_contentYChanged(oldValue, newValue) {
        super.SLOT_contentYChanged(oldValue, newValue)

        if (this.orientation === ListView.Vertical) {
            JQApplication.updateLater(this)
        }
    }

    SLOT_widthChanged(oldValue, newValue) {
        super.SLOT_widthChanged(oldValue, newValue)

        if (this.orientation === ListView.Horizontal) {
            JQApplication.updateLater(this)
        } else {
            this.contentItem.width = newValue
        }
    }

    SLOT_heightChanged(oldValue, newValue) {
        super.SLOT_heightChanged(oldValue, newValue)

        if (this.orientation === ListView.Vertical) {
            JQApplication.updateLater(this)
        } else {
            this.contentItem.height = newValue
        }
    }

    SLOT_spacingChanged(oldValue, newValue) {
        JQApplication.updateLater(this)
    }

    SLOT_orientationChanged(oldValue, newValue) {
        for (let i = 0; i < this.__items.length; i++) {
            let itemInfo = this.__getItemInfo(i)

            if (itemInfo.exist) {
                if (newValue === ListView.Horizontal) {
                    this.originY = 0
                } else {
                    this.originX = 0
                }

                this.__items[i].x = itemInfo.x
                this.__items[i].y = itemInfo.y
            }

        }
        JQApplication.updateLater(this)
    }

    __updateGeometry() {
        if (!this.__items.length) return

        let model = this.model
        if (Array.isArray(model)) {
            length = model.length
        } else if (typeof model === 'object') {
            length = model.count
        } else if (typeof model === 'number') {
            length = model
        } else {
            return
        }

        let lastIndex = 0
        let firstIndex = this.__items.length - 1
        let minX = Infinity
        let minY = Infinity

        let visibleCount = 0
        let visibleContentWidth = 0
        let visibleContentHeight = 0

        for (let i = 0; i < this.__items.length; i++) {
            if (this.__items[i]) {
                if (isNaN(this.__items[i].width) || this.__items[i].width === Infinity || this.__items[i].width === -Infinity ||
                    isNaN(this.__items[i].height) || this.__items[i].height === Infinity || this.__items[i].height === -Infinity) continue

                visibleCount++
                visibleContentWidth += this.__items[i].width
                visibleContentHeight += this.__items[i].height

                let x = this.__items[i].x
                let y = this.__items[i].y

                if (x < minX) minX = x
                if (y < minY) minY = y

                if (i < firstIndex) firstIndex = i
                if (i > lastIndex) lastIndex = i
            }
        }

        let middleWidth = visibleCount ? visibleContentWidth / visibleCount : 0
        let middleHeight = visibleCount ? visibleContentHeight / visibleCount : 0
        this.__middleWidth = middleWidth
        this.__middleHeight = middleHeight

        if (this.orientation === ListView.Horizontal) {
            this.contentWidth = visibleContentWidth + Math.round(middleWidth) * (length - visibleCount) + this.spacing * (length - 1)
            let originX = (minX - firstIndex * (Math.round(middleWidth + this.spacing)))
            if (originX !== Infinity && originX !== -Infinity) this.originX = originX

            Geometry.setAuto(this.__self, 'contentHeight', this.height, this.__self.constructor.meta.contentHeight)
            // this.__getDataQml('contentHeight').__setAuto(this.height)
        } else {
            this.contentHeight = visibleContentHeight + Math.round(middleHeight) * (length - visibleCount) + this.spacing * (length - 1)
            let originY = (minY - firstIndex * (Math.round(middleHeight + this.spacing)))
            if (originY !== Infinity && originY !== -Infinity) this.originY = originY

            // this.__getDataQml('contentWidth').__setAuto(this.width)
            Geometry.setAuto(this.__self, 'contentWidth', this.width, this.__self.constructor.meta.contentWidth)
        }
    }

    __endUpdate() {
        if(!this.__updating) this.__updateView([])
        this.__updateGeometry()
        super.__endUpdate()
    }

    __destroy() {
        if (this.model && typeof this.model === 'object' && !Array.isArray(this.model) && !this.model.__destroyed) {
            this.model.__removeViewListener(this)
        }
        this.__clear()
        super.__destroy()
    }
}



module.exports = ListView