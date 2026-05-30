const QtObject = require("../QtQml/QtObject")
const Animation = require("./Animation")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const List = require("../QtQml/List")
const Signal = require("../QtQml/Signal")

// Transition holds a list of animations to run during state changes.
// Without a State system the transition can be triggered manually via start().

class Transition extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        from: { type: String, value: '*' },
        to: { type: String, value: '*' },
        reversible: { type: Bool, value: false },
        enabled: { type: Bool, value: true },
        running: { type: Bool, value: false },
        animations: { type: List, value: [] },

        fromChanged: { type: Signal, args: [] },
        toChanged: { type: Signal, args: [] },
        reversibleChanged: { type: Signal, args: [] },
        enabledChanged: { type: Signal, args: [] },
        runningChanged: { type: Signal, args: [] },
        animationsChanged: { type: Signal, args: [] },

        started: { type: Signal, args: [] },
        stopped: { type: Signal, args: [] },
        finished: { type: Signal, args: [] },
    })

    __childAnimations(){
        const arr = this.animations || []
        if(arr.length) return arr.filter(o => o instanceof Animation)
        return (this.__children || []).filter(o => o instanceof Animation)
    }

    start(){
        if(!this.enabled) return
        const kids = this.__childAnimations()
        if(!kids.length){
            this.finished()
            return
        }
        this.running = true
        this.started()
        let pending = kids.length
        for(const k of kids){
            const onStop = () => {
                pending--
                if(pending <= 0){
                    this.running = false
                    this.stopped()
                    this.finished()
                }
            }
            try {
                if(k.finished && typeof k.finished.connect === 'function') k.finished.connect(onStop)
            } catch(e) {}
            k.start()
        }
    }

    stop(){
        for(const k of this.__childAnimations()) k.stop()
        this.running = false
        this.stopped()
    }
}

module.exports = Transition
