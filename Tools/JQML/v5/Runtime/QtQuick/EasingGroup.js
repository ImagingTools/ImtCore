const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { ease } = require('./Easing')

// ---- Easing group ---------------------------------------------------------------------------

class EasingGroup extends GroupObject {
    $apply(t){
        let v = this.$v
        return ease(v[EasingGroup.TYPE], t, v[EasingGroup.AMPLITUDE], v[EasingGroup.OVERSHOOT], v[EasingGroup.PERIOD], v[EasingGroup.BEZIERCURVE])
    }
}

defineType(EasingGroup, {
    props: {
        type: 'int',
        amplitude: { kind: 'real', initial: 1 },
        overshoot: { kind: 'real', initial: 1.70158 },
        period: { kind: 'real', initial: 0.3 },
        bezierCurve: 'var',
    },
})

module.exports = { EasingGroup }
