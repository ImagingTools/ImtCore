const { QtObject } = require('./QtObject')
const { QVector3D, QColor } = require('../utils/properties')

/**
 * InstanceListEntry - mirrors Qt Quick 3D's InstanceListEntry.
 *
 * Describes the transform and color for a single instance inside
 * an InstanceList.
 */
class InstanceListEntry extends QtObject {
    static defaultProperties = {
        position:      { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$entryChanged' },
        eulerRotation: { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$entryChanged' },
        scale:         { type: QVector3D, value: { x: 1, y: 1, z: 1 }, changed: '$entryChanged' },
        color:         { type: QColor,    value: '#ffffffff',           changed: '$entryChanged' },
    }

    $entryChanged(){
        if(this.$onEntryChanged) this.$onEntryChanged()
    }
}

module.exports.InstanceListEntry = InstanceListEntry
