const { Item } = require('./Item')
const { QReal, QBool, MapGestureArea, QVar } = require('../utils/properties')
const { QtPositioning } = require('../utils/QtPositioning')

class Map extends Item {
    static defaultProperties = {
        plugin: { type: QVar, value: '' },
        center: { type: QVar, value: QtPositioning.coordinate(0, 0) },
        zoomLevel: { type: QReal, value: 2 },
        copyrightsVisible: { type: QBool, value: true },
        bearing: { type: QReal, value: 0 },
        mapReady: { type: QBool, value: false },
        gesture: { type: MapGestureArea },

    }

    // not implemented yet
 
}

module.exports.Map = Map