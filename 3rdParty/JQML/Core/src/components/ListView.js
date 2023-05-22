import {Flickable} from './Flickable'

export class ListView extends Flickable {

    static Horizontal = 0
    static Vertical = 1

    static Beginning = 0
    static Center = 1
    static End = 2

    static NoSnap = 0
    static SnapToItem = 1
    static SnapOneItem = 2

    $useModel = true
    $childrenForUpdate = []
    $items = []
    $middleWidth = 0
    $middleHeight = 0
    $enabledScroll = true
    $oldContentX = 0
    $oldContentY = 0

    constructor(args) {
        super(args)

        this.$cP('model', undefined).connect(this.$modelChanged.bind(this))
        this.$cP('delegate', undefined).connect(this.$delegateChanged.bind(this))
        this.$cP('count', 0)//.connect(this.$countChanged.bind(this))
        this.$cP('orientation', ListView.Vertical).connect(this.$orientationChanged.bind(this))
        this.$cP('spacing', 0).connect(this.$spacingChanged.bind(this))
        this.$cP('currentIndex', -1).connect(this.$currentIndexChanged.bind(this))
        this.$cP('currentItem', undefined)
        this.$cP('snapMode', ListView.NoSnap)
        this.$cP('cacheBuffer', 320).connect(this.$cacheBufferChanged.bind(this))

        this.$updateGeometry()
    }
    $domCreate(){
        super.$domCreate()
    }

    $orientationChanged(){
        this.$updateGeometry()
        this.$updateView()
    }

    // $contentWidthChanged(){

    // }
    // $contentHeightChanged(){
        
    // }
    $getOriginY(){
        if(this.orientation === ListView.Vertical){
            if(this.$items[0]) return this.$items[0].y

            let visibleContentHeight = 0
            let lastIndex = 0
            for(let child of this.contentItem.children){
                if(child.dom){
                    if(child.index > lastIndex) lastIndex = child.index
                    if(child.y + child.height > visibleContentHeight){
                        visibleContentHeight = child.y + child.height
                    }
                }
            }
            return this.contentHeight - (visibleContentHeight + (this.$items.length - lastIndex - 1)*(this.$middleHeight + this.spacing))
            
        } else {
            return 0
        }
		
	}
	$getOriginX(){
        if(this.orientation === ListView.Horizontal){
            if(this.$items[0]) return this.$items[0].x

            let visibleContentWidth = 0
            let lastIndex = 0
            for(let child of this.contentItem.children){
                if(child.dom){
                    if(child.index > lastIndex) lastIndex = child.index
                    if(child.x + child.width > visibleContentWidth){
                        visibleContentWidth = child.x + child.width          
                    }
                }
            }
            return this.contentWidth - (visibleContentWidth + (this.$items.length - lastIndex - 1)*(this.$middleWidth + this.spacing))
            
        } else {
            return 0
        }
		
	}

