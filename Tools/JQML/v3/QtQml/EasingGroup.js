const GroupProperty = require("./GroupProperty")
const Int = require("./Int")
const Real = require("./Real")
const Signal = require("./Signal")

class EasingGroup extends GroupProperty {
    static meta = {
        type: { type: Int, value: 0 },
        amplitude: { type: Real, value: 1.0 },
        overshoot: { type: Real, value: 1.70158 },
        period: { type: Real, value: 0.3 },

        typeChanged: { type: Signal, args: [] },
        amplitudeChanged: { type: Signal, args: [] },
        overshootChanged: { type: Signal, args: [] },
        periodChanged: { type: Signal, args: [] },
    }
}

module.exports = EasingGroup
