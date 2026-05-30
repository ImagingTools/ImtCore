const PropertyAnimation = require("./PropertyAnimation")
const Animation = require("./Animation")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class Vector3dAnimation extends PropertyAnimation {

    static meta = Object.assign({}, PropertyAnimation.meta, {
        to: { type: Var, value: undefined },
        from: { type: Var, value: undefined },

        toChanged: { type: Signal, args: [] },
        fromChanged: { type: Signal, args: [] },
    })

    __elapsed = 0

    restart(){
        this.__elapsed = 0
        super.restart()
    }

    __tick(){
        if(!this.target || !this.running || this.paused) return
        const propsStr = this.properties || this.property
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const duration = Math.max(1, this.duration)
        this.__elapsed += 1000/60
        const progress = Math.min(1, this.__elapsed / duration)

        for(const prop of props){
            let from = this.from
            let to = this.to
            if(from === undefined) from = this.target[prop]
            if(to === undefined) to = this.target[prop]
            const f = from || {x:0,y:0,z:0}, t = to || {x:0,y:0,z:0}
            try {
                this.target[prop] = {
                    x: f.x + (t.x - f.x) * progress,
                    y: f.y + (t.y - f.y) * progress,
                    z: f.z + (t.z - f.z) * progress,
                }
            } catch(e) {}
        }

        if(progress >= 1){
            this.__elapsed = 0
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

module.exports = Vector3dAnimation
