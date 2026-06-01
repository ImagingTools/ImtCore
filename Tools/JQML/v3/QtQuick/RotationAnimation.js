const PropertyAnimation = require("./PropertyAnimation")
const Animation = require("./Animation")
const Real = require("../QtQml/Real")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")

class RotationAnimation extends PropertyAnimation {

    static Numerical = 0
    static Shortest = 1
    static Clockwise = 2
    static Counterclockwise = 3

    static meta = Object.assign({}, PropertyAnimation.meta, {
        to: { type: Real, value: 0 },
        from: { type: Real, value: 0 },
        direction: { type: Int, value: 0 },

        directionChanged: { type: Signal, args: [] },
    })

    __tick(){
        if(!this.target || !this.running || this.paused) return

        const propsStr = this.properties || this.property
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const duration = Math.max(1, this.duration)
        const direction = this.direction

        for(const prop of props){
            let from = this.from
            let to = this.to
            if(from === undefined) { from = this.target[prop]; this.from = from }
            if(to === undefined) { to = this.target[prop]; this.to = to }

            let delta = to - from
            if(direction === RotationAnimation.Shortest){
                while(delta > 180) delta -= 360
                while(delta < -180) delta += 360
            } else if(direction === RotationAnimation.Clockwise){
                if(delta < 0) delta += 360
            } else if(direction === RotationAnimation.Counterclockwise){
                if(delta > 0) delta -= 360
            }

            const increment = delta / (duration / (1000/60))
            const cur = this.target[prop]
            const value = cur + increment
            const done = (increment >= 0 && value >= from + delta) || (increment <= 0 && value <= from + delta)
            if(done){
                this.target[prop] = from + delta
                const loops = this.__loopCounter + 1
                if(loops < this.loops || this.loops === Animation.Infinite){
                    this.restart()
                    this.__loopCounter = loops
                } else {
                    this.running = false
                    this.stopped()
                    this.finished()
                }
            } else {
                this.target[prop] = value
            }
        }
    }
}

module.exports = RotationAnimation
