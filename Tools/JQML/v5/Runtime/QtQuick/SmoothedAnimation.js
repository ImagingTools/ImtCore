const { defineType } = require('../Core/type')
const { readProperty } = require('./Animation')
const { NumberAnimation } = require('./NumberAnimation')
const { PropertyAnimation } = require('./PropertyAnimation')

// SmoothedAnimation: "If both velocity and duration are specified, the one that results in the
// quickest animation is chosen"; the velocity profile is approximated by InOutQuad.
class SmoothedAnimation extends NumberAnimation {
    $duration(){
        let velocity = this.$v[SmoothedAnimation.VELOCITY]
        let duration = this.$v[PropertyAnimation.DURATION]
        let distance = 0
        for(let action of this.$actions){
            let from = action.from !== undefined ? action.from : readProperty(action.p)
            distance = Math.max(distance, Math.abs(Number(action.to) - Number(from)))
        }
        let byVelocity = velocity > 0 ? distance / velocity * 1000 : -1
        if(duration < 0) return Math.max(0, byVelocity)
        if(byVelocity < 0) return duration
        return Math.min(duration, byVelocity)
    }

    constructor(parent = null){
        super(parent)
        this.$v[PropertyAnimation.DURATION] = -1
    }
}
defineType(SmoothedAnimation, {
    props: {
        velocity: { kind: 'real', initial: 200 },
        reversingMode: 'int',
        maximumEasingTime: { kind: 'real', initial: -1 },
    },
})
Object.assign(SmoothedAnimation, { Eased: 0, Immediate: 1, Sync: 2 })

module.exports = { SmoothedAnimation }
