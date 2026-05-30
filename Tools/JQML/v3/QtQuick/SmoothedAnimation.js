const NumberAnimation = require("./NumberAnimation")
const Animation = require("./Animation")
const Real = require("../QtQml/Real")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")

class SmoothedAnimation extends NumberAnimation {

    static Eased = 0
    static Immediate = 1
    static Sync = 2

    static meta = Object.assign({}, NumberAnimation.meta, {
        velocity: { type: Real, value: 200 },
        reversingMode: { type: Int, value: 0 },
        maximumEasingTime: { type: Real, value: -1 },

        velocityChanged: { type: Signal, args: [] },
        reversingModeChanged: { type: Signal, args: [] },
        maximumEasingTimeChanged: { type: Signal, args: [] },
    })

    __tick(){
        if(!this.target || !this.running || this.paused) return
        const propsStr = this.properties || this.property
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const step = this.velocity / 60

        for(const prop of props){
            const to = this.to
            const cur = this.target[prop]
            const diff = to - cur
            if(Math.abs(diff) <= step){
                this.target[prop] = to
                const loops = this.__loopCounter + 1
                if(loops < this.loops || this.loops === Animation.Infinite){
                    this.__loopCounter = loops
                } else {
                    this.running = false
                    this.stopped()
                    this.finished()
                }
            } else {
                this.target[prop] = cur + Math.sign(diff) * step
            }
        }
    }
}

module.exports = SmoothedAnimation
