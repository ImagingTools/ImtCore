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
    }
    $domCreate(){
        super.$domCreate()
        this.dom.style.display = 'flex'
        this.dom.style.flexWrap = 'wrap'
    }
    $directionChanged(){
        if(this.layoutDirection === GridView.LeftToRight) {
            
        } else {

        }
        if(this.verticalLayoutDirection === GridView.TopToBottom) {

        } else {
            
        }
        // this.$updateChildren()
    }
    $cellChanged(){
        // this.$updateChildren()
    }
    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        this.$updateView()
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
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this, index: index}) : this.delegate({parent: this, index: index})
            this.children.pop()
            this.children.splice(index, 0, obj)

            for(let i = 0; i < this.children.length; i++){
                childRecursive(this.children[i], i)
            }
            
            this.count = this.children.length
            this.$updateChild(index)
            try {
                obj.$uP()
            } catch (error) {
                console.error(error)
            }
        }
    }

    $append(wait = false){
        let index = this.children.length
        if(this.model && this.delegate){
            let childRecursive = (obj)=>{
                
                obj.$cP('index', index)
                obj.index = index

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater)
                    childRecursive(child)
                }
            }
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this, index: index}) : this.delegate({parent: this, index: index})
            childRecursive(obj)
            this.count = this.children.length
            this.$updateChild(index)

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
        let removed = this.children.splice(index, count)
        for(let rem of removed){
            rem.$destroy()
        }
        let childRecursive = (obj, index)=>{
            obj.index = index
            for(let child of obj.children){
                childRecursive(child, index)
            }
        }
        for(let i = 0; i < this.children.length; i++){
            childRecursive(this.children[i], i)
        }
        this.count = this.children.length
    }

    $updateView(){
        if(this.model && this.delegate){
            if(typeof this.model === 'number'){
                if(this.model > this.children.length){
                    let count = this.model - this.children.length
                    for(let i = 0; i < count; i++){      
                        this.$append()
                    }
                } else {
                    this.$remove(3, this.children.length - this.model)
                }  
                
            } else if (typeof this.model === 'object'){
                while(this.children.length){
                    this.children.pop().$destroy()
                }
                this.children = []
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
    
    $updateChild(index){
        let child = this.children[index]
        child.dom.style.position = 'relative'
        child.$sP('width', ()=>{ return this.cellWidth })
        child.$sP('height', ()=>{ return this.cellHeight })
    }

    // $anchorsChild(index){
    //     let child = this.children[index]
    //     if(index === 0){
    //         child.$sP('anchors.left', ()=>{ return this.left })
    //         child.$sP('anchors.top', ()=>{ return this.top })
    //     } else {
    //         if(this.orientation === GridView.layoutDirection){
    //             child.$sP('anchors.left', ()=>{ return this.children[index-1].right })
    //             child.$sP('anchors.leftMargin', ()=>{ return this.spacing })
    //         } else {
    //             child.$sP('anchors.top', ()=>{ return this.children[index-1].bottom })
    //             child.$sP('anchors.topMargin', ()=>{ return this.spacing })
    //         }
    //     }
        
        
    // }

    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.impl.remove()
        super.$destroy()
    }

}

QML.GridView = GridView