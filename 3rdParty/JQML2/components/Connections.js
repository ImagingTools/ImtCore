const { QtObject } = require('./QtObject')
const { QVar, QBool } = require('../utils/properties')

class Connections extends QtObject {
    static defaultProperties = {
        target: { type: QVar, value: undefined },
        enabled: { type: QBool, value: true },
        ignoreUnknownSignals: { type: QBool, value: false },

    }
    
}

module.exports.Connections = Connections