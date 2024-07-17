const QtObject = require("./QtObject")
const Bool = require("./Bool")
const Var = require("./Var")
const Signal = require("./Signal")

class Connections extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        enabled: { type: Bool, value: true, signalName: 'enabledChanged' },
        target: { type: Var, value: undefined, signalName: 'targetChanged' },
        ignoreUnknownSignals: { type: Bool, value: false, signalName: 'ignoreUnknownSignalsChanged' },

        enabledChanged: {type:Signal, slotName:'onEnabledChanged', args:[]},
        targetChanged: {type:Signal, slotName:'onTargetChanged', args:[]},
        ignoreUnknownSignalsChanged: {type:Signal, slotName:'onIgnoreUnknownSignalsChanged', args:[]},
    })
}

module.exports = Connections