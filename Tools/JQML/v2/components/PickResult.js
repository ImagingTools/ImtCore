const { QtObject } = require('./QtObject')
const { QReal, QVar, QVector3D } = require('../utils/properties')

/**
 * PickResult - mirrors Qt Quick 3D's PickResult.
 *
 * Read-only data object returned by View3D.pick(). Holds the hit
 * position, normal, UV, distance, and a reference to the picked object.
 */
class PickResult extends QtObject {
    static defaultProperties = {
        objectHit:    { type: QVar,      value: undefined },
        position:     { type: QVector3D, value: { x: 0, y: 0, z: 0 } },
        normal:       { type: QVector3D, value: { x: 0, y: 0, z: 0 } },
        uvPosition:   { type: QVar,      value: { x: 0, y: 0 } },
        distance:     { type: QReal,     value: 0 },
        scenePosition:{ type: QVector3D, value: { x: 0, y: 0, z: 0 } },
    }
}

module.exports.PickResult = PickResult
