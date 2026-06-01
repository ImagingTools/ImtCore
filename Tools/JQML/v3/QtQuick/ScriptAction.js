const Animation = require("./Animation")
const Var = require("../QtQml/Var")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class ScriptAction extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        script: { type: Var, value: undefined },
        scriptName: { type: String, value: '' },

        scriptChanged: { type: Signal, args: [] },
        scriptNameChanged: { type: Signal, args: [] },
    })

    __tick(){
        if(!this.running) return
        const s = this.script
        try {
            if(typeof s === 'function') s.call(this)
        } catch(e) {
            console.warn('ScriptAction error:', e)
        }
        this.running = false
        this.stopped()
        this.finished()
    }
}

module.exports = ScriptAction
