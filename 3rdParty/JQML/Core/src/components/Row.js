import {Item} from './Item'
import {Repeater} from './Repeater'

export class Row extends Item {
    $wAuto = true
    $hAuto = true

    constructor(parent) {
        super(parent)

        this.$cP('spacing', 0).connect(this.$spacingChanged.bind(this))
    }

    $domCreate(){
        super.$domCreate()

    }

    $spacingChanged(){
        this.$updateGeometry()
    }
    $widthChanged(){
        super.$widthChanged()
        this.$wAuto = false
    }
    $heightChanged(){
        super.$heightChanged()
        this.$hAuto = false
    }
    $updateGeometry(){
        let dx = 0
        let height = 0
        let i = 0
        for(let child of this.children){
            
            if(child instanceof Repeater){
                let rDx = 0
                let rHeight = 0
                let k = 0
                for(let rChild of child.children){
                    rChild.x = k === 0 ? 0 : rDx + this.spacing
                    rDx += rChild.width + (k === 0 ? 0 : this.spacing)
                    if(rChild.height > rHeight) rHeight = rChild.height
                    k++
                }

                if(child.width < rDx) child.width = rDx
                if(child.height < rHeight) child.height = rHeight


            }

            child.x = i === 0 ? 0 : dx + this.spacing
            dx += child.width + (i === 0 ? 0 : this.spacing)
            if(child.height > height) height = child.height
            i++
        }

        if(this.$wAuto || this.width < dx){
            this.width = dx
            this.$wAuto = true
        }
        
        if(this.$hAuto){
            // this.height = height
            if(this.$p.height.func){
                height = this.$p.height.func()
                if(this.$p.height.val !== height){
                    this.$p.height.val = height
                    this.$p.height.signal()
                }
            } else {
                if(this.$p.height.val !== height){
                    this.$p.height.val = height
                    this.$p.height.signal()
                }
            }     
            this.$hAuto = true
        }

        for(let child of this.children){
            if(child.$changedHeight){
                child.height = this.$p.height.val
                child.$changedHeight = true
            } 
        }

        super.$updateGeometry()
    }
}

QML.Row = Row