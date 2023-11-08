const { QtObject } = require('./QtObject')
const { QPoint, QBool, QVar, QReal } = require('../utils/properties')

class MapQuickItem extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        coordinate: { type: QVar, value: undefined },
        sourceItem: { type: QVar, value: undefined },
        visible: { type: QBool, value: true },
        zoomLevel: { type: QReal, value: 0 },
        anchorPoint: { type: QPoint },

    }

    // not implemented yet
   
}

module.exports.MapQuickItem = MapQuickItem