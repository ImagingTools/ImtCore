const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

class StateChangeScript extends QtObject {
    $actions(){
        let script = this.$v[StateChangeScript.SCRIPT]
        return [{ event: () => { if(script && script.fn) script.fn.call(this, this, script.scope) } }]
    }
}

defineType(StateChangeScript, { props: { script: 'script', name: 'string' } })

module.exports = { StateChangeScript }
