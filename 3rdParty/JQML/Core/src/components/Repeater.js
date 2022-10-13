import {Item} from './Item'

export class Repeater extends Item {
    constructor(parent) {
        super(parent)

        this.$cP('model', undefined).connect(this.$modelChanged.bind(this))
        this.$cP('delegate', undefined).connect(this.$delegateChanged.bind(this))
        this.$cP('count', 0)
    }
    $destroy(){
        this.$model = null
        this.$delegate = null
        super.$destroy()
    }
    $domCreate(){
        super.$domCreate()
    }

    $modelChanged(){
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView()
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
                if(this.$model && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
                if(this.model.$data){
                    this.model.$deps[this.UID] = this
                    for(let i = 0; i < this.model.$data.length; i++){    
                        let obj = this.delegate(this) 
                        let tempModel = {
                            'index': i
                        } 
                        for(let name in this.model.$data[i].$p){
                            tempModel[name] = this.model.$data[i].$p[name].val
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

}

QML.Repeater = Repeater