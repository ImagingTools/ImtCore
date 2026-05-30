const { Animation } = require('./Animation')
const { QVar, QString } = require('../utils/properties')

class ScriptAction extends Animation {

    static defaultProperties = {
        script: { type: QVar, value: undefined },
        scriptName: { type: QString, value: '' },
    }

    tick(){
        if(!this.getPropertyValue('running')) return
        const s = this.getPropertyValue('script')
        try {
            if(typeof s === 'function') s.call(this)
        } catch(e) {
            console.warn('ScriptAction error:', e)
        }
        this.getProperty('running').reset(false)
        if(this.$signals.stopped) this.$signals.stopped()
        if(this.$signals.finished) this.$signals.finished()
    }
}

module.exports.ScriptAction = ScriptAction
