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

    $contentXChanged(){
        if(this.orientation === ListView.Horizontal){
            super.$contentXChanged()
            this.$updateView()
        }
    }
    $contentYChanged(){
        if(this.orientation === ListView.Vertical){
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
    $getMiddleWH(){
        let middleWidth = 0
        let middleHeight = 0
        if(this.contentItem.children.length){
            for(let i = 0; i < this.contentItem.children.length; i++){
                middleWidth += this.contentItem.children[i].width
                middleHeight += this.contentItem.children[i].height
            }
            middleWidth = Math.trunc(middleWidth / this.contentItem.children.length)
            middleHeight = Math.trunc(middleHeight / this.contentItem.children.length)
        }
        this.$middleWidth = middleWidth
        this.$middleHeight = middleHeight
    }
    $updateGeometry(){
        

        if(this.contentItem){
            this.$getMiddleWH()
            if(this.orientation === ListView.Vertical){
                let contentHeightFunc = ()=>{
                    // let top = 0
                    // let bottom = 0
                    // if(this.contentItem.children.length)
                    // for(let child of this.contentItem.children) {
                    // 	let childTop = child.y
                    // 	let childBottom = childTop + child.height
                    // 	if(childTop < top) top = childTop
                    // 	if(childBottom > bottom) bottom = childBottom
                    // }
                    // return bottom - top
                    if(this.$items[this.$items.length-1]){
                        return this.$items[this.$items.length-1].y + this.$items[this.$items.length-1].height
                    } else {
                        return this.$middleHeight * this.$items.length + this.spacing * (this.$items.length-1)
                    }
                    
                }
                this.contentHeight = contentHeightFunc()
                // this.$sP('contentHeight', contentHeightFunc)
                
            }
            
            if(this.orientation === ListView.Horizontal){
                let contentWidthFunc = ()=>{
                    // let left = 0
                    // let right = 0
                    // if(this.contentItem.children.length)
                    // for(let child of this.contentItem.children) {
                    // 	let childLeft = child.x
                    // 	let childRight = childLeft + child.width
                    // 	if(childLeft < left) left = childLeft
                    // 	if(childRight > right) right = childRight
                    // }
                    // return right - left
                    if(this.$items[this.$items.length-1]){
                        return this.$items[this.$items.length-1].x + this.$items[this.$items.length-1].width
                    } else {
                        return this.$middleWidth * this.$items.length + this.spacing * (this.$items.length-1)
                    }
                }
                this.contentWidth = contentWidthFunc()
                // this.$sP('contentWidth', contentWidthFunc)
                
            }
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
        if(this.model && this.delegate){
            let availableArea = {
                left: this.contentX - this.cacheBuffer,
                right: this.contentX + this.width + this.cacheBuffer,
                top: this.contentY - this.cacheBuffer,
                bottom: this.contentY + this.height + this.cacheBuffer,
            }
            let count = 0

            if(typeof this.model === 'number'){
                count = this.model
                
            } else if (typeof this.model === 'object'){
                if(this.model.$deps) {
                    count = this.model.data.length
                    this.model.$deps[this.UID] = this
                } else {
                    count = this.model.length
                }
                
            } else {

            }
            if(count <= 0) return

            if(!this.$items[0]){
                this.$items[0] = this.$createElement(0)
                this.$items[0].x = 0
                this.$items[0].y = 0
                // this.$items[0].$orientation = this.orientation
                this.$items[0].$uP()
                this.$updateGeometry()
            } else {
                this.$items[0].x = 0
                this.$items[0].y = 0
            }

            let currentIndex = this.$getCurrentIndex()
            // console.log('currentIndex', currentIndex)

            for(let i = currentIndex-1; i > 0; i--){
                if(this.$items[i]){

                    if(this.orientation === ListView.Horizontal){
                        this.$items[i].y = 0
                        this.$items[i].$sP('x', ()=>{
                            return this.$items[i+1].x - this.spacing - this.$items[i].width
                        })
                        this.$items[i].$uP()
                        this.$updateGeometry()
                    } else {
                        this.$items[i].x = 0
                        this.$items[i].$sP('y', ()=>{
                            return this.$items[i+1].y - this.spacing - this.$items[i].height
                        })
                        this.$items[i].$uP()
                        this.$updateGeometry()
                    }
                    
                    if(this.orientation === ListView.Horizontal){
                        if(this.$items[i].x + this.$items[i].width + this.spacing < availableArea.left || this.$items[i].x - this.spacing > availableArea.right){
                            this.$items[i].$destroy()
                            this.$items[i] = null
                        }
                    } else {
                        if(this.$items[i].y + this.$items[i].height + this.spacing < availableArea.top || this.$items[i].y - this.spacing > availableArea.bottom){
                            this.$items[i].$destroy()
                            this.$items[i] = null
                        }
                    }
                } else if(this.$items[i+1]){
                    if(this.orientation === ListView.Horizontal){
                        if(this.$items[i+1].x > availableArea.left){
                            this.$items[i] = this.$createElement(i)
                            this.$items[i].y = 0
                            this.$items[i].$sP('x', ()=>{
                                return this.$items[i+1].x - this.spacing - this.$items[i].width
                            })
                            this.$items[i].$uP()
                            this.$updateGeometry()
                        }
                    } else {
                        if(this.$items[i+1].y > availableArea.top){
                            this.$items[i] = this.$createElement(i)
                            this.$items[i].x = 0
                            this.$items[i].$sP('y', ()=>{
                                return this.$items[i+1].y - this.spacing - this.$items[i].height
                            })
                            this.$items[i].$uP()
                            this.$updateGeometry()
                        }
                    }
                    
                } else {
                    break
                }
            }
            for(let i = currentIndex+1; i < count; i++){
                if(this.$items[i]){
                    if(this.orientation === ListView.Horizontal){
                        this.$items[i].y = 0
                        this.$items[i].$sP('x', ()=>{
                            return this.$items[i-1].x + this.$items[i-1].width + this.spacing
                        })
                        this.$items[i].$uP()
                        this.$updateGeometry()
                    } else {
                        this.$items[i].x = 0
                        this.$items[i].$sP('y', ()=>{
                            return this.$items[i-1].y + this.$items[i-1].height + this.spacing
                        })
                        this.$items[i].$uP()
                        this.$updateGeometry()
                    }
                    if(this.orientation === ListView.Horizontal){
                        if(this.$items[i].x + this.$items[i].width < availableArea.left || this.$items[i].x > availableArea.right){
                            this.$items[i].$destroy()
                            this.$items[i] = null
                        }
                    } else {
                        if(this.$items[i].y + this.$items[i].height < availableArea.top || this.$items[i].y > availableArea.bottom){
                            this.$items[i].$destroy()
                            this.$items[i] = null
                        }
                    }
                } else if(this.$items[i-1]){
                    if(this.orientation === ListView.Horizontal){
                        if(this.$items[i-1].x + this.$items[i-1].width < availableArea.right){
                            this.$items[i] = this.$createElement(i)
                            this.$items[i].y = 0
                            this.$items[i].$sP('x', ()=>{
                                return this.$items[i-1].x + this.$items[i-1].width + this.spacing
                            })
                            this.$items[i].$uP()
                            this.$updateGeometry()
                        }
                    } else {
                        if(this.$items[i-1].y + this.$items[i-1].height < availableArea.bottom){
                            this.$items[i] = this.$createElement(i)
                            this.$items[i].x = 0
                            this.$items[i].$sP('y', ()=>{
                                return this.$items[i-1].y + this.$items[i-1].height + this.spacing
                            })
                            this.$items[i].$uP()
                            this.$updateGeometry()
                        }
                    }
                    
                } else {
                    break
                }
            }
            this.count = count
            
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