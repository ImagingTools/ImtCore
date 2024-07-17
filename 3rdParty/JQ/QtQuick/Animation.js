const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")

class Animation extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        loops: { type: Int, value: 1, signalName: 'loopsChanged' },
        running: { type: Bool, value: false, signalName: 'runningChanged' },
        paused: { type: Bool, value: false, signalName: 'pausedChanged' },
        alwaysRunToEnd: { type: Bool, value: false, signalName: 'alwaysRunToEndChanged' },

        loopsChanged: { type:Signal, slotName:'onLoopsChanged', args:[] },
        runningChanged: { type:Signal, slotName:'onRunningChanged', args:[] },
        pausedChanged: { type:Signal, slotName:'onPausedChanged', args:[] },
        alwaysRunToEndChanged: { type:Signal, slotName:'onAlwaysRunToEndChanged', args:[] },

        started: { type:Signal, slotName:'onStarted', args:[] },
        stopped: { type:Signal, slotName:'onStopped', args:[] },
        finished: { type:Signal, slotName:'onFinished', args:[] },
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        let self = proxy.__self 

        return proxy
    }


    pause() {

    }
    start() {

    }
    stop() {

    }
    resume() {

    }
    restart() {

    }
 
}

module.exports = Animation