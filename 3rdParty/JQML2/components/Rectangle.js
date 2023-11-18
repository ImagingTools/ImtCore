const { Item } = require('./Item')
const { QColor, QBorder, QReal, QVar } = require('../utils/properties')

class Rectangle extends Item {
    static defaultProperties = {
        color: { type: QColor, value: 'white', changed: '$colorChanged' },
        border: { type: QBorder, changed: '$borderChanged' },
        gradient: { type: QVar, value: undefined, changed: '$gradientChanged' },
        radius: { type: QReal, value: 0, changed: '$radiusChanged' },
    }

    constructor(parent){
        super(parent)
        

    }

    $gradientChanged(){
        if(this.getPropertyValue('gradient')) {
            let style = []
            
            for(let stop of this.getPropertyValue('gradient').getPropertyValue('stops')){
                style.push(`${stop.getPropertyValue('color')} ${stop.getPropertyValue('position')*100}%`)
            }
            if(this.getPropertyValue('gradient').getPropertyValue('orientation') === Gradient.Vertical){
                this.setStyle({
                    background: `linear-gradient(180deg, ${style.join(',')})`
                })
            } else {
                this.setStyle({
                    background: `linear-gradient(90deg, ${style.join(',')})`
                })
            }
        }
    }

    $colorChanged(){
        let rgba = this.getProperty('color').toRGBA()
        this.setStyle({
            opacity: 1,
            backgroundColor: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.getProperty('color').get() === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')})`
        })
    }

    $radiusChanged(){
        this.setStyle({
            borderRadius: `${this.getProperty('radius').get()}px`
        })
    }

    $borderChanged(){
        this.setStyle({
            outline: `${this.getProperty('border').getProperty('width').get()}px solid ${this.getProperty('border').getProperty('color').get()}`,
            outlineOffset: `-${this.getProperty('border').getProperty('width').get()}px`,
        })
    }

    $opacityChanged(){
        let rgba = this.getProperty('color').toRGBA()
        this.setStyle({
            opacity: 1,
            backgroundColor: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.getProperty('color').get() === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')})`
        })
    }
}

module.exports.Rectangle = Rectangle