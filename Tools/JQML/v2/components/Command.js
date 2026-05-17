const { QtObject } = require('./QtObject')
const { QString, QVar } = require('../utils/properties')

/**
 * Command - mirrors Qt Quick 3D's Command.
 *
 * A render command executed during a Pass (e.g. setting a uniform,
 * binding a buffer, or changing render state).
 */
class Command extends QtObject {
    static defaultProperties = {
        name:       { type: QString, value: '' },
        value:      { type: QVar,    value: undefined },
        destination:{ type: QVar,    value: undefined },
    }
}

module.exports.Command = Command
