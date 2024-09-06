const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")

const AnimationController = {
    animations: [],
    timer: null,

    update: function(){
        if(this.animations.length){
            if(!this.timer) {
                this.timer = setInterval(()=>{
                    for(let animation of this.animations){
                        animation.__tick()
                    }
                }, 1000 / 60)
            }
        } else {
            if(this.timer) {
                clearInterval(this.timer)
                this.timer = null
            }
        }
    },

    add: function(obj){
        if(this.animations.indexOf(obj) < 0) this.animations.push(obj)
        this.update()
    },

    remove: function(obj){
        let index = this.animations.indexOf(obj)
        if(index >= 0) this.animations.splice(index, 1)
        this.update()
    },
}

class Animation extends QtObject {
    static Infinite = -1
    
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

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)

        AnimationController.add(proxy)

        return proxy
    }

    __loopCounter = 0

    onLoopsChanged(){
        this.__loopCounter = 0
    }
    onRunningChanged(){

    }

    onPausedChanged(){

    }

    onAlwaysRunToEndChanged(){

    }

    pause() {
        this.paused = true
    }
    start() {
        this.restart()
    }
    stop() {
        this.running = false
        this.stopped()
    }
    resume() {
        this.paused = false
    }
    restart() {
        this.__loopCounter = 0
        this.paused = false
        this.running = true
        this.started()
    }
 
    __tick() {

    }
}

module.exports = Animation