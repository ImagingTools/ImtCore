const { PropertyAnimation } = require('./PropertyAnimation')
const { Animation } = require('./Animation')
const { QVar } = require('../utils/properties')

class Vector3dAnimation extends PropertyAnimation {

    static defaultProperties = {
        to: { type: QVar, value: undefined },
        from: { type: QVar, value: undefined },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$elapsed = 0
    }

    restart(){
        this.$elapsed = 0
        super.restart()
    }

    tick(){
        const target = this.getPropertyValue('target')
        if(!target || !this.getPropertyValue('running') || this.getPropertyValue('paused')) return

        const propsStr = this.getPropertyValue('properties') || this.getPropertyValue('property')
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const duration = Math.max(1, this.getPropertyValue('duration'))
        this.$elapsed += 1000/60
        const progress = Math.min(1, this.$elapsed / duration)

        for(const prop of props){
            let from = this.getPropertyValue('from')
            let to = this.getPropertyValue('to')
            if(from === undefined) from = target.getPropertyValue(prop)
            if(to === undefined) to = target.getPropertyValue(prop)
            const f = from || {x:0,y:0,z:0}, t = to || {x:0,y:0,z:0}
            const value = {
                x: f.x + (t.x - f.x) * progress,
                y: f.y + (t.y - f.y) * progress,
                z: f.z + (t.z - f.z) * progress,
            }
            if(target.getProperty(prop)) target.getProperty(prop).reset(value)
        }

        if(progress >= 1){
            this.$elapsed = 0
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

module.exports.Vector3dAnimation = Vector3dAnimation
