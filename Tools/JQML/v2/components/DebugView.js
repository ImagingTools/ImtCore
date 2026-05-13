const { QtObject } = require('./QtObject')
const { QBool, QVar } = require('../utils/properties')

/**
 * DebugView - mirrors Qt Quick 3D Helpers' DebugView.
 *
 * Provides an on-screen overlay showing render statistics such as
 * FPS, draw calls, and triangle count. On the web this is an Item
 * that reads from RenderStats.
 */
class DebugView extends QtObject {
    static defaultProperties = {
        visible:     { type: QBool, value: true },
        renderStats: { type: QVar,  value: undefined },
        fps:         { type: QVar,  value: 0 },
        drawCalls:   { type: QVar,  value: 0 },
        triangles:   { type: QVar,  value: 0 },
    }
}

module.exports.DebugView = DebugView
