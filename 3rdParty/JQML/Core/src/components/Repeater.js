import {Item} from './Item'
import {ListView} from './ListView'
import {GridView} from './GridView'

export class Repeater extends Item {
    $useModel = true
    $childrenForUpdate = []
    constructor(parent) {
        super(parent)

        this.$cP('model', undefined).connect(this.$modelChanged.bind(this))
        this.$cP('delegate', undefined).connect(this.$delegateChanged.bind(this))
        this.$cP('count', 0)

        this.$cS('itemAdded', 'index', 'item')
    }
    $destroy(){
        if(this.model && typeof this.model === 'object' && this.model.$deps && this.model.$deps[this.UID]) delete this.model.$deps[this.UID]
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        
        super.$destroy()
    }
    $domCreate(){
        super.$domCreate()
    }

    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        this.$updateView()
    }
    $delegateChanged(){
        this.$updateView()
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
            let obj = this.delegate.component ? this.delegate.component(this.parent) : this.delegate(this.parent) //нужно поправить
            obj.$repeater = this
            this.children.pop()
            this.children.splice(index, 0, obj)

            for(let i = 0; i < this.children.length; i++){
                childRecursive(this.children[i], i)
            }
            
            this.count = this.children.length
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
            let obj = this.delegate.component ? this.delegate.component(this.parent) : this.delegate(this.parent)
            obj.$repeater = this
            this.children.push(obj)
            childRecursive(obj)
            this.count = this.children.length
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

}

QML.Repeater = Repeater