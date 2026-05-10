const { QtObject } = require('./QtObject')
const { QString, QVar, QList } = require('../utils/properties')

/**
 * Pass - mirrors Qt Quick 3D's Pass.
 *
 * Describes a single render pass inside an Effect pipeline, including
 * which shaders to use and which buffers to read/write.
 */
class Pass extends QtObject {
    static defaultProperties = {
        shaders:  { type: QList, value: [] },
        output:   { type: QVar,  value: undefined },
        commands: { type: QList, value: [] },
    }
}

module.exports.Pass = Pass
