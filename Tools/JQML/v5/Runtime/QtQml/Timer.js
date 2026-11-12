const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- Timer ------------------------------------------------------------------------

class Timer extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$handle = null
    }

    start(){ this.running = true }
    stop(){ this.running = false }
    restart(){
        this.running = false
        this.running = true
    }

    $schedule(){
        this.$cancel()
        if(!this.$v[Timer.RUNNING] || this.$f & engine.DESTROYED) return
        let interval = Math.max(0, this.$v[Timer.INTERVAL])
        let fire = () => {
            this.$handle = null
            if(engine.isStopped()) return
            if(!this.$v[Timer.RUNNING]) return
            if(!this.$v[Timer.REPEAT]) this.$write(Timer.RUNNING, false)
            engine.batch(() => engine.emitArgs(this, Timer.SIGNAL_triggered, []))
            if(this.$v[Timer.REPEAT] && this.$v[Timer.RUNNING] && !this.$handle) this.$handle = setTimeout(fire, interval)
        }
        this.$handle = setTimeout(fire, interval)
    }

    $cancel(){
        if(this.$handle !== null){
            clearTimeout(this.$handle)
            this.$handle = null
        }
    }

    $disposeNative(){
        this.$cancel()
    }
}

defineType(Timer, {
    props: {
        interval: { kind: 'int', initial: 1000, effect: (timer) => { if(timer.$v[Timer.RUNNING]) timer.$schedule() } },
        running: {
            kind: 'bool',
            effect(timer, running){
                // A Timer that runs from the start fires after the component is complete
                if(running && timer.$v[Timer.TRIGGEREDONSTART]) engine.queueCall(() => engine.emitArgs(timer, Timer.SIGNAL_triggered, []))
                if(running) timer.$schedule()
                else timer.$cancel()
            },
        },
        repeat: 'bool',
        triggeredOnStart: 'bool',
    },
    signals: ['triggered'],
})

module.exports = { Timer }
