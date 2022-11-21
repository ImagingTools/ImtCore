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
                obj.$cP('model',model)
                obj.$cP('index',index)
                for(let child of obj.children){
                    childRecursive(child, model, index)
                }
            }

            this.count = 0
            if(typeof this.model === 'number'){     
                for(let i = 0; i < this.model; i++){      
                    let obj = this.delegate(this)

                    childRecursive(obj, {
                        index: i,
                    }, i)
                    this.count = i+1
                }
            } else if (typeof this.model === 'object'){
                if(this.model && this.model.$deps) this.model.$deps[this.UID] = this

                let data = this.model.$data

                // if(Array.isArray(data)){
                //     if(data.length === 1){
                //         if(typeof data[0] === 'object' && data[0].$data === undefined){
                //             data = data[0]
                //         }
                //     }
                // }

                if(Array.isArray(data) || data.$data === undefined){
                    let i = 0
                    for(let key in data){
                        let obj = this.delegate(this)
                        let tempModel = {
                            'index': i
                        }
                        if(data[key].$data){
                            for(let name in data[key].$data){
                                tempModel[name] = data[key].$data[name]
                            }
                        } else {
                            for(let name in data[key]){
                                tempModel[name] = data[key][name]
                            }
                        }
                        
                        childRecursive(obj, tempModel, i)
                        this.count = i+1
                        i++
                    }
                } else {
                    let obj = this.delegate(this)
                    let tempModel = {
                        'index': 0
                    }
                    for(let name in data){
                        tempModel[name] = data[name]
                    }
                    childRecursive(obj, tempModel, 0)
                    this.count = 1
                }
                // if(this.model.$data){
                //     this.model.$deps[this.UID] = this

                //     if(Array.isArray(this.model.$data)){
                //         if(Array.isArray(this.model.$data[0])){
                //             for(let i = 0; i < this.model.$data[0].length; i++){    
                //                 let obj = this.delegate(this) 
                //                 let tempModel = {
                //                     'index': i
                //                 } 
                //                 if(this.model.$data[0][i].$data){
                //                     if(Array.isArray(this.model.$data[0][i].$data)){
                //                         for(let name in this.model.$data[0][i].$data[0]){
                //                             tempModel[name] = this.model.$data[0][i].$data[0][name]
                //                         }
                //                     } else {
                //                         for(let name in this.model.$data[0][i].$data){
                //                             tempModel[name] = this.model.$data[0][i].$data[name]
                //                         }
                //                     }
                                    
                //                 } else if(this.model.$data[0][i].$p){
                //                     for(let name in this.model.$data[0][i].$p){
                //                         tempModel[name] = this.model.$data[0][i].$p[name].val
                //                     }
                //                 } else {
                //                     for(let name in this.model.$data[0][i]){
                //                         tempModel[name] = this.model.$data[0][i][name]
                //                     }
                //                 }
                                
                //                 // UID += UID - tempUID
                //                 // obj.$cPC('model', {
                //                 //     model: this.model.$data[i],
                //                 //     index: i,
                //                 // })
                //                 obj.$cPC('model',tempModel)
                //                 obj.$cP('index',i)
                //                 childRecursive(obj, tempModel, i)
                //                 this.count = i
                //             }
                //         } else {
                //             let obj = this.delegate(this) 
                //             let tempModel = {
                //                 'index': i
                //             } 
                //             if(this.model.$data[0].$data){
                //                 if(Array.isArray(this.model.$data[0].$data)){
                //                     for(let name in this.model.$data[0].$data[0]){
                //                         tempModel[name] = this.model.$data[0].$data[0][name]
                //                     }
                //                 } else {
                //                     for(let name in this.model.$data[0].$data){
                //                         tempModel[name] = this.model.$data[0].$data[name]
                //                     }
                //                 }
                                
                //             } else if(this.model.$data[0].$p){
                //                 for(let name in this.model.$data[0].$p){
                //                     tempModel[name] = this.model.$data[0].$p[name].val
                //                 }
                //             } else {
                //                 for(let name in this.model.$data[0]){
                //                     tempModel[name] = this.model.$data[0][name]
                //                 }
                //             }
                            
                //             // UID += UID - tempUID
                //             // obj.$cPC('model', {
                //             //     model: this.model.$data[i],
                //             //     index: i,
                //             // })
                //             obj.$cPC('model',tempModel)
                //             obj.$cP('index',0)
                //             childRecursive(obj, tempModel, 0)
                //             this.count = 1
                //         }
                        
                //     } else {
                //         for(let i = 0; i < this.model.$data.length; i++){    
                //             let obj = this.delegate(this) 
                //             let tempModel = {
                //                 'index': i
                //             } 
                //             if(this.model.$data[i].$p){
                //                 for(let name in this.model.$data[i].$p){
                //                     tempModel[name] = this.model.$data[i].$p[name].val
                //                 }
                //             } else {
                //                 for(let name in this.model.$data[i]){
                //                     tempModel[name] = this.model.$data[i][name]
                //                 }
                //             }
                            
                //             // UID += UID - tempUID
                //             // obj.$cPC('model', {
                //             //     model: this.model.$data[i],
                //             //     index: i,
                //             // })
                //             obj.$cPC('model',tempModel)
                //             obj.$cP('index',i)
                //             childRecursive(obj, tempModel, i)
                //             this.count = i
                //         }
                //     }
                    
                // } else {
                //     for(let i = 0; i < this.model.length; i++){
                //         // let tempUID = UID   
                //         let obj = this.delegate(this)  
                //         // UID += UID - tempUID
                //         obj.$cPC('model', {
                //             index: i,
                //         })
                //         obj.$cP('index',i)
                //         childRecursive(obj, {
                //             index: i,
                //         }, i)
                //         this.count = i
                //     }
                // }
                

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
        this.impl.remove()
        super.$destroy()
    }

}

QML.ListView = ListView