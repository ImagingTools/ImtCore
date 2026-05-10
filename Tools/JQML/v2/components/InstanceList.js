const { QtObject } = require('./QtObject')
const { QList } = require('../utils/properties')

/**
 * InstanceList - mirrors Qt Quick 3D's InstanceList.
 *
 * Holds a list of InstanceListEntry objects that define per-instance
 * transforms for instanced rendering.
 */
class InstanceList extends QtObject {
    static defaultProperties = {
        instances: { type: QList, value: [], changed: '$instancesChanged' },
    }

    $instancesChanged(){
        if(this.$onInstancesChanged) this.$onInstancesChanged()
    }
}

module.exports.InstanceList = InstanceList
