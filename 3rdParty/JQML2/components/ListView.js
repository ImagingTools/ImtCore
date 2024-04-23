const { Flickable } = require('./Flickable')
const { QVar, QReal, QInt } = require('../utils/properties')

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
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx
        this.$queueData = []
        this.$items = {
            length: new QInt(0)
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

    $modelDataChanged(leftTop, bottomRight, roles){
        if(roles) {
            let prev = this.$queueData[this.$queueData.length-1]
            if(prev) {
                if(roles === 'insert'){
                    if(prev[2] === 'insert'){
                        if((leftTop >= prev[0] && leftTop <= prev[1]) || (bottomRight >= prev[0] && bottomRight <= prev[1])){
                            let newLeftTop = Math.min(leftTop, prev[0])
                            let newBottomRight = newLeftTop + (bottomRight - leftTop) + (prev[1] - prev[0])
                            prev[0] = newLeftTop
                            prev[1] = newBottomRight
                        } else {
                            this.$queueData.push([leftTop, bottomRight, roles])
                        }
                    } else if(prev[2] === 'remove'){
                        if((leftTop >= prev[0] && leftTop <= prev[1]) || (bottomRight >= prev[0] && bottomRight <= prev[1])){
                            let newLeftTop = Math.min(leftTop, prev[0])
                            let newBottomRight = newLeftTop - (bottomRight - leftTop) + (prev[1] - prev[0])
                            prev[0] = newLeftTop
                            prev[1] = newBottomRight
                            if(newBottomRight - newLeftTop === 0) this.$queueData.pop()
                        } else {
                            this.$queueData.push([leftTop, bottomRight, roles])
                        }
                    }
                } else if(roles === 'remove'){
                    if(prev[2] === 'insert'){
                        if((leftTop >= prev[0] && leftTop <= prev[1]) || (bottomRight >= prev[0] && bottomRight <= prev[1])){
                            let newLeftTop = Math.min(leftTop, prev[0])
                            let newBottomRight = newLeftTop - (bottomRight - leftTop) + (prev[1] - prev[0])
                            prev[0] = newLeftTop
                            prev[1] = newBottomRight
                            if(newBottomRight - newLeftTop === 0) this.$queueData.pop()
                        } else {
                            this.$queueData.push([leftTop, bottomRight, roles])
                        }
                    } else if(prev[2] === 'remove'){
                        if((leftTop >= prev[0] && leftTop <= prev[1]) || (bottomRight >= prev[0] && bottomRight <= prev[1])){
                            let newLeftTop = Math.min(leftTop, prev[0])
                            let newBottomRight = newLeftTop + (bottomRight - leftTop) + (prev[1] - prev[0])
                            prev[0] = newLeftTop
                            prev[1] = newBottomRight
                        } else {
                            this.$queueData.push([leftTop, bottomRight, roles])
                        }
                    }
                } else {
                    this.$queueData.push([leftTop, bottomRight, roles])
                }
            } else {
                this.$queueData.push([leftTop, bottomRight, roles])
            }
            
            
        }

        if(this.$timer) clearTimeout(this.$timer)
        this.$timer = setTimeout(()=>{
            this.$modelDataUpdate()
        }, 10)
        // console.log('DEBUG:::', leftTop, bottomRight, roles)
    }

    $modelDataUpdate(){
        while(this.$queueData.length){
            let params = this.$queueData.shift()
            let leftTop = params[0]
            let bottomRight = params[1]
            let roles = params[2]
            if(this.getPropertyValue('count') === 0) {
                break
            } else if(roles === 'append'){
                break
            } else if(roles === 'remove'){
                for(let i = leftTop; i < bottomRight; i++){
                    if(this.$items[i]){
                        let obj = this.$items[i]
                        delete this.$items[i]
                        obj.destroy()
                    }
                }
                for(let k = bottomRight; k < this.$items.length.get() + (bottomRight - leftTop); k++){
                    if(this.$items[k]){
                        this.$items[k-(bottomRight - leftTop)] = this.$items[k]
                        delete this.$items[k]
                    }
                }
                if(this.getPropertyValue('orientation') === ListView.Horizontal){
                    if(leftTop > 0){
                        if(this.$items[leftTop] && this.$items[leftTop-1]){
                            this.$items[leftTop].getStatement('x').reset(this.$items[leftTop-1].getStatement('x').get()+this.$items[leftTop-1].getStatement('width').get()+this.getStatement('spacing').get())
                        }
                    } else if(leftTop === 0){
                        if(this.$items[leftTop]){
                            this.$items[leftTop].getStatement('x').reset(0)
                        }
                    }
                } else {
                    if(leftTop > 0){
                        if(this.$items[leftTop] && this.$items[leftTop-1]){
                            this.$items[leftTop].getStatement('y').reset(this.$items[leftTop-1].getStatement('y').get()+this.$items[leftTop-1].getStatement('height').get()+this.getStatement('spacing').get())
                        }
                    } else if(leftTop === 0){
                        if(this.$items[leftTop]){
                            this.$items[leftTop].getStatement('y').reset(0)
                        }
                        
                    }
                }

                this.getProperty('count').reset(this.$items.length.get())
                this.updateGeometry()
            } else if(roles === 'insert'){
                let needClear = false
                let clearIndex = 0
                for(let i = leftTop; i < bottomRight; i++){
                    if(this.$items[i]){
                        let length = this.$items.length.get()
                        for(let k = length; k > i; k--){
                            if(this.$items[k-1] && !this.$items[k-1].UID) delete this.$items[k-1]
                            if(this.$items[k-1]) this.$items[k] = this.$items[k-1]
                            
                        }
                        this.$items[i] = null
                    }
                    // this.createElement(i) 
                    // this.updateGeometry()
                    if(this.getPropertyValue('orientation') === ListView.Horizontal){
                        if(this.$items[i-1] && this.$items[i-1].UID){
                            if(this.$items[i-1].getPropertyValue('x') + this.$items[i-1].getPropertyValue('width') + this.getPropertyValue('spacing') < this.getPropertyValue('width') + this.getPropertyValue('contentX') + this.getPropertyValue('cacheBuffer')){
                                let obj = this.createElement(i)
                                obj.getStatement('x').reset(this.$items[i-1].getPropertyValue('x') + this.$items[i-1].getPropertyValue('width') + this.getPropertyValue('spacing'))
                                
                                // if(this.$items[i+1]){
                                //     this.$items[i+1].getStatement('x').reset(this.$items[i].getPropertyValue('x') + this.$items[i].getPropertyValue('width') + this.getPropertyValue('spacing'))
                                // }

                                this.updateGeometry()
                            } else {
                                delete this.$items[i]
                                needClear = true
                                clearIndex = i + 1
                                break
                            }
                        } else if(this.$items[i+1] && this.$items[i+1].UID){
                            if(this.$items[i+1].getPropertyValue('x') - this.getPropertyValue('spacing') > this.getPropertyValue('contentX') - this.getPropertyValue('cacheBuffer')){
                                let obj = this.createElement(i)
                                obj.getStatement('x').reset(this.$items[i+1].getPropertyValue('x') - obj.getPropertyValue('width') - this.getPropertyValue('spacing'))
                                this.updateGeometry()
                            } else {
                                delete this.$items[i]
                                needClear = true
                                clearIndex = i + 1
                                break
                            }
                        }
                    } else {
                        if(this.$items[i-1] && this.$items[i-1].UID){
                            if(this.$items[i-1].getPropertyValue('y') + this.$items[i-1].getPropertyValue('height') + this.getPropertyValue('spacing') < this.getPropertyValue('height') + this.getPropertyValue('contentY') + this.getPropertyValue('cacheBuffer')){
                                let obj = this.createElement(i)
                                obj.getStatement('y').reset(this.$items[i-1].getPropertyValue('y') + this.$items[i-1].getPropertyValue('height') + this.getPropertyValue('spacing'))
                                
                                // if(this.$items[i+1]){
                                //     this.$items[i+1].getStatement('y').reset(this.$items[i].getPropertyValue('y') + this.$items[i].getPropertyValue('height') + this.getPropertyValue('spacing'))
                                // }

                                this.updateGeometry()
                            } else {
                                delete this.$items[i]
                                needClear = true
                                clearIndex = i + 1
                                break
                            }
                        } else if(this.$items[i+1] && this.$items[i+1].UID){
                            if(this.$items[i+1].UID && this.$items[i+1].getPropertyValue('y') - this.getPropertyValue('spacing') > this.getPropertyValue('contentY') - this.getPropertyValue('cacheBuffer')){
                                let obj = this.createElement(i)
                                obj.getStatement('y').reset(this.$items[i+1].getPropertyValue('y') - obj.getPropertyValue('height') - this.getPropertyValue('spacing'))
                                this.updateGeometry()
                            } else {
                                delete this.$items[i]
                                needClear = true
                                clearIndex = i + 1
                                break
                            }
                        }
                    }
                    this.getProperty('count').reset(this.$items.length.get())
                    this.updateGeometry()
                }
                if(needClear){
                    let length = this.$items.length.get()
                    for(let i = clearIndex; i < length; i++){
                        if(this.$items[i] && this.$items[i].UID){
                            this.$items[i].destroy()
                            delete this.$items[i]
                        }
                    }
                }
                this.getProperty('count').reset(this.$items.length.get())
            }
        }
        this.updateView()
        this.getProperty('count').reset(this.$items.length.get())
        this.updateGeometry()
    }

    $disconnectModel(){
        if(this.$model && this.$model instanceof ListModel && this.$model.UID){
            this.$model.getProperty('data').getNotify().disconnect(this, this.$modelDataChanged)
        }
    }
    $connectModel(model){
        if(model && model instanceof ListModel){
            this.$model = model
            model.getProperty('data').getNotify().connect(this, this.$modelDataChanged)
        }
    }
    $modelChanged(){
        this.$queueData = []
        this.$disconnectModel()
        for(let key in this.$items){
            if(key !== 'length') {
                let obj = this.$items[key]
                delete this.$items[key]
                obj.destroy()
                
            }
        }

        this.getProperty('contentX').value = 0
        this.getProperty('contentY').value = 0
        this.getProperty('originX').value = 0
        this.getProperty('originY').value = 0
        this.getStatement('contentItem').get().getStatement('x').reset(-this.getStatement('contentX').get())
        this.getStatement('contentItem').get().getStatement('y').reset(-this.getStatement('contentY').get())

        this.$items.length.reset(0)
        if(typeof this.getPropertyValue('model') === 'number'){     
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getProperty('model')); return this.getPropertyValue('model')})
        } else {
            this.$connectModel(this.getPropertyValue('model'))
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getPropertyValue('model').getProperty('data')); return this.getPropertyValue('model').getPropertyValue('data').length}) 
            
        }
        this.$items.length.update()
        this.updateView()
    }

    $delegateChanged(){
        this.$queueData = []
        for(let key in this.$items){
            if(key !== 'length') {
                let obj = this.$items[key]
                delete this.$items[key]
                obj.destroy()
            }
        }
        this.updateView()
    }

    updateGeometry(){
        if(!this.$items.length) return 
        let visibleContentWidth = 0
        let visibleContentHeight = 0
        let lastIndex = 0
        let firstIndex = this.$items.length.get()-1
        let minX = Infinity
        let minY = Infinity

        if(this.getPropertyValue('orientation') === ListView.Horizontal){
            for(let child of this.getStatement('contentItem').get().getProperty('children').get()){  
                if(child.getStatement('index').get() < firstIndex) firstIndex = child.getStatement('index').get()
                if(child.getStatement('index').get() > lastIndex) lastIndex = child.getStatement('index').get()
                if(child.getPropertyValue('x') < minX) minX = child.getPropertyValue('x')
                visibleContentWidth += child.getPropertyValue('width')
            }
            let middleWidth = this.getStatement('contentItem').get().getProperty('children').get().length ? visibleContentWidth / this.getStatement('contentItem').get().getProperty('children').get().length : 0

            this.getStatement('contentWidth').reset(visibleContentWidth + Math.round(middleWidth)*(this.$items.length.get()-this.getStatement('contentItem').get().getProperty('children').get().length) + this.getPropertyValue('spacing') * (this.$items.length.get()-1))
            let originX = (minX - firstIndex*(Math.round(middleWidth+this.getPropertyValue('spacing'))))
            if(originX && originX !== Infinity && originX !== -Infinity) this.getStatement('originX').reset(originX)
            
            this.getStatement('contentHeight').setAuto(this.getPropertyValue('height'))
        } else {
            for(let child of this.getStatement('contentItem').get().getProperty('children').get()){  
                if(child.getStatement('index').get() < firstIndex) firstIndex = child.getStatement('index').get()
                if(child.getStatement('index').get() > lastIndex) lastIndex = child.getStatement('index').get()
                if(child.getPropertyValue('y') < minY) minY = child.getPropertyValue('y')
                visibleContentHeight += child.getPropertyValue('height')
            }
            let middleHeight = this.getStatement('contentItem').get().getProperty('children').get().length ? visibleContentHeight / this.getStatement('contentItem').get().getProperty('children').get().length : 0

            this.getStatement('contentHeight').reset(visibleContentHeight + Math.round(middleHeight)*(this.$items.length.get()-this.getStatement('contentItem').get().getProperty('children').get().length) + this.getPropertyValue('spacing') * (this.$items.length.get()-1))
            let originY = (minY - firstIndex*(Math.round(middleHeight+this.getPropertyValue('spacing'))))
            if(originY && originY !== Infinity && originY !== -Infinity) this.getStatement('originY').reset(originY)

            this.getStatement('contentWidth').setAuto(this.getPropertyValue('width'))
        }
        
        
    }

    prepare(){
        let children = this.getStatement('contentItem').get().getProperty('children').get().slice()

        let leftIndex = this.getStatement('contentItem').get().getProperty('children').get().length ? this.getStatement('contentItem').get().getProperty('children').get()[0].getStatement('index').get() : 0
        let rightIndex = 0
        let visibleCount = this.getStatement('contentItem').get().getProperty('children').get().length
        let maxX = 0
        let maxY = 0
        let minX = 0
        let minY = 0
        let middleWidth = 0
        let middleHeight = 0

        
        let countChanged = this.getPropertyValue('count') !== this.$items.length.get()
        this.getProperty('count').value = this.$items.length.get()
        
        if(countChanged && this.$items.length.get() === 0) {
            for(let child of children){
                child.destroy()
            }
            this.getProperty('count').getNotify()()
            return
        }

        while(children.length){
            let child = children.pop()

            middleWidth += child.getPropertyValue('width')
            middleHeight += child.getPropertyValue('height')

            if(child.getPropertyValue('index') > rightIndex){
                rightIndex = child.getPropertyValue('index')
            }
            if(child.getPropertyValue('index') < leftIndex){
                leftIndex = child.getPropertyValue('index')
            }
            if(child.getPropertyValue('x') + child.getPropertyValue('width') > maxX){
                maxX = child.getPropertyValue('x') + child.getPropertyValue('width')
            }
            if(child.getPropertyValue('y') + child.getPropertyValue('height') > maxY){
                maxY = child.getPropertyValue('y') + child.getPropertyValue('height')
            }
            if(child.getPropertyValue('x') < minX){
                minX = child.getPropertyValue('x')
            }
            if(child.getPropertyValue('y') < minY){
                minY = child.getPropertyValue('y')
            }

            if(this.getPropertyValue('orientation') === ListView.Horizontal){            
                if(child.getPropertyValue('x') + child.getPropertyValue('width') < this.getPropertyValue('contentX')-this.getPropertyValue('cacheBuffer') || child.getPropertyValue('x') > this.getPropertyValue('contentX') + this.getPropertyValue('width') + this.getPropertyValue('cacheBuffer')) {
                    delete this.$items[child.getStatement('index').get()]
                    child.destroy()
                    this.updateGeometry()
                }
            } else {
                
                if(child.getPropertyValue('y') + child.getPropertyValue('height') < this.getPropertyValue('contentY')-this.getPropertyValue('cacheBuffer') || child.getPropertyValue('y') > this.getPropertyValue('contentY') + this.getPropertyValue('height') + this.getPropertyValue('cacheBuffer')) {
                    delete this.$items[child.getStatement('index').get()]
                    child.destroy()
                    this.updateGeometry()
                }
            }
        }

        middleWidth = visibleCount ? middleWidth / visibleCount : 0
        middleHeight = visibleCount ? middleHeight / visibleCount : 0
        let currentIndex = 0

        if(this.getStatement('contentItem').get().getProperty('children').get().length === 0){
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(this.getPropertyValue('contentX') <= minX){
                    currentIndex = leftIndex + Math.ceil((this.getPropertyValue('contentX') - minX + (middleWidth ? this.getPropertyValue('spacing') : 0))/(Math.round(middleWidth + (middleWidth ? this.getPropertyValue('spacing') : 0))))
                    if(isNaN(currentIndex) || currentIndex === -Infinity || currentIndex === Infinity) currentIndex = leftIndex
                } else {
                    currentIndex = rightIndex + Math.ceil((this.getPropertyValue('contentX') - maxX - (middleWidth ? this.getPropertyValue('spacing') : 0))/(Math.round(middleWidth + (middleWidth ? this.getPropertyValue('spacing') : 0))))
                    if(isNaN(currentIndex) || currentIndex === -Infinity || currentIndex === Infinity) currentIndex = rightIndex
                }
                if(currentIndex < 0 || currentIndex >= this.$items.length.get()) return
                
                // currentIndex = (this.getPropertyValue('contentX') <= minX ? leftIndex : rightIndex) + (Math.round(middleWidth)*(this.$items.length.get() - visibleCount))

                if(!this.$items[currentIndex] && currentIndex >= 0 && currentIndex < this.$items.length.get()){
                    let obj = this.createElement(currentIndex)
                    if(this.contentY <= minX){
                        obj.getStatement('x').reset(minX + (Math.round(middleWidth + this.getPropertyValue('spacing'))*(currentIndex-leftIndex)))
                    } else {
                        obj.getStatement('x').reset(maxX + (Math.round(middleWidth + this.getPropertyValue('spacing'))*(currentIndex-rightIndex-1)))
                    }
                    // obj.x = middleWidth*currentIndex + this.getPropertyValue('spacing')*(currentIndex)
                    obj.getStatement('y').reset(0)

                    this.updateGeometry()
                }
            } else {
                if(this.getPropertyValue('contentY') <= minY){
                    currentIndex = leftIndex + Math.ceil((this.getPropertyValue('contentY') - minY + (middleHeight ? this.getPropertyValue('spacing') : 0))/(Math.round(middleHeight + (middleHeight ? this.getPropertyValue('spacing') : 0))))
                    if(isNaN(currentIndex) || currentIndex === -Infinity || currentIndex === Infinity) currentIndex = leftIndex
                } else {
                    currentIndex = rightIndex + Math.ceil((this.getPropertyValue('contentY') - maxY - (middleHeight ? this.getPropertyValue('spacing') : 0))/(Math.round(middleHeight + (middleHeight ? this.getPropertyValue('spacing') : 0))))
                    if(isNaN(currentIndex) || currentIndex === -Infinity || currentIndex === Infinity) currentIndex = rightIndex
                }
                if(currentIndex < 0 || currentIndex >= this.$items.length.get()) return
                
                // currentIndex = (this.getPropertyValue('contentY') <= minY ? leftIndex : rightIndex) + (Math.round(middleHeight)*(this.$items.length.get() - visibleCount))

                if(!this.$items[currentIndex] && currentIndex >= 0 && currentIndex < this.$items.length.get()){
                    let obj = this.createElement(currentIndex)
                    obj.getStatement('x').reset(0)
                    if(this.getPropertyValue('contentY') <= minY){
                        obj.getStatement('y').reset(minY + (Math.round(middleHeight + this.getPropertyValue('spacing'))*(currentIndex-leftIndex)))
                    } else {
                        obj.getStatement('y').reset(maxY + (Math.round(middleHeight + this.getPropertyValue('spacing'))*(currentIndex-rightIndex-1)))
                    }
                    // obj.y = middleHeight*currentIndex + this.getPropertyValue('spacing')*(currentIndex)

                    this.updateGeometry()
                }
            }
            leftIndex = currentIndex
            rightIndex = currentIndex

            
            
        }

        for(let index = leftIndex - 1; index >= 0; index--){
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(this.$items[index+1] && this.$items[index+1].getPropertyValue('x') - this.getPropertyValue('spacing') > this.getPropertyValue('contentX') - this.getPropertyValue('cacheBuffer')){
                    let obj = this.createElement(index)
                    obj.getStatement('x').reset(this.$items[index+1].getPropertyValue('x') - obj.getPropertyValue('width') - this.getPropertyValue('spacing'))
                    this.updateGeometry()
                } else {
                    break
                }
            } else {
                if(this.$items[index+1] && this.$items[index+1].getPropertyValue('y') - this.getPropertyValue('spacing') > this.getPropertyValue('contentY') - this.getPropertyValue('cacheBuffer')){
                    let obj = this.createElement(index)
                    obj.getStatement('y').reset(this.$items[index+1].getPropertyValue('y') - obj.getPropertyValue('height') - this.getPropertyValue('spacing'))
                    this.updateGeometry()
                } else {
                    break
                }
            }
        }

        for(let index = rightIndex + 1; index < this.$items.length.get(); index++){
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(this.$items[index-1] && this.$items[index-1].getPropertyValue('x') + this.$items[index-1].getPropertyValue('width') + this.getPropertyValue('spacing') < this.getPropertyValue('width') + this.getPropertyValue('contentX') + this.getPropertyValue('cacheBuffer')){
                    let obj = this.createElement(index)
                    obj.getStatement('x').reset(this.$items[index-1].getPropertyValue('x') + this.$items[index-1].getPropertyValue('width') + this.getPropertyValue('spacing'))
                    this.updateGeometry()
                } else {
                    break
                }
            } else {
                if(this.$items[index-1] && this.$items[index-1].getPropertyValue('y') + this.$items[index-1].getPropertyValue('height') + this.getPropertyValue('spacing') < this.getPropertyValue('height') + this.getPropertyValue('contentY') + this.getPropertyValue('cacheBuffer')){
                    let obj = this.createElement(index)
                    obj.getStatement('y').reset(this.$items[index-1].getPropertyValue('y') + this.$items[index-1].getPropertyValue('height') + this.getPropertyValue('spacing'))
                    this.updateGeometry()
                } else {
                    break
                }
            }
        }

        this.getProperty('count').value = this.$items.length.get()
        if(countChanged) this.getProperty('count').getNotify()()
    }

    $contentXChanged(){
        this.getStatement('contentItem').get().getStatement('x').reset(-this.getStatement('contentX').get())
        this.updateView()
    }

    $contentYChanged(){
        this.getStatement('contentItem').get().getStatement('y').reset(-this.getStatement('contentY').get())
        this.updateView()
    }

    // $contentXChanged(){
    //     super.$contentXChanged()
    //     this.updateView()
    // }
    // $contentYChanged(){
    //     super.$contentYChanged()
    //     this.updateView()     
    // }
    $contentWidthChanged(){
        this.getStatement('contentItem').get().getStatement('width').reset(this.getStatement('contentWidth').get())
        if(this.getPropertyValue('contentWidth') < this.getPropertyValue('width')){
            this.getProperty('contentX').reset(0)
        }
        // if(this.getPropertyValue('contentX') > this.getPropertyValue('contentWidth') - this.getPropertyValue('width')){
        //     this.getProperty('contentX').reset(this.getPropertyValue('contentWidth') - this.getPropertyValue('width'))
        // }
        // if(this.getPropertyValue('contentX') > this.getPropertyValue('contentWidth') - this.getPropertyValue('width')){
        //     this.getProperty('contentX').reset(this.getPropertyValue('contentWidth') - this.getPropertyValue('width'))
        // }
    }

    $contentHeightChanged(){
        this.getStatement('contentItem').get().getStatement('height').reset(this.getStatement('contentHeight').get())
        if(this.getPropertyValue('contentWidth') < this.getPropertyValue('width')){
            this.getProperty('contentY').reset(0)
        }
        // if(this.getPropertyValue('contentY') > this.getPropertyValue('contentHeight') - this.getPropertyValue('height')){
        //     this.getProperty('contentY').reset(this.getPropertyValue('contentHeight') - this.getPropertyValue('height'))
        // }
        // if(this.getPropertyValue('contentY') > this.getPropertyValue('contentHeight') - this.getPropertyValue('height')){
        //     this.getProperty('contentY').reset(this.getPropertyValue('contentHeight') - this.getPropertyValue('height'))
        // }
    }
    $widthChanged(){
        super.$widthChanged()
        if(this.getPropertyValue('orientation') === ListView.Vertical) this.getProperty('contentWidth').reset(this.getPropertyValue('width'))
        // this.updateView()
        this.$modelDataUpdate()
    }
    $heightChanged(){
        super.$heightChanged()
        if(this.getPropertyValue('orientation') === ListView.Horizontal) this.getProperty('contentHeight').reset(this.getPropertyValue('height'))
        this.$modelDataUpdate()
    }
    $cacheBufferChanged(){
        this.$modelDataUpdate()
    }
    createElement(index){ 
        if(this.$items[index]) return this.$items[index]
        let ctx = new ContextController(this.delegate.get().$exCtx, this.$exCtx)
        let createObject = this.getStatement('delegate').get().createObject
        let cls = this.getStatement('delegate').get().constructor

        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = createObject ? createObject(this.getStatement('contentItem').get(),ctx, {index: index}, false) : new cls(this.getStatement('contentItem').get(),ctx, {index: index})
            // obj.getStatement('index').reset(index)
            // obj.getStatement('model').reset({index: index})
            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = createObject ? createObject(this.getStatement('contentItem').get(),ctx, model, false) : new cls(this.getStatement('contentItem').get(),ctx, model)
            // obj.getStatement('index').setCompute(()=>{return model.index})
            // obj.getStatement('index').update()
            // obj.getStatement('model').reset(model)
            this.$items[index] = obj   
        }
        // while(updateList.length){
            for(let update of updateList.splice(0, updateList.length)){
                update()
            }
        // }

        
        let obj = this.$items[index]
        
        if(obj.getPropertyValue('width') <= 0 || obj.getPropertyValue('height') <= 0) {
            obj.setStyle({
                visibility: 'hidden'
            })
        } else {
            obj.setStyle({
                visibility: 'visible'
            })
        }
        
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
                if(this.$items[obj.getPropertyValue('index')+1] && this.$items[obj.getPropertyValue('index')+1] !== obj){
                    this.$items[obj.getPropertyValue('index')+1].getProperty('x').reset(obj.getPropertyValue('x')+obj.getPropertyValue('width')+this.getPropertyValue('spacing'))
                }
                this.updateGeometry()
            }
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
                if(this.$items[obj.getPropertyValue('index')+1] && this.$items[obj.getPropertyValue('index')+1] !== obj){
                    this.$items[obj.getPropertyValue('index')+1].getProperty('y').reset(obj.getPropertyValue('y')+obj.getPropertyValue('height')+this.getPropertyValue('spacing'))
                }
                this.updateGeometry()
            }
        })
        obj.getProperty('x').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(this.$items[obj.getPropertyValue('index')+1] && this.$items[obj.getPropertyValue('index')+1] !== obj){
                    this.$items[obj.getPropertyValue('index')+1].getProperty('x').reset(obj.getPropertyValue('x')+obj.getPropertyValue('width')+this.getPropertyValue('spacing'))
                    this.updateGeometry()
                }
            }
        })
        obj.getProperty('y').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                if(this.$items[obj.getPropertyValue('index')+1] && this.$items[obj.getPropertyValue('index')+1] !== obj){
                    this.$items[obj.getPropertyValue('index')+1].getProperty('y').reset(obj.getPropertyValue('y')+obj.getPropertyValue('height')+this.getPropertyValue('spacing'))
                    this.updateGeometry()
                }
            }
        })
        obj.$complete()
        return obj
    }

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return
        this.$queueData = []
        this.prepare() 
    }

    onMouseMove(x, y){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')) {
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                if(this.getPropertyValue('contentHeight') <= this.getPropertyValue('height')){
                    this.getStatement('contentY').reset(0)
                    return true
                }
                if(this.getPropertyValue('contentY') + (y) > this.getPropertyValue('originY') && this.getPropertyValue('contentY') + (y) < this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height')){
                    this.getStatement('contentY').reset(this.getPropertyValue('contentY') + (y))
                    return false
                } else {
                    if(this.getPropertyValue('contentY') + (y) <= this.getPropertyValue('originY')) {
                        this.getStatement('contentY').reset(this.getPropertyValue('originY'))
                        // MouseController.stopPropogation(null)
                    }
                    if(this.getPropertyValue('contentY') + (y) >= this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height')) {
                        this.getStatement('contentY').reset(this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height'))
                        // MouseController.stopPropogation(null)
                    }
                    return true
                }
            } else {
                if(this.getPropertyValue('contentWidth') <= this.getPropertyValue('width')){
                    this.getStatement('contentX').reset(0)
                    return true
                }
                if(this.getPropertyValue('contentX') + (x) > this.getPropertyValue('originX') && this.getPropertyValue('contentX') + (x) < this.getPropertyValue('contentWidth') + this.getPropertyValue('originX') - this.getPropertyValue('width')){
                    this.getStatement('contentX').reset(this.getPropertyValue('contentX') + (x))
                    return false
                } else {
                    if(this.getPropertyValue('contentX') + (x) <= this.getPropertyValue('originX')) {
                        this.getStatement('contentX').reset(this.getPropertyValue('originX'))
                        // MouseController.stopPropogation(null)
                    }
                    if(this.getPropertyValue('contentX') + (x) >= this.getPropertyValue('contentWidth') + this.getPropertyValue('originX') - this.getPropertyValue('width')) {
                        this.getStatement('contentX').reset(this.getPropertyValue('contentWidth') + this.getPropertyValue('originX') - this.getPropertyValue('width'))
                        // MouseController.stopPropogation(null)
                    }
                    return true
                }
            }
            
        } else {
            return true
        }
    }

    onWheel(x, y, deltaX, deltaY){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')){
            if(this.getPropertyValue('contentHeight') <= this.getPropertyValue('height')){
                this.getStatement('contentY').reset(0)
                return true
            }
            // if(this.getPropertyValue('contentWidth') <= this.getPropertyValue('width')){
            //     this.getStatement('contentX').reset(0)
            //     return true
            // }
            // this.getStatement('contentX').reset(this.getStatement('contentX').get()+deltaX)
            // this.getStatement('contentY').reset(this.getStatement('contentY').get()+deltaY)
            if(this.getPropertyValue('contentY') + deltaY > this.getPropertyValue('originY') && this.getPropertyValue('contentY') + deltaY < this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height')){
                this.getStatement('contentY').reset(this.getPropertyValue('contentY') + deltaY)
                return false
            } else {
                if(this.getPropertyValue('contentY') + deltaY <= this.getPropertyValue('originY')) this.getStatement('contentY').reset(this.getPropertyValue('originY'))
                if(this.getPropertyValue('contentY') + deltaY >= this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height')) this.getStatement('contentY').reset(this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height'))
                return true
            }
        } else {
            return true
        }
    }

    destroy(){
        this.$disconnectModel()
        this.$items.length.destroy()
        delete this.$items.length

        for(let key in this.$items){
            let obj = this.$items[key]
            delete this.$items[key]
            obj.destroy()
        }
        
        super.destroy()
    }
}

module.exports.ListView = ListView