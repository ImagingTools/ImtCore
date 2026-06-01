const { NumberAnimation } = require('./NumberAnimation')
const { Animation } = require('./Animation')
const { QReal } = require('../utils/properties')

class SpringAnimation extends NumberAnimation {

    static defaultProperties = {
        spring: { type: QReal, value: 0 },
        damping: { type: QReal, value: 0 },
        epsilon: { type: QReal, value: 0.01 },
        modulus: { type: QReal, value: 0 },
        mass: { type: QReal, value: 1 },
        velocity: { type: QReal, value: 0 },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$vel = 0
    }

    restart(){
        this.$vel = 0
        super.restart()
    }

    tick(){
        const target = this.getPropertyValue('target')
        if(!target || !this.getPropertyValue('running') || this.getPropertyValue('paused')) return

        const propsStr = this.getPropertyValue('properties') || this.getPropertyValue('property')
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const k = this.getPropertyValue('spring')
        const d = this.getPropertyValue('damping')
        const m = Math.max(0.0001, this.getPropertyValue('mass'))
        const eps = this.getPropertyValue('epsilon')
        const dt = 1/60

        for(const prop of props){
            const to = this.getPropertyValue('to')
            const cur = target.getPropertyValue(prop)
            const force = k * (to - cur) - d * this.$vel
            this.$vel += (force / m) * dt
            const next = cur + this.$vel * dt
            target.getProperty(prop).reset(next)

            if(Math.abs(next - to) < eps && Math.abs(this.$vel) < eps){
                target.getProperty(prop).reset(to)
                this.$vel = 0
                const loops = this.$loops + 1
                if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                    this.$loops = loops
                } else {
                    this.getProperty('running').reset(false)
                    if(this.$signals.stopped) this.$signals.stopped()
                    if(this.$signals.finished) this.$signals.finished()
                }
            }
        }
    }
}

module.exports.SpringAnimation = SpringAnimation
