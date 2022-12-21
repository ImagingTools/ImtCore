import {Flickable} from './Flickable'

export class ListView extends Flickable {

    static Horizontal = 0
    static Vertical = 1

    static Beginning = 0
    static Center = 1
    static End = 2

    $useModel = true
    $childrenForUpdate = []

    constructor(args) {
        super(args)

        this.$cP('model', undefined).connect(this.$modelChanged.bind(this))
        this.$cP('delegate', undefined).connect(this.$delegateChanged.bind(this))
        this.$cP('count', 0)
        this.$cP('orientation', ListView.Vertical).connect(this.$orientationChanged.bind(this))
        this.$cP('spacing', 0).connect(this.$spacingChanged.bind(this))
        this.$cP('currentIndex', -1).connect(this.$currentIndexChanged.bind(this))
        this.$cP('currentItem', undefined)
    }
    $domCreate(){
        super.$domCreate()
    }

    $orientationChanged(){

    }

    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView()
    }
    $spacingChanged(){

    }
    $currentIndexChanged(){
        //this.currentItem = this.currentIndex > -1 && this.currentIndex < this.children.length ? this.children[this.currentIndex] : undefined
    }
    itemAtIndex(index){
        return index >= 0 && index < this.children.length ? this.children[index] : undefined
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
            this.$anchorsChild(index)
            obj.$uP()
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
            this.$anchorsChild(index)

            if(wait){
                this.$childrenForUpdate.push(obj)
            } else {
                obj.$uP()
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

    $anchorsChild(index){
        let child = this.children[index]

        child.$sP('anchors.left', ()=>{ return this.left })
        child.$sP('anchors.top', ()=>{ return this.top })
        // child.anchors.left = 0
        // child.anchors.top = 0
        child.$sP('anchors.topMargin', ()=>{ return this.orientation === ListView.Vertical ? this.children[0].height * child.index + child.index * this.spacing : 0 })
        child.$sP('anchors.leftMargin', ()=>{ return this.orientation === ListView.Horizontal ? this.children[0].width * child.index + child.index * this.spacing : 0 })
    }

    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.impl.remove()
        super.$destroy()
    }

}

QML.ListView = ListView