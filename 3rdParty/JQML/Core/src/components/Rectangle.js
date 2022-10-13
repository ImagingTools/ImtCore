import {Item} from './Item'
import {Gradient} from './Gradient'
import {Signal} from '../utils/Signal'

export class Rectangle extends Item {
    constructor(parent) {
        super(parent)

        this.$cP('color', 'white').connect(this.$colorChanged.bind(this))
        this.$cP('radius', 0).connect(this.$radiusChanged.bind(this))
        this.$cP('gradient', undefined).connect(this.$gradientChanged.bind(this))
        this.$cPC('border', {
            width: 0,
            color: 'white',
        }).connect(this.$borderChanged.bind(this))
    }

    $domCreate(){
        super.$domCreate()
        this.dom.style.backgroundColor = this.$p.color.val
    }

    $gradientChanged(){
        this.$gradientStyleChanged()
    }
    $gradientStyleChanged(){
        if(this.$p.gradient.val) {
            let style = []
            
            for(let stop of this.$p.gradient.val.$p.stops.val){
                style.push(`${stop.$p.color.val} ${stop.$p.position.val*100}%`)
            }
            if(this.$p.gradient.val.$p.orientation.val === Gradient.Vertical){
                this.dom.style.background = `linear-gradient(180deg, ${style.join(',')})`
            } else {
                this.dom.style.background = `linear-gradient(90deg, ${style.join(',')})`
            }
        }
        
    }

    $colorChanged(){
        this.dom.style.backgroundColor = this.$p.color.val
    }
    $radiusChanged(){
        this.dom.style.borderRadius = `${this.$p.radius.val}px`
    }
    $borderChanged(){
        this.dom.style.outline = `${this.$p['border.width'].val}px solid ${this.$p['border.color'].val}`
        this.dom.style.outlineOffset = `-${this.$p['border.width'].val}px`
    }

    $destroy(){
        if(this.gradient) this.gradient.$destroy()

        super.$destroy()
    }
}

QML.Rectangle = Rectangle