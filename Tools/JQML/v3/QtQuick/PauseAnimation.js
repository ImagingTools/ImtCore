const Animation = require("./Animation")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class PauseAnimation extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        duration: { type: Real, value: 250},

        durationChanged: { type:Signal, args:[] },
    })

    __duration = 250
 
    SLOT_durationChanged() {
        this.__duration = this.duration
    }

    restart() { 
        this.__duration = this.duration
        super.restart()
    }

    __tick(){
        if(this.running && !this.paused && this.__duration > 0){
            this.__duration -= 1000/60

            if(this.__duration <= 0) {
                this.running = false
                this.finished()
            }
        }
    }
}



module.exports = PauseAnimation