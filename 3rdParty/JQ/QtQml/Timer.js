const QtObject = require("./QtObject")
const Bool = require("./Bool")
const Real = require("./Real")
const Signal = require("./Signal")

class Timer extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        interval: { type: Real, value: 1000, signalName: 'intervalChanged' },
        repeat: { type: Bool, value: false, signalName: 'repeatChanged' },
        running: { type: Bool, value: false, signalName: 'runningChanged' },
        triggeredOnStart: { type: Bool, value: false, signalName: 'triggeredOnStartChanged' },

        intervalChanged: {type:Signal, slotName:'onIntervalChanged', args:[]},
        repeatChanged: {type:Signal, slotName:'onRepeatChanged', args:[]},
        runningChanged: {type:Signal, slotName:'onRunningChanged', args:[]},
        triggeredOnStartChanged: {type:Signal, slotName:'onTriggeredOnStartChanged', args:[]},

        triggered: {type:Signal, slotName:'onTriggered', args:[]},
    })

    restart(){
        this.stop()
        this.start()
    }
    start(){
        if(!this.getPropertyValue('running')) this.getProperty('running').reset(true)
    }
    stop(){
        if(this.getPropertyValue('running')) this.getProperty('running').reset(false)
    }
}

module.exports = Timer