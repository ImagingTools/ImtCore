import {Item} from './Item'
import {ListModel} from './ListModel'
import {Repeater} from './Repeater'

export class Column extends Item {
    $widthAuto = true
    $heightAuto = true
    constructor(args) {
        super(args)

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
        if(this.$heightAuto)
        this.$sP('height', ()=>{ return this.children.length ? this.children[this.children.length-1].y + this.children[this.children.length-1].height : 0})

        if(this.$widthAuto)
        this.$sP('width', ()=>{ 
            let width = 0
            if(this.children.length)
            for(let child of this.children) {
                let childWidth = child.width
                if(childWidth > width) width = childWidth
            }
            return width
        })
    }
    $updateChildren(){
        let prevIndex = -1
        for(let i = 0; i < this.children.length; i++){
            if(this.children[i].dom && !(this.children[i] instanceof ListModel || this.children[i] instanceof Repeater)){
                this.$anchorsChild(i, prevIndex)
                prevIndex = i
            }
            
        }
    }

    $anchorsChild(index, prevIndex){
        let child = this.children[index]
        
        child.$sP('anchors.left', ()=>{ return this.left })
        if(index === 0){
            child.$sP('anchors.top', ()=>{ return this.top })
        } else if(prevIndex >= 0) {
            let prevChild = this.children[prevIndex]
            child.$sP('anchors.top', ()=>{ return prevChild.bottom })
            child.$sP('anchors.topMargin', ()=>{ return this.spacing })
        }
    }

    $destroy(){
        clearTimeout(this.$childUpdater)
        super.$destroy()
    }
}

QML.Column = Column