import {Item} from './Item'
import {ListModel} from './ListModel'
import {Repeater} from './Repeater'

export class Column extends Item {
    $widthAuto = true
    $heightAuto = true
    constructor(args) {
        super(args)

        this.$cP('spacing', 0).connect(this.$spacingChanged.bind(this))
    }

    $childDomChanged(child){
        this.$cssDisplay = this.dom.style.display
        if(child.$qmlClassName !== 'Repeater'){
            child.dom.style.position = 'relative'
            child.$p.y.preventDefault = true

            child.heightChanged.connect(()=>{
                child.dom.style.height = `${child.height}px`
                for(let c of this.children){
                    if(c.$qmlClassName !== 'Repeater' && c.dom){
                        c.y = c.dom.offsetTop
                    }
                }
                this.$updateGeometry()
            })
            child.visibleChanged.connect(()=>{
                if(child.visible){
                    child.dom.style.display = child.$cssDisplay     
                } else {
                    child.$cssDisplay = child.dom.style.display
                    child.dom.style.display = 'none'
                }
                child.dom.style.height = `${child.height}px`
                for(let c of this.children){
                    if(c.$qmlClassName !== 'Repeater' && c.dom){
                        c.y = c.dom.offsetTop
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
                child.y = child.dom.offsetTop
            }
        }
        this.$updateGeometry()
    }
    $domCreate(){
        super.$domCreate()
        this.dom.style.display = 'flex'
        this.dom.style.flexDirection = 'column'
    }
    $updateGeometry(){
        if(this.$heightAuto){
            this.$p.height.preventDefault = true
            let y1 = 0
            let y2 = 0
            let first = true
            for(let child of this.$availableGeometry){
                let rect = child.dom.getBoundingClientRect()
                if(child.visible){
                    if(first){
                        y1 = rect.y
                        y2 = child.height + rect.y
                        first = false
                    } else {
                        y2 = rect.y + child.height
                    }
                }
            }
            this.height = y2-y1
        } else {
            this.$p.height.preventDefault = false
        }

        if(this.$widthAuto){
            this.$p.width.preventDefault = true
            let width = 0
            if(this.$availableGeometry.length)
            for(let child of this.$availableGeometry) {
                let childWidth = child.width
                if(childWidth > width) width = childWidth
            }
            this.width = width
        } else {
            this.$p.width.preventDefault = false
        }
    }
    

    $destroy(){
        super.$destroy()
    }
}

QML.Column = Column