import {Flickable} from './Flickable'

export class GridView extends Flickable {
    static Beginning = 0
    static Center = 1
    static End = 2

    static LeftToRight = 0
    static RightToLeft = 1
    static TopToBottom = 2
    static BottomToTop = 3

    $useModel = true
    $childrenForUpdate = []

    constructor(args) {
        super(args)

        this.$cP('model', undefined).connect(this.$modelChanged.bind(this))
        this.$cP('delegate', undefined).connect(this.$delegateChanged.bind(this))
        this.$cP('count', 0)
        this.$cP('cellWidth', 100).connect(this.$cellChanged.bind(this))
        this.$cP('cellHeight', 100).connect(this.$cellChanged.bind(this))
        this.$cP('verticalLayoutDirection', GridView.TopToBottom).connect(this.$directionChanged.bind(this))
        this.$cP('layoutDirection', GridView.LeftToRight).connect(this.$directionChanged.bind(this))
        // this.$cP('spacing', 'real', 0).connect(this.$spacingChanged.bind(this))
        this.$cP('currentIndex', -1).connect(this.$currentIndexChanged.bind(this))
        this.$cP('currentItem', undefined)

        this.$updateTimer = null
    }
    $domCreate(){
        super.$domCreate()

        // this.contentItem.dom.style.display = 'flex'
        // this.contentItem.dom.style.flexWrap = 'wrap'
    }
    $directionChanged(){
        if(this.layoutDirection === GridView.LeftToRight) {
            
        } else {

        }
        if(this.verticalLayoutDirection === GridView.TopToBottom) {

        } else {
            
        }
        this.$updateGeometry()
    }
    $cellChanged(){
        this.$updateGeometry()
    }
    $widthChanged(){
        super.$widthChanged()
        this.$updateGeometry()
    }
    $heightChanged(){
        super.$heightChanged()
        this.$updateGeometry()
    }
    $contentWidthChanged(){

    }
    // $contentXChanged(){

