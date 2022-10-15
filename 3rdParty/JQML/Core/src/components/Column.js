import {Item} from './Item'
import {Repeater} from './Repeater'

export class Column extends Item {
    $wAuto = true
    $hAuto = true

    constructor(parent) {
        super(parent)

        this.$cP('spacing', 0).connect(this.$spacingChanged.bind(this))
    }
    $uP(){
        super.$uP()
        this.$updateGeometry()
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
        let dy = 0
        let width = 0
        let i = 0
        for(let child of this.children){
            if(child instanceof Repeater){
                let rDy = 0
                let rWidth = 0
                let k = 0
                for(let rChild of child.children){
                    rChild.y = k === 0 ? 0 : rDy + this.$p.spacing.val
                    rDy += rChild.$p.height.val + (k === 0 ? 0 : this.$p.spacing.val)
                    if(rChild.$p.width.val > rWidth) rWidth = rChild.$p.width.val
                    k++
                }
                child.height = rDy
                child.width = rWidth


            }

            child.y = i === 0 ? 0 : dy + this.$p.spacing.val
            dy += child.$p.height.val + (i === 0 ? 0 : this.$p.spacing.val)
            if(child.$p.width.val > width) width = child.$p.width.val
            i++
        }
        
        if(this.$wAuto){
            if(this.$p.width.func){
                width = this.$p.width.func()
                if(this.$p.width.val !== width){
                    this.$p.width.val = width
                    this.dom.style.width = `${this.$p.width.val}px`
                    this.$xChanged()
                    // this.$p.width.signal()
                }
            } else {
                if(this.$p.width.val !== width){
                    this.$p.width.val = width
                    this.dom.style.width = `${this.$p.width.val}px`
                    this.$xChanged()
                    // this.$p.width.signal()
                }
            }        
            
            this.$wAuto = true
        }
        if(this.$hAuto){
            this.$p.height.val = dy
            this.dom.style.height = `${this.$p.height.val}px`
            this.$yChanged()
            // this.height = dy
            this.$hAuto = true
        }


        super.$updateGeometry()
    }
}

QML.Column = Column