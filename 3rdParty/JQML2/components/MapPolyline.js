const { QtObject } = require('./QtObject')
const { QLine, QBool, QVar } = require('../utils/properties')

class MapPolyline extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        path: { type: QVar, value: [] },
        line: { type: QLine },

    }

    // not implemented yet
   
}

module.exports.MapPolyline = MapPolyline