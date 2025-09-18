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
        loops: { type: Int, value: 1},
        running: { type: Bool, value: false},
        paused: { type: Bool, value: false},
        alwaysRunToEnd: { type: Bool, value: false},

        loopsChanged: { type:Signal, args:[] },
        runningChanged: { type:Signal, args:[] },
        pausedChanged: { type:Signal, args:[] },
        alwaysRunToEndChanged: { type:Signal, args:[] },

        started: { type:Signal, args:[] },
        stopped: { type:Signal, args:[] },
        finished: { type:Signal, args:[] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        AnimationController.add(obj)

        return obj
    }

    __loopCounter = 0

    SLOT_loopsChanged(oldValue, newValue){
        this.__loopCounter = 0
    }
    SLOT_runningChanged(oldValue, newValue){

    }

    SLOT_pausedChanged(oldValue, newValue){

    }

    SLOT_alwaysRunToEndChanged(oldValue, newValue){

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

    __destroy(){
        AnimationController.remove(this)

        super.__destroy()
    }
}



module.exports = Animation