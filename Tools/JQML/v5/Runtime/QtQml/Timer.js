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
        this.$start(Math.max(0, this.$v[Timer.INTERVAL]))
    }

    // the timer expires after interval; a frame that comes first fires it when it is due (engine.runDueTimers)
    $start(interval){
        this.$handle = setTimeout(() => this.$fire(), interval)
        engine.timerScheduled(this, interval)
    }

    $fire(){
        this.$handle = null
        engine.timerDone(this)
        if(engine.isStopped()) return
        if(!this.$v[Timer.RUNNING]) return
        if(!this.$v[Timer.REPEAT]) this.$write(Timer.RUNNING, false)
        engine.batch(() => engine.emitArgs(this, Timer.SIGNAL_triggered, []))
        if(this.$v[Timer.REPEAT] && this.$v[Timer.RUNNING] && this.$handle === null && !(this.$f & engine.DESTROYED)){
            this.$start(Math.max(0, this.$v[Timer.INTERVAL]))
        }
    }

    $fireNow(){
        if(this.$handle !== null){
            clearTimeout(this.$handle)
            this.$handle = null
        }
        this.$fire()
    }

    $cancel(){
        if(this.$handle !== null){
            clearTimeout(this.$handle)
            this.$handle = null
        }
        engine.timerDone(this)
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
