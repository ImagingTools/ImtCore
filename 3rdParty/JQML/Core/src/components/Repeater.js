import {Item} from './Item'
import {ListView} from './ListView'
import {GridView} from './GridView'

export class Repeater extends Item {
    $useModel = true
    $childrenForUpdate = []
    $widthAuto = true
    $heightAuto = true
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
        if(!this.model){
            while(this.children.length){
                this.children.pop().$destroy()
            }
            this.children = []
            this.count = 0
        }
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
                
                if(obj.$qmlClassName !== 'ListElement'){
                    obj.$cP('index', indx)
                    obj.index = indx
                }

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
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
            this.$updateGeometry()
            try {
                this.$uP()
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
                
                if(obj.$qmlClassName !== 'ListElement'){
                    obj.$cP('index', index)
                    obj.index = index
                }
                

                for(let child of obj.children){
                    if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
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
                this.$updateGeometry()
                try {
                    this.$uP()
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
                if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
                childRecursive(child, index)
            }
        }
        for(let i = 0; i < this.children.length; i++){
            childRecursive(this.children[i], i)
        }
        this.count = this.children.length
    }

    $updateGeometry(){
        if(this.parent && this.children.length > 0){
            if(this.parent.$qmlClassName === 'Column'){
                if(this.$widthAuto) this.$sP('width', ()=>{return this.children[0].width})
                if(this.$heightAuto) this.$sP('height', ()=>{return this.children[this.children.length-1].y - this.children[0].y + this.children[0].height})
            } else if(this.parent.$qmlClassName === 'Row'){
                if(this.$widthAuto) this.$sP('height', ()=>{return this.children[0].height})
                if(this.$heightAuto) this.$sP('width', ()=>{return this.children[this.children.length-1].x - this.children[0].x + this.children[0].width})
            } else {
                if(this.$widthAuto) this.$sP('width', ()=>{return this.children[0].width})
                if(this.$heightAuto) this.$sP('height', ()=>{return this.children[0].height})
            }
        }
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
                this.$updateGeometry()
                this.$uP()
            } else {

            }
        }
    
    }

}

QML.Repeater = Repeater