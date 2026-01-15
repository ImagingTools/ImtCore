// import {QtObject} from './QtObject'
import {Item} from './Item'

export class InnerShadow extends Item {
    constructor(args) {
        super(args)

        this.$cP('color', 'black', this.$updateShadow)
        this.$cP('horizontalOffset', 0, this.$updateShadow)
        this.$cP('verticalOffset', 0, this.$updateShadow)
        this.$cP('radius', 0, this.$updateShadow)
        this.$cP('samples', 0, this.$updateShadow)
        this.$cP('source', undefined, this.$updateShadow)
        this.$cP('spread', 0, this.$updateShadow)
        
    }
    $domCreate(){
        super.$domCreate()

    }

    $updateShadow(){
        if(this.source) {
            let rgba = Qt.$colorToRGBA(this.color)
            this.source.dom.style.boxShadow = `${this.horizontalOffset}px ${this.verticalOffset}px ${this.radius}px ${this.spread}px rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity}) inset`
        }
    }

}

QML.InnerShadow = InnerShadow