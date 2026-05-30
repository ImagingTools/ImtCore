const NumberAnimation = require("./NumberAnimation")
const Animation = require("./Animation")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class SpringAnimation extends NumberAnimation {

    static meta = Object.assign({}, NumberAnimation.meta, {
        spring: { type: Real, value: 0 },
        damping: { type: Real, value: 0 },
        epsilon: { type: Real, value: 0.01 },
        modulus: { type: Real, value: 0 },
        mass: { type: Real, value: 1 },
        velocity: { type: Real, value: 0 },

        springChanged: { type: Signal, args: [] },
        dampingChanged: { type: Signal, args: [] },
        epsilonChanged: { type: Signal, args: [] },
        modulusChanged: { type: Signal, args: [] },
        massChanged: { type: Signal, args: [] },
        velocityChanged: { type: Signal, args: [] },
    })

    __vel = 0

    restart(){
        this.__vel = 0
        super.restart()
    }

    __tick(){
        if(!this.target || !this.running || this.paused) return
        const propsStr = this.properties || this.property
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const k = this.spring
        const d = this.damping
        const m = Math.max(0.0001, this.mass)
        const eps = this.epsilon
        const dt = 1/60

        for(const prop of props){
            const to = this.to
            const cur = this.target[prop]
            const force = k * (to - cur) - d * this.__vel
            this.__vel += (force / m) * dt
            const next = cur + this.__vel * dt
            this.target[prop] = next

            if(Math.abs(next - to) < eps && Math.abs(this.__vel) < eps){
                this.target[prop] = to
                this.__vel = 0
                const loops = this.__loopCounter + 1
                if(loops < this.loops || this.loops === Animation.Infinite){
                    this.__loopCounter = loops
                } else {
                    this.running = false
                    this.stopped()
                    this.finished()
                }
            }
        }
    }
}

module.exports = SpringAnimation
