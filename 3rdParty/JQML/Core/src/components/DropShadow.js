// import {QtObject} from './QtObject'
import {Item} from './Item'

export class DropShadow extends Item {
    constructor(parent) {
        super(parent)

        this.$cP('color', 'black').connect(this.$updateShadow.bind(this))
        this.$cP('horizontalOffset', 0).connect(this.$updateShadow.bind(this))
        this.$cP('verticalOffset', 0).connect(this.$updateShadow.bind(this))
        this.$cP('radius', 4).connect(this.$updateShadow.bind(this))
        this.$cP('samples', 9).connect(this.$updateShadow.bind(this))
        this.$cP('source', undefined).connect(this.$updateShadow.bind(this))
        this.$cP('spread', 0).connect(this.$updateShadow.bind(this))
        
    }
    $domCreate(){
        super.$domCreate()

    }

    $updateShadow(){
        if(this.$p.source.val) {
            this.source.dom.style.boxShadow = `${this.$p.horizontalOffset.val}px ${this.$p.verticalOffset.val}px ${this.$p.radius.val}px ${this.$p.spread.val}px ${this.$p.color.val}`
        }
    }

}

QML.DropShadow = DropShadow