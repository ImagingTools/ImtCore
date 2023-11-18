const { Animation } = require('./Animation')
const { QVar, QReal, QString } = require('../utils/properties')

class PropertyAnimation extends Animation {

    static defaultProperties = {
        duration: { type: QReal, value: 250 },
        to: { type: QVar, value: undefined },
        from: { type: QVar, value: undefined },
        property: { type: QString, value: '', changed: '$propertyChanged'},
        properties: { type: QString, value: '' },
        target: { type: QVar, value: undefined },

    }

    constructor(parent) {
        super(parent)

        this.getProperty('target').reset(parent)
    }

    tick(timeStamp){
        if(this.getPropertyValue('target') && this.getPropertyValue('running')){
            
            let properties = this.getPropertyValue('properties').split(',')

            for(let prop of properties){
                if(this.getPropertyValue('to') === undefined) this.getProperty('to').reset(this.getPropertyValue('target').getPropertyValue(prop))
                if(this.getPropertyValue('from') === undefined) this.getProperty('from').reset(this.getPropertyValue('target').getPropertyValue(prop))
                
                let increment = (this.getPropertyValue('to') - this.getPropertyValue('from')) / (this.getPropertyValue('duration') / (1000 / 60))
     
                this.getPropertyValue('target').getProperty(prop).reset(this.getPropertyValue('target').getPropertyValue(prop) + increment)
     

                if(this.getPropertyValue('to') >= this.getPropertyValue('from') && this.getPropertyValue('target').getPropertyValue(prop) >= this.getPropertyValue('to')) {
                    this.getPropertyValue('target').getProperty(prop).reset(this.getPropertyValue('to'))
                    let loops = this.$loops + 1
                    if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                        this.restart()
                        this.$loops = loops
                        // this.requestAnimationFrame()
                    } else {
                        this.getProperty('running').reset(false)
                        if(this.$signals.stopped) this.$signals.stopped()
                        if(this.$signals.finished) this.$signals.finished()
                    }
                } else if(this.getPropertyValue('to') <= this.getPropertyValue('from') && this.getPropertyValue('target').getPropertyValue(prop) <= this.getPropertyValue('to')) {
                    this.getPropertyValue('target').getProperty(prop).reset(this.getPropertyValue('to'))
                    let loops = this.$loops + 1
                    if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                        this.restart()
                        this.$loops = loops
                        // this.requestAnimationFrame()
                    } else {
                        this.getProperty('running').reset(false)
                        if(this.$signals.stopped) this.$signals.stopped()
                        if(this.$signals.finished) this.$signals.finished()
                    }
                }
                // this.target.$p[prop].getSignal()()

            }
            // this.target.$updateRect()
        }
    }
    
    $propertyChanged(){
        this.getProperty('properties').reset(this.getPropertyValue('property'))
    }

    restart() { 
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