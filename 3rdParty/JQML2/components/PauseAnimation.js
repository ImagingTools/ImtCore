const { Animation } = require('./Animation')
const { QReal } = require('../utils/properties')

class PauseAnimation extends Animation {

    static defaultProperties = {
        duration: { type: QReal, value: 250, changed: 'durationChanged' },

    }

    constructor(parent) {
        super(parent)

        this.$duration = this.getPropertyValue('duration')
    }

    tick(){
        if(this.getPropertyValue('running') && !this.getPropertyValue('paused') && this.$duration > 0){
            this.$duration -= 1000/60

            if(this.$duration <= 0) {
                this.getProperty('running').reset(false)
                if(this.$signals.finished) this.$signals.finished()
            }
        }
    }

    durationChanged(){
        this.$duration = this.getPropertyValue('duration')
    }

    restart() { 
        this.$duration = this.getPropertyValue('duration')
        super.restart()
    }


}

module.exports.PauseAnimation = PauseAnimation