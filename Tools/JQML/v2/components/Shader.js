const { QtObject } = require('./QtObject')
const { QString, QReal } = require('../utils/properties')

/**
 * Shader - mirrors Qt Quick 3D's Shader.
 *
 * References a GLSL shader source file and its stage (vertex or
 * fragment) for use inside a Pass.
 */
class Shader extends QtObject {
    static Vertex   = 0
    static Fragment = 1

    static defaultProperties = {
        source: { type: QString, value: '' },
        stage:  { type: QReal,   value: 0 },
    }
}

module.exports.Shader = Shader
