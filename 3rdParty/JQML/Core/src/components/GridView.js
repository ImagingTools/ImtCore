import {Flickable} from './Flickable'

export class GridView extends Flickable {
    static Beginning = 0
    static Center = 1
    static End = 2

    static LeftToRight = 0
    static RightToLeft = 1
    static BottomToTop = 2
    static BottomToTop = 3

    $wAuto = false
    $hAuto = false

    constructor(parent) {
        super(parent)

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
    }
    $domCreate(){
        super.$domCreate()
    }
    $directionChanged(){
        this.$updateChildren()
    }
    $cellChanged(){
        this.$updateChildren()
    }
    $modelChanged(){
        this.$updateView()
    }
    $widthChanged(){
        this.$wAuto = true
        super.$widthChanged()
    }
    $heightChanged(){
        this.$hAuto = true
        super.$heightChanged()
    }
    $delegateChanged(){
        this.$updateView()
    }
    // $spacingChanged(){
    //     this.$updateChildren()
    // }
    $currentIndexChanged(){
        //this.currentItem = this.currentIndex > -1 && this.currentIndex < this.children.length ? this.children[this.currentIndex] : undefined
    }
    positionViewAtBeginning(){
        this.contentX = 0
        this.contentY = 0
    }
    positionViewAtEnd(){
        this.contentX = this.children[this.children.length-1].x + this.children[this.children.length-1].width
        this.contentY = this.children[this.children.length-1].y + this.children[this.children.length-1].height
    }
    positionViewAtIndex(index, mode){
        let pos = 'start'
        switch(mode){
            case ListView.Beginning: pos = 'start'; break;
            case ListView.Center: pos = 'center'; break;
            case ListView.End: pos = 'end'; break;
        }
        if(pos === 'start') {
            this.contentX = this.children[index].x
            this.contentY = this.children[index].y
        } else if(pos === 'center') {
            this.contentX = this.children[index].x - (this.width - this.children[index].width)/2
            this.contentY = this.children[index].y - (this.height - this.children[index].height)/2
        } else if(pos === 'end') {
            this.contentX = this.children[index].x - (this.width - this.children[index].width)
            this.contentY = this.children[index].y - (this.height - this.children[index].height)
        }
    }

    $updateView(){
        if(this.model && this.delegate){
            
            while(this.children.length){
                this.children.pop().$destroy()
            }
            // for(let child of this.children){
            //     child.$destroy()
            // }
            this.children = []

            let childRecursive = (obj, model, index)=>{
                // obj.$cPC('model', model)
                obj.$cPC('model',model)
                obj.$cP('index',index)
                for(let child of obj.children){
                    childRecursive(child, model, index)
                }
            }

            this.count = 0
            if(typeof this.model === 'number'){     
                for(let i = 0; i < this.model; i++){    
                    // let tempUID = UID     
                    let obj = this.delegate(this)
                    // UID += UID - tempUID
                    
                    obj.$cPC('model', {
                        index: i,
                    })
                    obj.$cP('index',i)
                    childRecursive(obj, {
                        index: i,
                    }, i)
                    this.count = i
                }
            } else if (typeof this.model === 'object'){
                // if(this.$model && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
                if(this.model && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
                if(this.model.$data){
                    this.model.$deps[this.UID] = this

                    if(Array.isArray(this.model.$data)){
                        for(let i = 0; i < this.model.$data[0].length; i++){    
                            let obj = this.delegate(this) 
                            let tempModel = {
                                'index': i
                            } 
                            if(this.model.$data[0][i].$data){
                                if(Array.isArray(this.model.$data[0][i].$data)){
                                    for(let name in this.model.$data[0][i].$data[0]){
                                        tempModel[name] = this.model.$data[0][i].$data[0][name]
                                    }
                                } else {
                                    for(let name in this.model.$data[0][i].$data){
                                        tempModel[name] = this.model.$data[0][i].$data[name]
                                    }
                                }
                                
                            } else if(this.model.$data[0][i].$p){
                                for(let name in this.model.$data[0][i].$p){
                                    tempModel[name] = this.model.$data[0][i].$p[name].val
                                }
                            } else {
                                for(let name in this.model.$data[0][i]){
                                    tempModel[name] = this.model.$data[0][i][name]
                                }
                            }
                            
                            // UID += UID - tempUID
                            // obj.$cPC('model', {
                            //     model: this.model.$data[i],
                            //     index: i,
                            // })
                            obj.$cPC('model',tempModel)
                            obj.$cP('index',i)
                            childRecursive(obj, tempModel, i)
                            this.count = i
                        }
                    } else {
                        for(let i = 0; i < this.model.$data.length; i++){    
                            let obj = this.delegate(this) 
                            let tempModel = {
                                'index': i
                            } 
                            if(this.model.$data[i].$p){
                                for(let name in this.model.$data[i].$p){
                                    tempModel[name] = this.model.$data[i].$p[name].val
                                }
                            } else {
                                for(let name in this.model.$data[i]){
                                    tempModel[name] = this.model.$data[i][name]
                                }
                            }
                            
                            // UID += UID - tempUID
                            // obj.$cPC('model', {
                            //     model: this.model.$data[i],
                            //     index: i,
                            // })
                            obj.$cPC('model',tempModel)
                            obj.$cP('index',i)
                            childRecursive(obj, tempModel, i)
                            this.count = i
                        }
                    }
                    
                } else {
                    for(let i = 0; i < this.model.length; i++){
                        // let tempUID = UID   
                        let obj = this.delegate(this)  
                        // UID += UID - tempUID
                        obj.$cPC('model', {
                            index: i,
                        })
                        obj.$cP('index',i)
                        childRecursive(obj, {
                            index: i,
                        }, i)
                        this.count = i
                    }
                }
                

            } else {

            }
            this.$uP()
            this.$updateGeometry()
            
 

        }
    }

    $updateGeometry(){
        this.$updateChildren()

        super.$updateGeometry()
    }

    $updateChildren(){
        let maxWidth = this.$wAuto && this.$contentWAuto ? this.width : 9999999
        let maxHeight = this.$hAuto && this.$contentHAuto ? this.height : 9999999

        // console.log('maxWidth = ', maxWidth, 'maxHeight = ', maxHeight)

        let dy = this.verticalLayoutDirection === GridView.TopToBottom ? 0 : maxHeight
        let dx = this.layoutDirection === GridView.LeftToRight ? 0 : maxWidth

        for(let i = 0; i < this.children.length; i++){
            let child = this.children[i]
            
            if(this.layoutDirection === GridView.LeftToRight){
                if(dx + this.cellWidth > maxWidth) {
                    dx = 0
                    if(this.verticalLayoutDirection === GridView.TopToBottom){
                        dy += this.cellHeight
                    } else {
                        dy -= this.cellHeight
                    }
                }
            } else {
                if(dx < 0) {
                    dx = maxWidth
                    if(this.verticalLayoutDirection === GridView.TopToBottom){
                        dy += this.cellHeight
                    } else {
                        dy -= this.cellHeight
                    }
                }
            }
            if(this.verticalLayoutDirection === GridView.TopToBottom){
                if(dy + this.cellHeight > maxHeight) dy = 0
            } else {
                if(dy < 0) dy = maxHeight
            }

            child.x = dx
            child.y = dy
            // child.dom.style.zIndex = i

            if(this.layoutDirection === GridView.LeftToRight){
                dx += this.cellWidth
            } else {
                dx -= this.cellWidth
            }
            
            // console.log('child.x = ', child.x, 'child.y = ', child.y)
        }

    }

    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        this.impl.remove()
        super.$destroy()
    }

}

QML.GridView = GridView