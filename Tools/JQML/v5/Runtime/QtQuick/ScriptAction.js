const { defineType } = require('../Core/type')
const { Animation } = require('./Animation')

class ScriptAction extends Animation {
    $updateCurrentTime(){
        let script = this.$v[ScriptAction.SCRIPT]
        if(script && script.fn) script.fn.call(this, this, script.scope)
    }
}
defineType(ScriptAction, { props: { script: 'script', scriptName: 'string' } })

module.exports = { ScriptAction }