    // }
    $updateGeometry(){
        let width = this.$p.width.func ? this.$p.width.func() : this.width
        this.contentItem.width = width
        this.contentWidth = -1;
        let childLength = typeof this.model === 'number' ? this.model : this.contentItem.children.length
        let rowLength = width / this.cellWidth  <= 1 ? 1 : Math.trunc(width / this.cellWidth)
        let contentHeight = Math.ceil(childLength / rowLength) * this.cellHeight

        this.contentHeight = contentHeight

        this.$updateChildren()
        // clearTimeout(this.$updateTimer)
        // this.$updateTimer = setTimeout(()=>{
        //     if(this.$contentHeightAuto){
        //         let top = 0
        //         let bottom = 0
        //         if(this.contentItem.children.length)
        //         for(let child of this.contentItem.children) {
        //             let childTop = child.dom.offsetTop
        //             let childBottom = childTop + child.height
        //             if(childTop < top) top = childTop
        //             if(childBottom > bottom) bottom = childBottom
        //         }
        //         this.contentHeight = bottom - top
        //     }
        //     if(this.$contentWidthAuto){
        //         let left = 0
        //         let right = 0
        //         if(this.contentItem.children.length)
        //         for(let child of this.contentItem.children) {
        //             let childLeft = child.dom.offsetLeft
        //             let childRight = childLeft + child.width
        //             if(childLeft < left) left = childLeft
        //             if(childRight > right) right = childRight
        //         }
        //         this.contentWidth = right - left
        //     }
        // }, 100)
		
    }
    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        if(!this.model){
            while(this.contentItem.children.length){
                this.contentItem.children.pop().$destroy()
            }
            this.contentItem.children = []
            this.count = 0
        }
        this.$updateGeometry()
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView()
    }
    $currentIndexChanged(){
        //this.currentItem = this.currentIndex > -1 && this.currentIndex < this.contentItem.children.length ? this.contentItem.children[this.currentIndex] : undefined
    }
    positionViewAtBeginning(){
        this.contentX = 0
        this.contentY = 0
    }
    positionViewAtEnd(){
        this.contentX = this.contentItem.children[this.contentItem.children.length-1].x + this.contentItem.children[this.contentItem.children.length-1].width
        this.contentY = this.contentItem.children[this.contentItem.children.length-1].y + this.contentItem.children[this.contentItem.children.length-1].height
    }
    positionViewAtIndex(index, mode){
        let pos = 'start'
        switch(mode){
            case ListView.Beginning: pos = 'start'; break;
            case ListView.Center: pos = 'center'; break;
            case ListView.End: pos = 'end'; break;
        }
        if(pos === 'start') {
            this.contentX = this.contentItem.children[index].x
            this.contentY = this.contentItem.children[index].y
        } else if(pos === 'center') {
            this.contentX = this.contentItem.children[index].x - (this.width - this.contentItem.children[index].width)/2
            this.contentY = this.contentItem.children[index].y - (this.height - this.contentItem.children[index].height)/2
        } else if(pos === 'end') {
            this.contentX = this.contentItem.children[index].x - (this.width - this.contentItem.children[index].width)
            this.contentY = this.contentItem.children[index].y - (this.height - this.contentItem.children[index].height)
        }
    }
    $clear(){}
    $insert(index){
        if(this.model && this.delegate){
            let childRecursive = (obj, indx)=>{
                
                obj.$cP('index', indx)
                obj.index = indx

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater)
                    childRecursive(child, indx)
                }
            }
            // let item = this.createComponent('Item', this.contentItem) 
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
            this.contentItem.children.pop()
            this.contentItem.children.splice(index, 0, obj)

            for(let i = 0; i < this.contentItem.children.length; i++){
                childRecursive(this.contentItem.children[i], i)
            }
            
            this.count = this.contentItem.children.length
            this.$updateChild(index)
            this.$updateChildren()
            try {
                obj.$uP()
            } catch (error) {
                console.error(error)
            }
        }
    }

    $append(wait = false){
        let index = this.contentItem.children.length
        if(this.model && this.delegate){
            let childRecursive = (obj)=>{
                
                obj.$cP('index', index)
                obj.index = index

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater)
                    childRecursive(child)
                }
            }
            // let item = this.createComponent('Item', this.contentItem) 
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
            childRecursive(obj)
            this.count = this.contentItem.children.length
            this.$updateChild(index)
            this.$updateChildren()

            if(wait){
                this.$childrenForUpdate.push(obj)
            } else {
                try {
                    obj.$uP()
                } catch (error) {
                    console.error(error)
                }
            }
        }
    }
    $remove(index, count){
        let removed = this.contentItem.children.splice(index, count)
        for(let rem of removed){
            rem.$destroy()
        }
        let childRecursive = (obj, index)=>{
            obj.index = index
            for(let child of obj.children){
                if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
                childRecursive(child, index)
            }
        }
        for(let i = 0; i < this.contentItem.children.length; i++){
            childRecursive(this.contentItem.children[i], i)
        }
        this.count = this.contentItem.children.length
    }

    $updateView(){
        if(this.model && this.delegate){
            if(typeof this.model === 'number'){
                if(this.model > this.contentItem.children.length){
                    let count = this.model - this.contentItem.children.length
                    for(let i = 0; i < count; i++){      
                        this.$append()
                    }
                } else {
                    this.$remove(3, this.contentItem.children.length - this.model)
                }  
                
            } else if (typeof this.model === 'object'){
                while(this.contentItem.children.length){
                    this.contentItem.children.pop().$destroy()
                }
                this.contentItem.children = []
                this.count = 0

                
                if(this.model.$deps) {
                    this.model.$deps[this.UID] = this
                    for(let i = 0; i < this.model.data.length; i++){
                        this.$append(true)
                    }
                } else {
                    for(let i = 0; i < this.model.length; i++){
                        this.$append(true)
                    }
                }

                while(this.$childrenForUpdate.length){
                    this.$childrenForUpdate.shift().$uP()
                }
                
            } else {

            }
        }
    
    }
    

    $updateChildren(){
        let x = 0
        let y = 0
        for(let i = 0; i < this.contentItem.children.length; i++){
            let child = this.contentItem.children[i]

            if(i > 0) {
                x += this.cellWidth
                if(x + this.cellWidth > this.width){
                    x = 0
                    y += this.cellHeight
                }
            }

            child.x = x
            child.y = y
        }
    }
    $updateChild(index){
        this.contentItem.children[index].xChanged.connect(()=>{
            this.contentX = 0
            this.contentY = 0
        })
        this.contentItem.children[index].yChanged.connect(()=>{
            this.contentX = 0
            this.contentY = 0
        })
    }

    $anchorsChild(index){

    }

    $destroy(){
        clearTimeout(this.$updateTimer)
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        super.$destroy()
    }

}

QML.GridView = GridView