const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapItemView extends QtObject {
    static defaultProperties = {
        add: { type: QVar, value: undefined },
        autoFitViewport: { type: QBool, value: undefined },
        delegate: { type: QBool, value: undefined },
        model: { type: QBool, value: undefined },
        remove: { type: QBool, value: undefined },
    }

    // not implemented yet
   
}

module.exports.MapItemView = MapItemView