const { Flickable } = require('./Flickable')
const { QVar, QReal, QInt } = require('../utils/properties')

class ListView extends Flickable {
    static Horizontal = 0
    static Vertical = 1

    static Beginning = 0
    static Center = 1
    static End = 2

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
        this.$items = {
            length: new QInt(0)
        }
        this.$items.length.getNotify().connect(()=>{
            this.updateView()
        })
    }

    $modelChanged(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.$items.length.reset(0)
        if(typeof this.getPropertyValue('model') === 'number'){     
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getProperty('model')); return this.getPropertyValue('model')})
        } else {
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getPropertyValue('model').getProperty('data')); return this.getPropertyValue('model').getPropertyValue('data').length}) 
        }
        this.$items.length.update()
    }

    $delegateChanged(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.updateView()
    }

    updateGeometry(){
        let visibleContentWidth = 0
        let visibleContentHeight = 0
        let lastIndex = 0
        let firstIndex = this.$items.length.get()-1
        let minX = Infinity
        let minY = Infinity

        if(this.getPropertyValue('orientation') === ListView.Horizontal){
            for(let child of this.getStatement('contentItem').get().children()){  
                if(child.getStatement('index').get() < firstIndex) firstIndex = child.getStatement('index').get()
                if(child.getStatement('index').get() > lastIndex) lastIndex = child.getStatement('index').get()
                if(child.getPropertyValue('x') < minX) minX = child.getPropertyValue('x')
                visibleContentWidth += child.getPropertyValue('width')
            }
            let middleWidth = this.getStatement('contentItem').get().children().length ? visibleContentWidth / this.getStatement('contentItem').get().children().length : 0

            this.getStatement('contentWidth').reset(visibleContentWidth + Math.round(middleWidth)*(this.$items.length.get()-this.getStatement('contentItem').get().children().length) + this.getPropertyValue('spacing') * (this.$items.length.get()-1))
            let originX = (minX - firstIndex*(Math.round(middleWidth+this.getPropertyValue('spacing'))))
            if(originX && originX !== Infinity && originX !== -Infinity) this.getStatement('originX').reset(originX)
            
            this.getStatement('contentHeight').setAuto(this.getPropertyValue('height'))
        } else {
            for(let child of this.getStatement('contentItem').get().children()){  
                if(child.getStatement('index').get() < firstIndex) firstIndex = child.getStatement('index').get()
                if(child.getStatement('index').get() > lastIndex) lastIndex = child.getStatement('index').get()
                if(child.getPropertyValue('y') < minY) minY = child.getPropertyValue('y')
                visibleContentHeight += child.getPropertyValue('height')
            }
            let middleHeight = this.getStatement('contentItem').get().children().length ? visibleContentHeight / this.getStatement('contentItem').get().children().length : 0

            this.getStatement('contentHeight').reset(visibleContentHeight + Math.round(middleHeight)*(this.$items.length.get()-this.getStatement('contentItem').get().children().length) + this.getPropertyValue('spacing') * (this.$items.length.get()-1))
            let originY = (minY - firstIndex*(Math.round(middleHeight+this.getPropertyValue('spacing'))))
            if(originY && originY !== Infinity && originY !== -Infinity) this.getStatement('originY').reset(originY)

            this.getStatement('contentWidth').setAuto(this.getPropertyValue('width'))
        }
        
        
    }

    prepare(){
        let children = this.getStatement('contentItem').get().children().slice()

        let leftIndex = this.getStatement('contentItem').get().children().length ? this.getStatement('contentItem').get().children()[0].getStatement('index').get() : 0
        let rightIndex = 0
        let visibleCount = this.getStatement('contentItem').get().children().length
        let maxX = 0
        let maxY = 0
        let minX = 0
        let minY = 0
        let middleWidth = 0
        let middleHeight = 0

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
                    child.$destroy()
                    this.updateGeometry()
                }
            } else {
                
                if(child.getPropertyValue('y') + child.getPropertyValue('height') < this.getPropertyValue('contentY')-this.getPropertyValue('cacheBuffer') || child.getPropertyValue('y') > this.getPropertyValue('contentY') + this.getPropertyValue('height') + this.getPropertyValue('cacheBuffer')) {
                    delete this.$items[child.getStatement('index').get()]
                    child.$destroy()
                    this.updateGeometry()
                }
            }
        }

        middleWidth = visibleCount ? middleWidth / visibleCount : 0
        middleHeight = visibleCount ? middleHeight / visibleCount : 0
        let currentIndex = 0

        if(this.getStatement('contentItem').get().children().length === 0){
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

        this.getProperty('count').reset(this.$items.length.get())
    }

    $contentXChanged(){
        super.$contentXChanged()
        this.updateView()
    }
    $contentYChanged(){
        super.$contentYChanged()
        this.updateView()     
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
    $widthChanged(){
        super.$widthChanged()
        if(this.getPropertyValue('orientation') === ListView.Vertical) this.getProperty('contentWidth').reset(this.getPropertyValue('width'))
        this.updateView()
    }
    $heightChanged(){
        super.$heightChanged()
        if(this.getPropertyValue('orientation') === ListView.Horizontal) this.getProperty('contentHeight').reset(this.getPropertyValue('height'))
        this.updateView()
    }
    $cacheBufferChanged(){
        this.updateView()
    }
    createElement(index){
        if(this.$items[index]) return this.$items[index]
        let ctx = new ContextController(this.delegate.get().$exCtx, this.$exCtx)
        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = this.delegate.get().createObject(this.getStatement('contentItem').get(), ctx, {index: index})
            // obj.getStatement('index').reset(index)
            // obj.getStatement('model').reset({index: index})
            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = this.delegate.get().createObject(this.getStatement('contentItem').get(), ctx, model)
            // obj.getStatement('index').setCompute(()=>{return model.index})
            // obj.getStatement('index').update()
            // obj.getStatement('model').reset(model)
            this.$items[index] = obj   
        }
        for(let update of updateList.splice(0, updateList.length)){
            update()
        }
        this.$items[index].getProperty('width').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(this.$items[index+1]){
                    this.$items[index+1].getProperty('x').reset(this.$items[index].getPropertyValue('x')+this.$items[index].getPropertyValue('width')+this.getPropertyValue('spacing'))
                }
                this.updateGeometry()
            }
        })
        this.$items[index].getProperty('height').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                if(this.$items[index+1]){
                    this.$items[index+1].getProperty('y').reset(this.$items[index].getPropertyValue('y')+this.$items[index].getPropertyValue('height')+this.getPropertyValue('spacing'))
                }
                this.updateGeometry()
            }
        })
        this.$items[index].getProperty('x').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Horizontal){
                if(this.$items[index+1]){
                    this.$items[index+1].getProperty('x').reset(this.$items[index].getPropertyValue('x')+this.$items[index].getPropertyValue('width')+this.getPropertyValue('spacing'))
                    this.updateGeometry()
                }
            }
        })
        this.$items[index].getProperty('y').getNotify().connect(()=>{
            if(this.getPropertyValue('orientation') === ListView.Vertical){
                if(this.$items[index+1]){
                    this.$items[index+1].getProperty('y').reset(this.$items[index].getPropertyValue('y')+this.$items[index].getPropertyValue('height')+this.getPropertyValue('spacing'))
                    this.updateGeometry()
                }
            }
        })
        this.$items[index].$complete()
        return this.$items[index]
    }

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return

        this.prepare()

        
    }

    onMouseMove(x, y){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.getPropertyValue('interactive')) {
            if(this.getPropertyValue('contentHeight') <= this.getPropertyValue('height')){
                this.getStatement('contentY').reset(0)
                return true
            }
            if(this.getPropertyValue('contentY') + (this.$mouseY - y) > this.getPropertyValue('originY') && this.getPropertyValue('contentY') + (this.$mouseY - y) < this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height')){
                this.getStatement('contentY').reset(this.getPropertyValue('contentY') + (this.$mouseY - y))
                return false
            } else {
                if(this.getPropertyValue('contentY') + (this.$mouseY - y) <= this.getPropertyValue('originY')) {
                    this.getStatement('contentY').reset(this.getPropertyValue('originY'))
                    // MouseController.stopPropogation(null)
                }
                if(this.getPropertyValue('contentY') + (this.$mouseY - y) >= this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height')) {
                    this.getStatement('contentY').reset(this.getPropertyValue('contentHeight') + this.getPropertyValue('originY') - this.getPropertyValue('height'))
                    // MouseController.stopPropogation(null)
                }
                return true
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
            if(this.getPropertyValue('contentWidth') <= this.getPropertyValue('width')){
                this.getStatement('contentX').reset(0)
                return true
            }
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
}

module.exports.ListView = ListView