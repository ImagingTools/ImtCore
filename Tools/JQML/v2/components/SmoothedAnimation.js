const { NumberAnimation } = require('./NumberAnimation')
const { Animation } = require('./Animation')
const { QReal, QInt } = require('../utils/properties')

class SmoothedAnimation extends NumberAnimation {

    static Eased = 0
    static Immediate = 1
    static Sync = 2

    static defaultProperties = {
        velocity: { type: QReal, value: 200 },
        reversingMode: { type: QInt, value: 0 },
        maximumEasingTime: { type: QReal, value: -1 },
    }

    tick(){
        const target = this.getPropertyValue('target')
        if(!target || !this.getPropertyValue('running') || this.getPropertyValue('paused')) return

        const propsStr = this.getPropertyValue('properties') || this.getPropertyValue('property')
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const velocity = this.getPropertyValue('velocity')
        const step = velocity / 60

        for(const prop of props){
            const to = this.getPropertyValue('to')
            const cur = target.getPropertyValue(prop)
            const diff = to - cur
            if(Math.abs(diff) <= step){
                target.getProperty(prop).reset(to)
                const loops = this.$loops + 1
                if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                    this.$loops = loops
                } else {
                    this.getProperty('running').reset(false)
                    if(this.$signals.stopped) this.$signals.stopped()
                    if(this.$signals.finished) this.$signals.finished()
                }
            } else {
                target.getProperty(prop).reset(cur + Math.sign(diff) * step)
            }
        }
    }
}

module.exports.SmoothedAnimation = SmoothedAnimation
