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

        this.$updateGeometry()
    }
    $domCreate(){
        super.$domCreate()
    }

    $orientationChanged(){
        this.$updateGeometry()
    }

    // $contentWidthChanged(){

    // }
    // $contentHeightChanged(){
        
    // }

    $contentXChanged(){
        if(this.orientation === ListView.Horizontal){
            super.$contentXChanged()
        }
    }
    $contentYChanged(){
        if(this.orientation === ListView.Vertical){
            super.$contentYChanged()
        }
    }

    $updateGeometry(){
		if(this.orientation === ListView.Vertical && this.contentItem){
            // this.$sP('contentHeight', ()=>{ 
                let childLength = typeof this.model === 'number' ? this.model : this.contentItem.children.length
                let childHeight = this.contentItem.children.length > 0 ? this.contentItem.children[0].height : 100
                let contentHeight = 0
                if(childLength > 0){
                    contentHeight = (childLength-1)*this.spacing + childHeight*childLength
                }
                this.contentHeight = contentHeight
            // })
        }
        
		if(this.orientation === ListView.Horizontal && this.contentItem){
            // this.$sP('contentWidth', ()=>{ 
                let childLength = typeof this.model === 'number' ? this.model : this.contentItem.children.length
                let childWidth = this.contentItem.children.length > 0 ? this.contentItem.children[0].width : 100
                let contentWidth = 0
                if(childLength > 0){
                    contentWidth = (childLength-1)*this.spacing + childWidth*childLength
                }
                this.contentWidth = contentWidth
            // })
        }
        
    }

    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        this.$updateGeometry()
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView()
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

    $insert(index){
        if(this.model && this.delegate){
            let childRecursive = (obj, indx)=>{
                if(obj.$qmlClassName !== 'ListElement'){
                    obj.$cP('index', indx)
                    obj.index = indx
                }

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
                    childRecursive(child, indx)
                }
            }
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
            this.contentItem.children.pop()
            this.contentItem.children.splice(index, 0, obj)

            for(let i = 0; i < this.contentItem.children.length; i++){
                childRecursive(this.contentItem.children[i], i)
            }
            
            this.count = this.contentItem.children.length
            this.$anchorsChild(index)
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
                if(obj.$qmlClassName !== 'ListElement'){
                    obj.$cP('index', index)
                    obj.index = index
                }

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
                    childRecursive(child)
                }
            }
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.contentItem, index: index}) : this.delegate({parent: this.contentItem, index: index})
            childRecursive(obj)
            this.count = this.contentItem.children.length
            this.$anchorsChild(index)

            if(wait){
                this.$childrenForUpdate.push(obj)
            } else {
                // obj.$uP()
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
    

    $anchorsChild(index){
        let child = this.contentItem.children[index]
        if(index === 0){
            child.$sP('anchors.left', ()=>{ return this.contentItem.left })
            child.$sP('anchors.top', ()=>{ return this.contentItem.top })
        } else {
            if(this.orientation === ListView.Horizontal){
                child.$sP('anchors.left', ()=>{ return this.contentItem.children[index-1].right })
                child.$sP('anchors.leftMargin', ()=>{ return this.spacing })
            } else {
                child.$sP('anchors.top', ()=>{ return this.contentItem.children[index-1].bottom })
                child.$sP('anchors.topMargin', ()=>{ return this.spacing })
            }
        }
        
        
    }

    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        super.$destroy()
    }

}

QML.ListView = ListView