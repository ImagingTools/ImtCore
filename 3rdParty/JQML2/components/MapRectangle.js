const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapRectangle extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        topLeft: { type: QVar, value: undefined },
        bottomRight: { type: QVar, value: undefined }, 
        color: { type: QColor, value: 'transparent' },
        opacity: { type: QReal, value: 1 },
        border: { type: QBorder },

    }

    // not implemented yet
   
}

module.exports.MapRectangle = MapRectangle