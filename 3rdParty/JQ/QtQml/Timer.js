const QtObject = require("./QtObject")
const Bool = require("./Bool")
const Real = require("./Real")
const Signal = require("./Signal")

class Timer extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        interval: { type: Real, value: 1000},
        repeat: { type: Bool, value: false},
        running: { type: Bool, value: false},
        triggeredOnStart: { type: Bool, value: false},

        intervalChanged: {type:Signal, args:[]},
        repeatChanged: {type:Signal, args:[]},
        runningChanged: {type:Signal, args:[]},
        triggeredOnStartChanged: {type:Signal, args:[]},

        triggered: {type:Signal, args:[]},
    })

    __timer = null

    __complete(){
        super.__complete()
        if(this.triggeredOnStart) this.triggered()
    }

    restart(){
        this.stop()
        this.start()
    }
    
    start(){
        if(!this.running) this.running = true
    }

    stop(){
        if(this.running) this.running = false
    }

    SLOT_intervalChanged(oldValue, newValue){
        this.__updateTimer()
    }

    SLOT_repeatChanged(oldValue, newValue){
        this.__updateTimer()
    }

    SLOT_runningChanged(oldValue, newValue){
        this.__updateTimer()
    }

    SLOT_triggeredOnStartChanged(oldValue, newValue){
        this.__updateTimer()
    }

    __triggered(){
        if(this.__destroying || this.__destroyed){
            clearTimeout(this.__timer)
            return
        }

        this.triggered()
        if(this.repeat && this.running) {
            if(this.__timer) clearTimeout(this.__timer)
            this.__timer = setTimeout(()=>{
                this.__triggered()
            }, this.interval)
        } else {
            this.stop()
        }
    }

    __updateTimer(){
        if(this.__timer) clearTimeout(this.__timer)
        if(this.triggeredOnStart && this.__completed) this.triggered()

        if(this.running)
        this.__timer = setTimeout(()=>{
            this.__triggered()
        }, this.interval)
    }

    __destroy(){
        clearTimeout(this.__timer)
        delete this.__timer
        super.__destroy()
    }
}



module.exports = Timer