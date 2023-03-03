import {Item} from './Item'
import {ListModel} from './ListModel'
import {Repeater} from './Repeater'

export class Row extends Item {
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
        if(this.$widthAuto)
        this.$sP('width', ()=>{ 
            let x1 = 0
            let x2 = 0
            let first = true
            for(let child of this.$availableGeometry){
                if(child.visible){
                    if(first){
                        x1 = child.x
                        first = false
                    } else {
                        x2 = child.x + child.width
                    }
                }
            }
            return x2 - x1
        })

        if(this.$heightAuto)
        this.$sP('height', ()=>{ 
            let height = 0
            if(this.$availableGeometry.length)
            for(let child of this.$availableGeometry) {
                let childHeight = child.height
                if(childHeight > height) height = childHeight
            }
            return height
        })
    }
    $updateChildren(){
        let prevIndex = -1
        for(let i = 0; i < this.$availableGeometry.length; i++){
            if(!(this.$availableGeometry[i] instanceof Repeater) && this.$availableGeometry[i].visible){
                this.$anchorsChild(i, prevIndex)
                prevIndex = i
            }
            
        }
    }

    $anchorsChild(index, prevIndex){
        let child = this.$availableGeometry[index]
        
        child.$sP('anchors.top', ()=>{ return this.top })
        if(prevIndex < 0){
            child.$sP('anchors.left', ()=>{ return this.left })
            child.anchors.leftMargin = 0
        } else {
            let prevChild = this.$availableGeometry[prevIndex]
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