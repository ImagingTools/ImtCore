const { QtObject } = require('./QtObject')
const { QReal, QVector3D, QColor } = require('../utils/properties')

/**
 * InstanceRange - mirrors Qt Quick 3D Helpers' RandomInstancing.
 *
 * Generates random per-instance transforms within user-defined ranges.
 * The web backend pre-computes instance matrices when attached.
 */
class InstanceRange extends QtObject {
    static defaultProperties = {
        instanceCount: { type: QReal,     value: 0,                     changed: '$rangeChanged' },
        randomSeed:    { type: QReal,     value: 0,                     changed: '$rangeChanged' },
        positionMin:   { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$rangeChanged' },
        positionMax:   { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$rangeChanged' },
        rotationMin:   { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$rangeChanged' },
        rotationMax:   { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$rangeChanged' },
        scaleMin:      { type: QVector3D, value: { x: 1, y: 1, z: 1 }, changed: '$rangeChanged' },
        scaleMax:      { type: QVector3D, value: { x: 1, y: 1, z: 1 }, changed: '$rangeChanged' },
        colorMin:      { type: QColor,    value: '#ffffffff',           changed: '$rangeChanged' },
        colorMax:      { type: QColor,    value: '#ffffffff',           changed: '$rangeChanged' },
    }

    $rangeChanged(){
        if(this.$onRangeChanged) this.$onRangeChanged()
    }
}

module.exports.InstanceRange = InstanceRange
