const { QtObject } = require('./QtObject')
const { QString, QReal, QVar } = require('../utils/properties')

/**
 * Buffer - mirrors Qt Quick 3D's Buffer.
 *
 * Defines a render buffer (color / depth / stencil) used within
 * multi-pass rendering pipelines.
 */
class Buffer extends QtObject {
    static defaultProperties = {
        name:           { type: QString, value: '' },
        format:         { type: QReal,   value: 0 },
        textureFilterOperation: { type: QReal, value: 0 },
        textureCoordOperation:  { type: QReal, value: 0 },
        sizeMultiplier: { type: QReal,   value: 1 },
        bufferFlags:    { type: QReal,   value: 0 },
    }
}

module.exports.Buffer = Buffer
