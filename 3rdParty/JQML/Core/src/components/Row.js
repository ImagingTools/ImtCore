import {Item} from './Item'
import {ListModel} from './ListModel'
import {Repeater} from './Repeater'

export class Row extends Item {
    $widthAuto = true
    $heightAuto = true
    constructor(parent) {
        super(parent)

        this.$cP('spacing', 0)
        this.$childUpdater = null
    }

    $childChanged(){
        clearTimeout(this.$childUpdater)
        this.$childUpdater = setTimeout(()=>{
            this.$updateGeometry()
            this.$updateChildren()
            this.$uP()
        }, 100)
    }
    $uP(){
        this.$updateChildren()
        super.$uP()
    }
    $domCreate(){
        super.$domCreate()

    }
    $updateGeometry(){
        if(this.$widthAuto)
        this.$sP('width', ()=>{ return this.children.length ? this.children[this.children.length-1].x + this.children[this.children.length-1].width : 0})

        if(this.$heightAuto)
        this.$sP('height', ()=>{ 
            let height = 0
            if(this.children.length)
            for(let child of this.children) {
                let childHeight = child.height
                if(childHeight > height) height = childHeight
            }
            return height
        })
    }
    $updateChildren(){
        let prevIndex = 0
        for(let i = 0; i < this.children.length; i++){
            if(!(this.children[i] instanceof ListModel || this.children[i] instanceof Repeater)){
                this.$anchorsChild(i, prevIndex)
                prevIndex = i
            }
            
        }
    }

    $anchorsChild(index, prevIndex){
        let child = this.children[index]
        let prevChild = this.children[prevIndex]

        child.$sP('anchors.top', ()=>{ return this.top })
        if(index === 0){
            child.$sP('anchors.left', ()=>{ return this.left })
        } else {
            child.$sP('anchors.left', ()=>{ return prevChild.right })
            child.$sP('anchors.leftMargin', ()=>{ return this.spacing })
        }
    }

    $destroy(){
        clearTimeout(this.$childUpdater)
        super.$destroy()
    }
}

QML.Row = Row