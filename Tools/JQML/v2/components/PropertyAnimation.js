const { Animation } = require('./Animation')
const { QVar, QReal, QString } = require('../utils/properties')
const { $easingFunc } = require('../utils/easing')

class PropertyAnimation extends Animation {

    static defaultProperties = {
        duration: { type: QReal, value: 250 },
        to: { type: QVar, value: undefined },
        from: { type: QVar, value: undefined },
        property: { type: QString, value: '', changed: '$propertyChanged'},
        properties: { type: QString, value: '' },
        target: { type: QVar, value: undefined },
        easing: { type: QVar, value: undefined, changed: '$easingChanged' },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)

        this.getProperty('target').reset(parent)
        this.$elapsed = 0
        this.$easingObj = { type: 0, amplitude: 1.0, overshoot: 1.70158, period: 0.3 }
    }

    $easingChanged(){
        const v = this.getPropertyValue('easing')
        if(v && typeof v === 'object'){
            if('type' in v) this.$easingObj.type = v.type
            if('amplitude' in v) this.$easingObj.amplitude = v.amplitude
            if('overshoot' in v) this.$easingObj.overshoot = v.overshoot
            if('period' in v) this.$easingObj.period = v.period
        }
    }

    tick(timeStamp){
        if(this.getPropertyValue('target') && this.getPropertyValue('running')){
            
            let properties = this.getPropertyValue('properties').split(',')
            const duration = this.getPropertyValue('duration')

            for(let prop of properties){
                if(this.getPropertyValue('to') === undefined) this.getProperty('to').reset(this.getPropertyValue('target').getPropertyValue(prop))
                if(this.getPropertyValue('from') === undefined) this.getProperty('from').reset(this.getPropertyValue('target').getPropertyValue(prop))

                const from = this.getPropertyValue('from')
                const to = this.getPropertyValue('to')

                this.$elapsed += 1000 / 60
                let t = Math.min(1, this.$elapsed / duration)
                let progress = $easingFunc(t, this.$easingObj)
                let value = from + (to - from) * progress

                if(t >= 1){
                    this.getPropertyValue('target').getProperty(prop).reset(to)
                    this.$elapsed = 0
                    let loops = this.$loops + 1
                    if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                        this.restart()
                        this.$loops = loops
                    } else {
                        this.getProperty('running').reset(false)
                        if(this.$signals.stopped) this.$signals.stopped()
                        if(this.$signals.finished) this.$signals.finished()
                    }
                } else {
                    this.getPropertyValue('target').getProperty(prop).reset(value)
                }
            }
        }
    }
    
    $propertyChanged(){
        this.getProperty('properties').reset(this.getPropertyValue('property'))
    }

    restart() { 
        this.$elapsed = 0
        let properties = this.getPropertyValue('properties').split(',')
        for(let prop of properties){
            if(this.getPropertyValue('target').getPropertyValue(prop) !== this.getPropertyValue('from')){
                this.getPropertyValue('target').getProperty(prop).reset(this.getPropertyValue('from'))
            }
            
        }
		super.restart()
    }


}

module.exports.PropertyAnimation = PropertyAnimation