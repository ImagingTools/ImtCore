const { defineType } = require('../Core/type')
const { Animation } = require('./Animation')

// ---- PauseAnimation -------------------------------------------------------------------------

class PauseAnimation extends Animation {
    $duration(){ return this.$v[PauseAnimation.DURATION] }
}

defineType(PauseAnimation, { props: { duration: { kind: 'int', initial: 250 } } })

module.exports = { PauseAnimation }
