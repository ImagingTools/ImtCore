const PropertyAnimation = require("./PropertyAnimation")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class NumberAnimation extends PropertyAnimation {
    static meta = Object.assign({}, PropertyAnimation.meta, {
        to: { type: Real, value: 0},
        from: { type: Real, value: 0},

        toChanged: { type:Signal, slotName:'onToChanged', args:[] },
        fromChanged: { type:Signal, slotName:'onFromChanged', args:[] },
    })
}



module.exports = NumberAnimation