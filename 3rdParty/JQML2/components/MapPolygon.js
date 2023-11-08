const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapPolygon extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        path: { type: QVar, value: [] },
        color: { type: QColor, value: 'transparent' },
        opacity: { type: QReal, value: 1 },
        border: { type: QBorder },

    }

    // not implemented yet
   
}

module.exports.MapPolygon = MapPolygon