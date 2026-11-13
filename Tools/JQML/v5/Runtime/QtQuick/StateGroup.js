const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { machine } = require('./StateMachine')

// QML StateGroup
class StateGroup extends QtObject {
    constructor(parent = null){
        super(parent)
        engine.scheduleComponentComplete(this)
    }

    $machine(){ return machine(this, StateGroup) }

    $write(id, value){
        if(id === StateGroup.STATE){
            this.$machine().setState(value)
            return
        }
        super.$write(id, value)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$machine().componentComplete()
    }
}

defineType(StateGroup, {
    props: {
        state: 'string',
        states: { kind: 'var', effect(group){ group.$machine().statesChanged() } },
        transitions: 'var',
    },
})

module.exports = { StateGroup }
