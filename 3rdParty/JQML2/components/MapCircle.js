const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapCircle extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        center: { type: QBool, value: undefined },
        color: { type: QColor, value: 'transparent' },
        opacity: { type: QReal, value: 1 },
        radius: { type: QReal, value: 0 },
        border: { type: QBorder },

    }

    // not implemented yet
   
}

module.exports.MapCircle = MapCircle