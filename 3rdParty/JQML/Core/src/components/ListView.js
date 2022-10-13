import {Flickable} from './Flickable'

export class ListView extends Flickable {

    static Horizontal = 0
    static Vertical = 1

    static Beginning = 0
    static Center = 1
    static End = 2


    constructor(parent) {
        super(parent)

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
        this.$updateChildren()
    }

    $modelChanged(){
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView()
    }
    $spacingChanged(){
        this.$updateChildren()
    }
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
        if(this.orientation === ListView.Vertical) {
            let dy = 0
            for(let i = 0; i < this.children.length; i++){
                let child = this.children[i]
                child.x = 0
                child.y = i === 0 ? 0 : dy + this.spacing
                dy += child.height + (i === 0 ? 0 : this.spacing)
            }
        } else {
            let dx = 0
            for(let i = 0; i < this.children.length; i++){
                let child = this.children[i]
                child.y = 0
                child.x = i === 0 ? 0 : dx + this.spacing
                dx += child.width + (i === 0 ? 0 : this.spacing)
            }

        }
    }

    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        this.$model = null
        this.$delegate = null
        this.impl.remove()
        super.$destroy()
    }

}

QML.ListView = ListView