const { QtObject } = require('./QtObject')
const { Animation } = require('./Animation')
const { QBool, QString, QList } = require('../utils/properties')

// Transition holds a list of animations to run during state changes.
// Without a State system, the transition can be triggered manually via start().

class Transition extends QtObject {

    static defaultProperties = {
        from: { type: QString, value: '*' },
        to: { type: QString, value: '*' },
        reversible: { type: QBool, value: false },
        enabled: { type: QBool, value: true },
        running: { type: QBool, value: false, changed: '$runningChanged' },
        animations: { type: QList },
    }

    static defaultSignals = {
        started: { params: [] },
        stopped: { params: [] },
        finished: { params: [] },
    }

    $childAnimations(){
        let arr = this.getPropertyValue('animations') || []
        if(arr.length) return arr.filter(o => o instanceof Animation)
        const data = this.getPropertyValue('data') || []
        return data.filter(o => o instanceof Animation)
    }

    start(){
        if(!this.getPropertyValue('enabled')) return
        const kids = this.$childAnimations()
        if(!kids.length){
            if(this.$signals.finished) this.$signals.finished()
            return
        }
        this.getProperty('running').reset(true)
        if(this.$signals.started) this.$signals.started()
        let pending = kids.length
        for(const k of kids){
            const onStop = () => {
                pending--
                if(pending <= 0){
                    this.getProperty('running').reset(false)
                    if(this.$signals.stopped) this.$signals.stopped()
                    if(this.$signals.finished) this.$signals.finished()
                }
            }
            const sig = k.$signals && k.$signals.finished
            if(sig && typeof sig.connect === 'function') sig.connect(onStop)
            k.start()
        }
    }

    stop(){
        for(const k of this.$childAnimations()) k.stop()
        this.getProperty('running').reset(false)
        if(this.$signals.stopped) this.$signals.stopped()
    }

    $runningChanged(){}
}

module.exports.Transition = Transition
