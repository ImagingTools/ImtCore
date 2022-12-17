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
        this.dom.style.backgroundColor = this.color
    }

    $gradientChanged(){
        this.$gradientStyleChanged()
    }
    $gradientStyleChanged(){
        if(this.gradient) {
            let style = []
            
            for(let stop of this.gradient.stops){
                style.push(`${stop.color} ${stop.position*100}%`)
            }
            if(this.gradient.orientation === Gradient.Vertical){
                this.dom.style.background = `linear-gradient(180deg, ${style.join(',')})`
            } else {
                this.dom.style.background = `linear-gradient(90deg, ${style.join(',')})`
            }
        }
        
    }

    $colorChanged(){
        this.dom.style.backgroundColor = this.color
    }
    $radiusChanged(){
        this.dom.style.borderRadius = `${this.radius}px`
    }
    $borderChanged(){
        this.dom.style.outline = `${this['border.width']}px solid ${this['border.color']}`
        this.dom.style.outlineOffset = `-${this['border.width']}px`
    }

    $destroy(){
        if(this.gradient) this.gradient.$destroy()

        super.$destroy()
    }
}

QML.Rectangle = Rectangle