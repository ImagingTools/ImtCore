const PropertyAnimation = require("./PropertyAnimation")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class NumberAnimation extends PropertyAnimation {
    static meta = Object.assign({}, PropertyAnimation.meta, {
        to: { type: Real, value: 0, signalName: 'toChanged' },
        from: { type: Real, value: 0, signalName: 'fromChanged' },

        toChanged: { type:Signal, slotName:'onToChanged', args:[] },
        fromChanged: { type:Signal, slotName:'onFromChanged', args:[] },
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        let self = proxy.__self 

        return proxy
    }

    restart() {

    }
 
}

module.exports = NumberAnimation