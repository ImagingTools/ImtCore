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
        this.$sP('height', ()=>{ 
            let y1 = 0
            let y2 = 0
            let first = true
            for(let child of this.$availableGeometry){
                if(child.visible){
                    if(first){
                        y1 = child.y
                        y2 = child.height + y1
                        first = false
                    } else {
                        y2 = child.y + child.height
                    }
                }
            }
            return y2 - y1
        })

        if(this.$widthAuto)
        this.$sP('width', ()=>{ 
            let width = 0
            if(this.$availableGeometry.length)
            for(let child of this.$availableGeometry) {
                let childWidth = child.width
                if(childWidth > width) width = childWidth
            }
            return width
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
        
        child.$sP('anchors.left', ()=>{ return this.left })
        if(prevIndex < 0){
            child.$sP('anchors.top', ()=>{ return this.top })
            child.anchors.topMargin = 0
        } else {
            let prevChild = this.$availableGeometry[prevIndex]
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