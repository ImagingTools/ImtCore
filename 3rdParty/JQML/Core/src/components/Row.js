import {Item} from './Item'
import {ListModel} from './ListModel'
import {Repeater} from './Repeater'

export class Row extends Item {
    $widthAuto = true
    $heightAuto = true
    constructor(args) {
        super(args)

        this.$cP('spacing', 0).connect(this.$spacingChanged.bind(this))
    }

    $childDomChanged(child){
        if(child.$qmlClassName !== 'Repeater'){
            child.dom.style.position = 'relative'
            child.$p.x.preventDefault = true

            child.widthChanged.connect(()=>{
                child.dom.style.width = `${child.width}px`
                for(let c of this.children){
                    if(c.$qmlClassName !== 'Repeater' && c.dom){
                        c.x = c.dom.offsetLeft
                    }
                }
                this.$updateGeometry()
            })
            
        }
        this.$updateGeometry()
    }
    $spacingChanged(){
        this.dom.style.gap = `${this.spacing}px`
        for(let child of this.children){
            if(child.$qmlClassName !== 'Repeater' && child.dom){
                child.x = child.dom.offsetLeft
            }
        }
        this.$updateGeometry()
    }
    $domCreate(){
        super.$domCreate()
        this.dom.style.display = 'flex'
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
                        x2 = child.width + x1
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

    $destroy(){
        super.$destroy()
    }
}

QML.Row = Row