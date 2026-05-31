import {Item} from './Item'
import {Gradient} from './Gradient'
import {Signal} from '../utils/Signal'
import {Qt} from '../utils/Qt'

export class Rectangle extends Item {
    constructor(args) {
        super(args)

        this.$cP('color', 'white', this.$colorChanged)
        this.$cP('radius', 0, this.$radiusChanged)
        this.$cP('gradient', undefined, this.$gradientChanged)
        this.$cPC('border', {
            width: 0,
            color: 'black',
        }, this.$borderChanged)
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
        let rgba = Qt.$colorToRGBA(this.color)
        this.dom.style.backgroundColor = `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
    }
    $opacityChanged(){
        let rgba = Qt.$colorToRGBA(this.color)
        this.dom.style.opacity = 1
        this.dom.style.backgroundColor = `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
    }
    $radiusChanged(){
        this.dom.style.borderRadius = `${this.radius}px`
    }
    $borderChanged(){
        this.dom.style.outline = `${this.border.width}px solid ${this.border.color}`
        this.dom.style.outlineOffset = `-${this.border.width}px`
    }

    $destroy(){
        if(this.gradient) this.gradient.$destroy()

        super.$destroy()
    }
}

QML.Rectangle = Rectangle