    $contentXChanged(){
        if(this.orientation === ListView.Horizontal){
            if(this.$items[0] && this.$p.contentX.val < this.$items[0].x){
                this.$p.contentX.val = this.$items[0].x
            }
            if(this.$items[this.$items.length-1] && this.$p.contentX.val > this.$items[this.$items.length-1].x + this.$items[this.$items.length-1].width - this.width){
                this.$p.contentX.val = this.$items[this.$items.length-1].x + this.$items[this.$items.length-1].width - this.width
            }
            super.$contentXChanged()
            this.$updateView()
            
        }
    }
    $contentYChanged(){
        if(this.orientation === ListView.Vertical){
            if(this.$items[0] && this.$p.contentY.val < this.$items[0].y){
                this.$p.contentY.val = this.$items[0].y
            }
            if(this.$items[this.$items.length-1] && this.$p.contentY.val > this.$items[this.$items.length-1].y + this.$items[this.$items.length-1].height - this.height){
                this.$p.contentY.val = this.$items[this.$items.length-1].y + this.$items[this.$items.length-1].height - this.height
            }
            super.$contentYChanged()
            this.$updateView()
            
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

    // $updateGeometry(){
	// 	if(this.contentItem){
    //         // this.$sP('contentHeight', ()=>{ 
    //             let childLength = typeof this.model === 'number' ? this.model : this.contentItem.children.length
    //             let contentHeight = 0
                
    //             for(let i = 0; i < childLength; i++){
    //                 let childHeight = 100
    //                 if(i < this.contentItem.children.length){
    //                     childHeight = this.contentItem.children[i].height
    //                 }
    //                 contentHeight += childHeight
    //             }
    //             contentHeight += (childLength-1)*this.spacing
                
    //             this.contentHeight = contentHeight
    //         // })
    //     }
        
	// 	if(this.contentItem){
    //         // this.$sP('contentWidth', ()=>{ 
    //             let childLength = typeof this.model === 'number' ? this.model : this.contentItem.children.length
    //             let contentWidth = 0
                
    //             for(let i = 0; i < childLength; i++){
    //                 let childWidth = 100
    //                 if(i < this.contentItem.children.length){
    //                     childWidth = this.contentItem.children[i].width
    //                 }
    //                 contentWidth += childWidth
    //             }
    //             contentWidth += (childLength-1)*this.spacing
                
    //             this.contentWidth = contentWidth
    //         // })
    //     }
        
    // }
    $childChanged(){
  
    }
    $getMiddleWH(){
        let middleWidth = 0
        let middleHeight = 0
        if(this.contentItem.children.length){
            for(let i = 0; i < this.contentItem.children.length; i++){
                middleWidth += this.contentItem.children[i].width
                middleHeight += this.contentItem.children[i].height
            }
            middleWidth = middleWidth / this.contentItem.children.length
            middleHeight = middleHeight / this.contentItem.children.length
        }
        this.$middleWidth = middleWidth
        this.$middleHeight = middleHeight
    }
    $updateGeometry(){
        if(this.contentItem){
            // this.$getMiddleWH()
            if(this.orientation === ListView.Vertical){
                let visibleContentHeight = 0
                let lastIndex = 0
                let firstIndex = this.contentItem.children.length ? this.contentItem.children[0].index : 0
                let minY = this.contentItem.children.length ? this.contentItem.children[0].y : 0
                for(let child of this.contentItem.children){
                    if(child.dom){
                        if(child.index > lastIndex) lastIndex = child.index
                        // if(child.y + child.height > visibleContentHeight){
                        //     visibleContentHeight = child.y + child.height
                        // }
                        
                        visibleContentHeight += child.height
                        
                        if(child.y < minY) minY = child.y
                        if(child.index < firstIndex) firstIndex = child.index
                    }
                }
                let middleHeight = visibleContentHeight / this.contentItem.children.length
                // let contentHeight = visibleContentHeight - minY + (Math.round(this.$middleHeight) + this.spacing)*(this.$items.length-this.contentItem.children.length)
                // this.contentHeight = contentHeight
                // this.originY = -(contentHeight - (visibleContentHeight + (this.$items.length - lastIndex - 1)*(Math.round(this.$middleHeight) + this.spacing)))

                this.$middleHeight = middleHeight
                let contentHeight = visibleContentHeight + Math.round(middleHeight)*(this.$items.length-this.contentItem.children.length) + this.spacing * (this.$items.length-1)
                this.contentHeight = contentHeight
                this.originY = (minY - firstIndex*(Math.round(middleHeight+this.spacing)))
            }
            
            if(this.orientation === ListView.Horizontal){
                let visibleContentWidth = 0
                let lastIndex = 0
                let firstIndex = this.contentItem.children.length ? this.contentItem.children[0].index : 0
                let minX = this.contentItem.children.length ? this.contentItem.children[0].x : 0
                for(let child of this.contentItem.children){
                    if(child.dom){
                        if(child.index > lastIndex) lastIndex = child.index
                        visibleContentWidth += child.width
                        if(child.x < minX) minX = child.x
                        if(child.index < firstIndex) firstIndex = child.index
                    }
                }
                let middleWidth = visibleContentWidth / this.contentItem.children.length
                // let contentWidth = visibleContentWidth - minX + (Math.round(this.$middleWidth) + this.spacing)*(this.$items.length-this.contentItem.children.length)
                // this.contentWidth = contentWidth
                // this.originX = -(contentWidth - (visibleContentWidth + (this.$items.length - lastIndex - 1)*(Math.round(this.$middleWidth) + this.spacing)))
                this.$middleWidth = middleWidth
                let contentWidth = visibleContentWidth + Math.round(middleWidth)*(this.$items.length-this.contentItem.children.length) + this.spacing * (this.$items.length-1)
                this.contentWidth = contentWidth
                this.originX = (minX - firstIndex*(Math.round(middleWidth)))
            }

            //-this.$getOriginX()
            //-this.$getOriginY()
        }
		
    }

    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        if(!this.model){
            while(this.contentItem.children.length){
                let child = this.contentItem.children.pop()
                if(child) child.$destroy()
            }
            this.contentItem.children = []
            this.count = 0
        }

        for(let item of this.$items){
            if(item) item.$destroy()
        }
        this.$items = []
        if(typeof this.model === 'number'){
            for(let i = 0; i < this.model; i++){ 
                this.$items.push(null)
            }
            
        } else if (typeof this.model === 'object'){
            if(this.model.$deps) {
                this.model.$deps[this.UID] = this
                for(let i = 0; i < this.model.data.length; i++){
                    this.$items.push(null)
                }
            } else {
                for(let i = 0; i < this.model.length; i++){
                    this.$items.push(null)
                }
            }
        }
        this.$updateGeometry()
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView(true)
    }
    $spacingChanged(){

    }
    $currentIndexChanged(){
        //this.currentItem = this.currentIndex > -1 && this.currentIndex < this.contentItem.children.length ? this.contentItem.children[this.currentIndex] : undefined
    }
    itemAtIndex(index){
        return index >= 0 && index < this.contentItem.children.length ? this.contentItem.children[index] : undefined
    }
    positionViewAtBeginning(){
        this.contentX = 0
        this.contentY = 0
    }
    positionViewAtEnd(){
        if(this.contentWidth > this.width){
            this.contentX = Math.min(this.contentItem.children[this.contentItem.children.length-1].x + this.contentItem.children[this.contentItem.children.length-1].width, this.contentWidth - this.width)
        }
        if(this.contentHeight > this.height){
            this.contentY = Math.min(this.contentItem.children[this.contentItem.children.length-1].y + this.contentItem.children[this.contentItem.children.length-1].height, this.contentHeight - this.height)
        }
        
    }
    positionViewAtIndex(index, mode){
        let pos = 'start'
        switch(mode){
            case ListView.Beginning: pos = 'start'; break;
            case ListView.Center: pos = 'center'; break;
            case ListView.End: pos = 'end'; break;
        }
        if(pos === 'start') {
            if(this.contentWidth > this.width){
                this.contentX = Math.min(this.contentItem.children[index].x, this.contentWidth - this.width)
            }
            if(this.contentHeight > this.height){
                this.contentY = Math.min(this.contentItem.children[index].y, this.contentHeight - this.height)
            }
            // this.contentX = this.contentItem.children[index].x
            // this.contentY = this.contentItem.children[index].y
        } else if(pos === 'center') {
            if(this.contentWidth > this.width){
                this.contentX = Math.min(this.contentItem.children[index].x - (this.width - this.contentItem.children[index].width)/2, this.contentWidth - this.width)
            }
            if(this.contentHeight > this.height){
                this.contentY = Math.min(this.contentItem.children[index].y - (this.height - this.contentItem.children[index].height)/2, this.contentHeight - this.height)
            }
            // this.contentX = this.contentItem.children[index].x - (this.width - this.contentItem.children[index].width)/2
            // this.contentY = this.contentItem.children[index].y - (this.height - this.contentItem.children[index].height)/2
        } else if(pos === 'end') {
            if(this.contentWidth > this.width){
                this.contentX = Math.min(this.contentItem.children[index].x - (this.width - this.contentItem.children[index].width), this.contentWidth - this.width)
            }
            if(this.contentHeight > this.height){
                this.contentY = Math.min(this.contentItem.children[index].y - (this.height - this.contentItem.children[index].height), this.contentHeight - this.height)
            }
            // this.contentX = this.contentItem.children[index].x - (this.width - this.contentItem.children[index].width)
            // this.contentY = this.contentItem.children[index].y - (this.height - this.contentItem.children[index].height)
        }
    }

    // $insert(index){
    //     if(this.model && this.delegate){
    //         let childRecursive = (obj, indx)=>{
    //             if(obj.$qmlClassName !== 'ListElement'){
    //                 obj.$cP('index', indx)
    //                 obj.index = indx
    //             }

    //             for(let child of obj.children){
    //                 if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
    //                 childRecursive(child, indx)
    //             }
    //         }
    //         let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
    //         obj.widthChanged.connect(this.$updateGeometry.bind(this))
    //         obj.heightChanged.connect(this.$updateGeometry.bind(this))
    //         this.contentItem.children.pop()
    //         this.contentItem.children.splice(index, 0, obj)

    //         for(let i = 0; i < this.contentItem.children.length; i++){
    //             childRecursive(this.contentItem.children[i], i)
    //         }
            
            
    //         this.count = this.contentItem.children.length
    //         // this.$anchorsChild(index)
    //         try {
    //             obj.$uP()
    //         } catch (error) {
    //             console.error(error)
    //         }
    //         // this.$updateGeometry()
    //     }
    // }

    // $append(wait = false){
    //     let index = this.contentItem.children.length
    //     if(this.model && this.delegate){
    //         let childRecursive = (obj)=>{
    //             if(obj.$qmlClassName !== 'ListElement'){
    //                 obj.$cP('index', index)
    //                 obj.index = index
    //             }

    //             for(let child of obj.children){
    //                 if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
    //                 childRecursive(child)
    //             }
    //         }
    //         let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
    //         obj.widthChanged.connect(this.$updateGeometry.bind(this))
    //         obj.heightChanged.connect(this.$updateGeometry.bind(this))
    //         childRecursive(obj)
    //         this.count = this.contentItem.children.length
    //         // this.$anchorsChild(index)

    //         if(wait){
    //             this.$childrenForUpdate.push(obj)
    //         } else {
    //             // obj.$uP()
    //             try {
    //                 obj.$uP()
    //             } catch (error) {
    //                 console.error(error)
    //             }
    //         }
    //     }
    // }
    // $remove(index, count){
    //     let removed = this.contentItem.children.splice(index, count)
    //     for(let rem of removed){
    //         rem.$destroy()
    //     }
    //     let childRecursive = (obj, index)=>{
    //         obj.index = index
    //         for(let child of obj.children){
    //             if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
    //             childRecursive(child, index)
    //         }
    //     }
    //     for(let i = 0; i < this.contentItem.children.length; i++){
    //         childRecursive(this.contentItem.children[i], i)
    //     }
    //     this.count = this.contentItem.children.length
    //     // this.$updateGeometry()
    // }

    $recursiveIndex(obj, index){
        if(obj.$qmlClassName !== 'ListElement'){
            obj.$cP('index', index)
            obj.index = index
        }

        for(let child of obj.children){
            if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
            this.$recursiveIndex(child, index)
        }
    }
    $createElement(index){
        let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
        obj.widthChanged.connect(this.$updateGeometry.bind(this))
        obj.heightChanged.connect(this.$updateGeometry.bind(this))
        
        // obj.$orientation = this.orientation
        this.$recursiveIndex(obj, index)
        // obj.$uP()
        return obj
    }

    $getCurrentIndex(){
        let i = 0
        while(i < this.contentItem.children.length){
            if(this.orientation === ListView.Horizontal){
                if(this.contentItem.children[i].x - this.spacing <= this.contentX  && this.contentItem.children[i].x + this.contentItem.children[i].width > this.contentX){
                    return this.contentItem.children[i].index
                }
            } else {
                if(this.contentItem.children[i].y - this.spacing <= this.contentY  && this.contentItem.children[i].y + this.contentItem.children[i].height > this.contentY){
                    return this.contentItem.children[i].index
                }
            }
            i++
        }
        return undefined

    }
    $remove(index, count){
        let removed = this.$items.splice(index, count)
        while(removed.length){
            removed.pop().$destroy()
        }
        this.$updateView()
    }
    $insert(index){
        this.$items.splice(index, 0, null)
        this.$updateView()
    }
    $append(){
        this.$items.push(null)
        this.$updateView()
    }
    $updateView(){
        try {
            this.$enabledScroll = false
            if(this.model && this.delegate){
                
                if(this.$items.length <= 0) return

                let leftIndex = this.contentItem.children.length ? this.contentItem.children[0].index : 0
                let rightIndex = 0
                let maxX = this.contentItem.children.length ? this.contentItem.children[0].x + this.contentItem.children[0].width : 0
                let maxY = this.contentItem.children.length ? this.contentItem.children[0].y + this.contentItem.children[0].height : 0
                let minX = this.contentItem.children.length ? this.contentItem.children[0].x + this.contentItem.children[0].width : 0
                let minY = this.contentItem.children.length ? this.contentItem.children[0].y + this.contentItem.children[0].height : 0
                let visibleCount = this.contentItem.children.length
                let middleWidth = 0
                let middleHeight = 0
                let tempChild = this.contentItem.children.slice()

                while(tempChild.length){
                    let child = tempChild.pop()
                    middleWidth += child.width
                    middleHeight += child.height

                    if(child.index > rightIndex){
                        rightIndex = child.index
                    }
                    if(child.index < leftIndex){
                        leftIndex = child.index
                    }
                    if(child.x + child.width > maxX){
                        maxX = child.x + child.width
                    }
                    if(child.y + child.height > maxY){
                        maxY = child.y + child.height
                    }
                    if(child.x < minX){
                        minX = child.x
                    }
                    if(child.y < minY){
                        minY = child.y
                    }
                    if(this.orientation === ListView.Horizontal){
                            
                        if(child.x + child.width < this.contentX-this.cacheBuffer || child.x > this.contentX + this.width + this.cacheBuffer) {
                            this.$items[child.index] = null
                            child.$destroy()
                            this.$updateGeometry()
                        }
                    } else {
                        
                        if(child.y + child.height < this.contentY-this.cacheBuffer || child.y > this.contentY + this.height + this.cacheBuffer) {
                            this.$items[child.index] = null
                            child.$destroy()
                            this.$updateGeometry()
                        }
                    }
                }
                middleWidth = visibleCount ? middleWidth / visibleCount : 0
                middleHeight = visibleCount ? middleHeight / visibleCount : 0
    
                let currentIndex = this.$getCurrentIndex()

                if(this.contentItem.children.length === 0){
                    if(this.orientation === ListView.Horizontal){
                        if(this.contentX <= minX){
                            currentIndex = leftIndex + Math.ceil((this.contentX - minX + this.spacing)/(Math.round(middleWidth + this.spacing)))
                        } else {
                            currentIndex = rightIndex + Math.ceil((this.contentX - maxX - this.spacing)/(Math.round(middleWidth + this.spacing)))
                        }
                        if(currentIndex < 0 || currentIndex >= this.$items.length) return
                        if(middleWidth === 0) currentIndex = 0
                        // currentIndex = (this.contentX <= minX ? leftIndex : rightIndex) + (Math.round(middleWidth)*(this.$items.length - visibleCount))
    
                        if(!this.$items[currentIndex]){
                            this.$items[currentIndex] = this.$createElement(currentIndex)
                            if(this.contentY <= minX){
                                this.$items[currentIndex].x = minX + (Math.round(middleWidth + this.spacing)*(currentIndex-leftIndex))
                            } else {
                                this.$items[currentIndex].x = maxX + (Math.round(middleWidth + this.spacing)*(currentIndex-rightIndex-1))
                            }
                            // this.$items[currentIndex].x = middleWidth*currentIndex + this.spacing*(currentIndex)
                            this.$items[currentIndex].y = 0
    
                            this.$items[currentIndex].$uP()
                            this.$updateGeometry()
                        }
                    } else {
                        if(this.contentY <= minY){
                            currentIndex = leftIndex + Math.ceil((this.contentY - minY + this.spacing)/(Math.round(middleHeight + this.spacing)))
                        } else {
                            currentIndex = rightIndex + Math.ceil((this.contentY - maxY - this.spacing)/(Math.round(middleHeight + this.spacing)))
                        }
                        if(currentIndex < 0 || currentIndex >= this.$items.length) return
                        if(middleHeight === 0) currentIndex = 0
                        // currentIndex = (this.contentY <= minY ? leftIndex : rightIndex) + (Math.round(middleHeight)*(this.$items.length - visibleCount))
    
                        if(!this.$items[currentIndex]){
                            this.$items[currentIndex] = this.$createElement(currentIndex)
                            this.$items[currentIndex].x = 0
                            if(this.contentY <= minY){
                                this.$items[currentIndex].y = minY + (Math.round(middleHeight + this.spacing)*(currentIndex-leftIndex))
                            } else {
                                this.$items[currentIndex].y = maxY + (Math.round(middleHeight + this.spacing)*(currentIndex-rightIndex-1))
                            }
                            // this.$items[currentIndex].y = middleHeight*currentIndex + this.spacing*(currentIndex)
    
                            this.$items[currentIndex].$uP()
                            this.$updateGeometry()
                        }
                    }
                    leftIndex = currentIndex
                    rightIndex = currentIndex
                }
                
                
                // let leftIndex = this.$items.length
                // let rightIndex = 0

                // for(let i = 0; i < this.contentItem.children.length; i++){
                //     if(this.contentItem.children[i].index < leftIndex) leftIndex = this.contentItem.children[i].index
                //     if(this.contentItem.children[i].index > rightIndex) rightIndex = this.contentItem.children[i].index
                // }

                
                // console.log(`DEBUG:::leftIndex=${leftIndex}, rightIndex=${rightIndex}`)
                
                if(this.orientation === ListView.Horizontal){
                    while(leftIndex > 0 && this.$items[leftIndex]){
                        if(this.$items[leftIndex].x - this.spacing > this.contentX - this.cacheBuffer){
                            let currIndex = leftIndex-1
                            let prevIndex = leftIndex
                            if(!this.$items[currIndex]){
                                this.$items[currIndex] = this.$createElement(currIndex)
                                this.$items[currIndex].y = 0
                                this.$items[currIndex].$sP('x', ()=>{
                                    if(this.$items[prevIndex]){
                                        return this.$items[prevIndex].x - this.$items[currIndex].width - this.spacing
                                    } else {
                                        return this.$items[currIndex].$p.x.val
                                    }     
                                })
                                this.$items[currIndex].$uP()
                                this.$updateGeometry()
                                // console.log(`DEBUG:::createElement(${currIndex})`)
                            }
                            
                        }
                        leftIndex -= 1
                    }
                } else {
                    while(leftIndex > 0 && this.$items[leftIndex]){
                        if(this.$items[leftIndex].y - this.spacing > this.contentY - this.cacheBuffer){
                            let currIndex = leftIndex-1
                            let prevIndex = leftIndex
                            if(!this.$items[currIndex]){
                                this.$items[currIndex] = this.$createElement(currIndex)
                                this.$items[currIndex].x = 0
                                this.$items[currIndex].$sP('y', ()=>{
                                    if(this.$items[prevIndex]){
                                        return this.$items[prevIndex].y - this.$items[currIndex].height - this.spacing
                                    } else {
                                        return this.$items[currIndex].$p.y.val
                                    }     
                                })
                                this.$items[currIndex].$uP()
                                this.$updateGeometry()
                                // console.log(`DEBUG:::createElement(${currIndex})`)
                            }
                            
                        }
                        leftIndex -= 1
                    }
                }

                if(this.orientation === ListView.Horizontal){
                    while(rightIndex < this.$items.length-1 && this.$items[rightIndex]){
                        if(this.$items[rightIndex].x + this.$items[rightIndex].width + this.spacing < this.contentX + this.width + this.cacheBuffer){
                            let currIndex = rightIndex+1
                            let prevIndex = rightIndex
                            if(!this.$items[currIndex]){
                                this.$items[currIndex] = this.$createElement(currIndex)
                                this.$items[currIndex].y = 0
                                this.$items[currIndex].$sP('x', ()=>{
                                    if(this.$items[prevIndex]){
                                        return this.$items[prevIndex].x + this.$items[prevIndex].width + this.spacing
                                    } else {
                                        return this.$items[currIndex].$p.x.val
                                    }     
                                })
                                this.$items[currIndex].$uP()
                                this.$updateGeometry()
                                // console.log(`DEBUG:::createElement(${currIndex})`)
                            }
                        }
                        rightIndex += 1
                    }
                } else {
                    while(rightIndex < this.$items.length-1 && this.$items[rightIndex]){
                        if(this.$items[rightIndex].y + this.$items[rightIndex].height + this.spacing < this.contentY + this.height + this.cacheBuffer){
                            let currIndex = rightIndex+1
                            let prevIndex = rightIndex
                            if(!this.$items[currIndex]){
                                this.$items[currIndex] = this.$createElement(currIndex)
                                this.$items[currIndex].x = 0
                                this.$items[currIndex].$sP('y', ()=>{
                                    if(this.$items[prevIndex]){
                                        return this.$items[prevIndex].y + this.$items[prevIndex].height + this.spacing
                                    } else {
                                        return this.$items[currIndex].$p.y.val
                                    }     
                                })
                                this.$items[currIndex].$uP()
                                this.$updateGeometry()
                                // console.log(`DEBUG:::createElement(${currIndex})`)
                            }
                        }
                        rightIndex += 1
                    }
                }
                
                
                this.count = this.$items.length
                
            }
        } catch (error) {
            
        } finally {
            // this.$oldContentX = this.contentX
            // this.$oldContentY = this.contentY
            this.$enabledScroll = true
        }
        
    
    }

    // $countChanged(){
    //     this.$updateChildren()
    // }
    
    // $updateChildren(){
    //     for(let i = 0; i < this.contentItem.children.length; i++){
    //         this.$anchorsChild(i)
    //     }
    //     this.contentItem.$uP()
    //     this.$updateGeometry()
    // }
    // $anchorsChild(index){
    //     let child = this.$items[index]
    //     let prevChild = this.$items[index-1]
    //     let nextChild = this.$items[index+1]
    //     if(index === 0){
    //         child.$sP('anchors.left', ()=>{ return this.contentItem.left })
    //         child.$sP('anchors.top', ()=>{ return this.contentItem.top })
    //     } else {
    //         if(this.orientation === ListView.Horizontal){
    //             // child.$sP('x', ()=>{
    //             //     if(prevChild){
    //             //         return prevChild.x + prevChild.width + this.spacing
    //             //     } else if(nextChild){
    //             //         return nextChild.x - child.width - this.spacing
    //             //     } else {
    //             //         this.$getMiddleWH()
    //             //         return this.$middleWidth * index + this.spacing * (index-1)
    //             //     }
    //             //     //return this.$items[index-1] ? this.$items[index-1].x + this.$items[index-1].width : (this.$items[0].width * index + this.spacing * (index-1)) 
    //             // })
    //             child.$sP('anchors.left', ()=>{ return this.$items[index-1] ? this.$items[index-1].right : (this.$items[0].width * index + this.spacing * (index-1)) })
    //             child.$sP('anchors.leftMargin', ()=>{ return this.spacing })
    //         } else {
    //             // child.$sP('y', ()=>{
    //             //     if(prevChild){
    //             //         return prevChild.y + prevChild.height + this.spacing
    //             //     } else if(nextChild){
    //             //         return nextChild.y - child.height - this.spacing
    //             //     } else {
    //             //         this.$getMiddleWH()
    //             //         return this.$middleHeight * index + this.spacing * (index-1)
    //             //     }
    //             //     // return this.$items[index-1] ? this.$items[index-1].y + this.$items[index-1].height : (this.$items[0].height * index + this.spacing * (index-1)) 
    //             // })
    //             child.$sP('anchors.top', ()=>{ return this.$items[index-1] ? this.$items[index-1].bottom : (this.$items[0].height * index + this.spacing * (index-1))})
    //             child.$sP('anchors.topMargin', ()=>{ return this.spacing })
    //         }
    //     }
        
        
    // }

    $scroll(deltaX, deltaY){
        if(!this.$enabledScroll) return

		if(this.interactive && this.enabled){
			if(this.flickableDirection !== Flickable.VerticalFlick && this.contentWidth > 0 && this.contentWidth > this.width){
				if(deltaX > 0)
				if(!this.$items[this.$items.length-1] || this.contentX + deltaX < this.$items[this.$items.length-1].x + this.$items[this.$items.length-1].width - this.width){
					this.contentX += deltaX
				} else {
					this.contentX = this.$items[this.$items.length-1].x+this.$items[this.$items.length-1].width-this.width//this.contentHeight - this.height
				}
				if(deltaX < 0)
				if(!this.$items[0] || this.contentX + deltaX > this.$items[0].x){
					this.contentX += deltaX
				} else {
					this.contentX = this.$items[0].x
				}
			} else {
				// this.$parentScroll(deltaX, 0)
			}
			
			if(this.flickableDirection !== Flickable.HorizontalFlick && this.contentHeight > 0 && this.contentHeight > this.height){
				if(deltaY > 0)
				if(!this.$items[this.$items.length-1] || this.contentY + deltaY < this.$items[this.$items.length-1].y + this.$items[this.$items.length-1].height - this.height){
					this.contentY += deltaY
				} else {
					this.contentY = this.$items[this.$items.length-1].y+this.$items[this.$items.length-1].height-this.height//this.contentHeight - this.height
				}
				if(deltaY < 0)
				if(!this.$items[0] || this.contentY + deltaY > this.$items[0].y){
					this.contentY += deltaY
				} else {
					this.contentY = this.$items[0].y
				}
			} else {
				// this.$parentScroll(0, deltaY)
			}
		}

	}

    $mousedown(e, state) {
        this.$snapX = this.contentX
        this.$snapY = this.contentY
        for(let i = 0; i < this.contentItem.children.length; i++){
            let item = this.contentItem.children[i]
            if(this.orientation === ListView.Horizontal){
                if(item.x === this.$snapX) {
                    this.$snapIndex = i
                }
            } else {
                if(item.Y === this.$snapY) {
                    this.$snapIndex = i
                }
            }
            
        }
        
        super.$mousedown(e, state)
	}
    $mouseup(e, state) {
        if(this.snapMode !== ListView.NoSnap){
            e.preventDefault()
            if(this.enabled && this.interactive){
                if(this.orientation === ListView.Horizontal){
                    if(this.$snapX > this.contentX){
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.contentItem.dom.style.transitionProperty = 'left'
                        this.positionViewAtIndex(this.$snapIndex-1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    } else if(this.$snapX < this.contentX){
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.contentItem.dom.style.transitionProperty = 'left'
                        this.positionViewAtIndex(this.$snapIndex+1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    }
                } else {
                    if(this.$snapY > this.contentY){
                        this.contentItem.dom.style.transitionProperty = 'top'
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.positionViewAtIndex(this.$snapIndex-1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    } else if(this.$snapY < this.contentY){
                        this.contentItem.dom.style.transitionProperty = 'top'
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.positionViewAtIndex(this.$snapIndex+1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    }
                }
                state.release()
                Core.velocityX = 0
		        Core.velocityY = 0
                this.$pressed = false
            }
        } else {
            super.$mouseup(e, state)
        }
		

	}

    $touchstart(e, state) {
        this.$snapX = this.contentX
        this.$snapY = this.contentY
        for(let i = 0; i < this.contentItem.children.length; i++){
            let item = this.contentItem.children[i]
            if(this.orientation === ListView.Horizontal){
                if(item.x === this.$snapX) {
                    this.$snapIndex = i
                }
            } else {
                if(item.Y === this.$snapY) {
                    this.$snapIndex = i
                }
            }
            
        }
        
        super.$touchstart(e, state)
	}
    $touchend(e, state) {
        if(this.snapMode !== ListView.NoSnap){
            e.preventDefault()
            if(this.enabled && this.interactive){
                if(this.orientation === ListView.Horizontal){
                    if(this.$snapX > this.contentX){
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.contentItem.dom.style.transitionProperty = 'left'
                        this.positionViewAtIndex(this.$snapIndex-1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    } else if(this.$snapX < this.contentX){
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.contentItem.dom.style.transitionProperty = 'left'
                        this.positionViewAtIndex(this.$snapIndex+1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    }
                } else {
                    if(this.$snapY > this.contentY){
                        this.contentItem.dom.style.transitionProperty = 'top'
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.positionViewAtIndex(this.$snapIndex-1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    } else if(this.$snapY < this.contentY){
                        this.contentItem.dom.style.transitionProperty = 'top'
                        this.contentItem.dom.style.transitionDuration = '600ms'
                        this.positionViewAtIndex(this.$snapIndex+1)
                        clearTimeout(this.$snapTimer)
                        this.$snapTimer = setTimeout(()=>{
                            this.contentItem.dom.style.transitionProperty = 'unset'
                            this.contentItem.dom.style.transitionDuration = 'unset'
                        }, 600)
                    }
                }
                state.release()
                Core.velocityX = 0
		        Core.velocityY = 0
                this.$pressed = false
            }
        } else {
            super.$touchend(e, state)
        }
		

	}
    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        super.$destroy()
    }

}

QML.ListView = ListView