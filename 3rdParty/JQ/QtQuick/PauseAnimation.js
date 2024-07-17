const Animation = require("./Animation")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class PauseAnimation extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        duration: { type: Real, value: 250, signalName: 'durationChanged' },

        durationChanged: { type:Signal, slotName:'onDurationChanged', args:[] },
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        let self = proxy.__self 

        return proxy
    }

    restart() {

    }
 
}

module.exports = PauseAnimation