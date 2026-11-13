const { defineType, makeReadonly } = require('../Core/type')
const { STOPPED } = require('./Animation')
const { ParallelAnimation } = require('./ParallelAnimation')

// ---- Transition -----------------------------------------------------------------------------

// Its animations run in parallel ("the top-level animations of a Transition are run in parallel").
// Reversed transitions run forwards here.
class Transition extends ParallelAnimation {
    constructor(parent = null){
        super(parent)
        this.$userControlDisabled = true
    }

    $prepare(actions, touched, manager, defaultTarget){
        if(this.$state !== STOPPED){
            this.$listener = null
            this.$stopJob()
        }
        this.$transition(actions, touched, defaultTarget)
        this.$listener = manager
        this.$startJob()
    }
}

defineType(Transition, {
    props: {
        from: { kind: 'string', initial: '*' },
        to: { kind: 'string', initial: '*' },
        reversible: 'bool',
        enabled: { kind: 'bool', initial: true },
    },
})

// read-only in Qt, writable in the base class
makeReadonly(Transition, 'running')

module.exports = { Transition }
