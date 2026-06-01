const { PropertyAnimation } = require('./PropertyAnimation')
const { Animation } = require('./Animation')
const { QReal, QInt } = require('../utils/properties')

class RotationAnimation extends PropertyAnimation {

    static Numerical = 0
    static Shortest = 1
    static Clockwise = 2
    static Counterclockwise = 3

    static defaultProperties = {
        to: { type: QReal },
        from: { type: QReal },
        direction: { type: QInt, value: 0 },
    }

    tick(){
        const target = this.getPropertyValue('target')
        if(!target || !this.getPropertyValue('running') || this.getPropertyValue('paused')) return

        const propsStr = this.getPropertyValue('properties') || this.getPropertyValue('property')
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const duration = Math.max(1, this.getPropertyValue('duration'))
        const direction = this.getPropertyValue('direction')

        for(const prop of props){
            let from = this.getPropertyValue('from')
            let to = this.getPropertyValue('to')
            if(from === undefined) { from = target.getPropertyValue(prop); this.getProperty('from').reset(from) }
            if(to === undefined) { to = target.getPropertyValue(prop); this.getProperty('to').reset(to) }

            let delta = to - from
            if(direction === RotationAnimation.Shortest){
                while(delta > 180) delta -= 360
                while(delta < -180) delta += 360
            } else if(direction === RotationAnimation.Clockwise){
                if(delta < 0) delta += 360
            } else if(direction === RotationAnimation.Counterclockwise){
                if(delta > 0) delta -= 360
            }

            const increment = delta / (duration / (1000/60))
            const cur = target.getPropertyValue(prop)
            const value = cur + increment
            const done = (increment >= 0 && value >= from + delta) || (increment <= 0 && value <= from + delta)
            if(done){
                target.getProperty(prop).reset(from + delta)
                const loops = this.$loops + 1
                if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                    this.restart()
                    this.$loops = loops
                } else {
                    this.getProperty('running').reset(false)
                    if(this.$signals.stopped) this.$signals.stopped()
                    if(this.$signals.finished) this.$signals.finished()
                }
            } else {
                target.getProperty(prop).reset(value)
            }
        }
    }
}

module.exports.RotationAnimation = RotationAnimation
