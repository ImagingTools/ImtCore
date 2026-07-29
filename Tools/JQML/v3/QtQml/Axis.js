const GroupProperty = require("./GroupProperty")
const Bool = require("./Bool")
const Real = require("./Real")
const Signal = require("./Signal")

class Axis extends GroupProperty {
    static meta = {
        enabled: { type: Bool, value: true },
        minimum: { type: Real, value: -Infinity },
        maximum: { type: Real, value: Infinity },
        activeValue: { type: Real, value: 0 },

        enabledChanged: { type: Signal, args: [] },
        minimumChanged: { type: Signal, args: [] },
        maximumChanged: { type: Signal, args: [] },
        activeValueChanged: { type: Signal, args: [] },
    }
}

module.exports = Axis