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
        this.$sP('width', ()=>{ return this.$availableGeometry.length ? this.$availableGeometry[this.$availableGeometry.length-1].x + this.$availableGeometry[this.$availableGeometry.length-1].width : 0})

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
            if(!(this.$availableGeometry[i] instanceof Repeater)){
                this.$anchorsChild(i, prevIndex)
                prevIndex = i
            }
            
        }
    }

    $anchorsChild(index, prevIndex){
        let child = this.$availableGeometry[index]
        
        child.$sP('anchors.top', ()=>{ return this.top })
        if(index === 0){
            child.$sP('anchors.left', ()=>{ return this.left })
        } else if(prevIndex >= 0) {
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