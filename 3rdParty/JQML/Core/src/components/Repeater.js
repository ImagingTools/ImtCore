import {Item} from './Item'
import {ListView} from './ListView'
import {GridView} from './GridView'

export class Repeater extends Item {
    $useModel = true
    $childrenForUpdate = []
    constructor(args) {
        super(args)

        this.$cP('model', undefined).connect(this.$modelChanged.bind(this))
        this.$cP('delegate', undefined).connect(this.$delegateChanged.bind(this))
        this.$cP('count', 0)

        this.$cS('itemAdded', 'index', 'item')
        this.$cS('itemRemoved', 'index', 'item')
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

    itemAt(index){
        return index >= 0 && index < this.children.length ? this.children[index] : undefined
    }
    $insert(index){
        let pos = this.parent.children.indexOf(this)
        if(this.model && this.delegate){
            let childRecursive = (obj, indx)=>{
                
                obj.$cP('index', indx)
                obj.index = indx

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater)
                    childRecursive(child, indx)
                }
            }
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.parent, index: index, repeater: this, pos: pos + index + 1}) : this.delegate({parent: this.parent, index: index, repeater: this, pos: pos + index + 1}) //нужно поправить
            obj.$repeater = this
            // this.children.pop()
            this.children.splice(index, 0, obj)

            for(let i = 0; i < this.children.length; i++){
                childRecursive(this.children[i], i)
            }
            
            this.count = this.children.length
            try {
                obj.$uP()
            } catch (error) {
                console.error(error)
            }
            this.itemAdded(obj.index, obj)
        }
    }

    $append(wait = false){
        let index = this.children.length
        let pos = this.parent.children.indexOf(this)
        if(this.model && this.delegate){
            let childRecursive = (obj)=>{
                
                obj.$cP('index', index)
                obj.index = index

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater)
                    childRecursive(child)
                }
            }
            let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.parent, index: index, repeater: this, pos: pos + index + 1}) : this.delegate({parent: this.parent, index: index, repeater: this, pos: pos + index + 1})
            obj.$repeater = this
            this.children.push(obj)
            childRecursive(obj)
            this.count = this.children.length
            if(wait){
                this.$childrenForUpdate.push(obj)
            } else {
                try {
                    obj.$uP()
                } catch (error) {
                    console.error(error)
                }
                this.itemAdded(obj.index, obj)
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
                    let obj = this.$childrenForUpdate.shift()
                    obj.$uP()
                    this.itemAdded(obj.index, obj)
                }
            } else {

            }
        }
    
    }

}

QML.Repeater = Repeater