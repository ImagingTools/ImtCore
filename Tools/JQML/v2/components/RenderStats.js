const { QtObject } = require('./QtObject')
const { QReal } = require('../utils/properties')

/**
 * RenderStats - mirrors Qt Quick 3D's RenderStats.
 *
 * Read-only statistics about the most recent frame rendered by
 * View3D. Updated automatically each frame.
 */
class RenderStats extends QtObject {
    static defaultProperties = {
        fps:            { type: QReal, value: 0 },
        frameTime:      { type: QReal, value: 0 },
        renderTime:     { type: QReal, value: 0 },
        syncTime:       { type: QReal, value: 0 },
        maxFrameTime:   { type: QReal, value: 0 },
        drawCalls:      { type: QReal, value: 0 },
        triangles:      { type: QReal, value: 0 },
    }
}

module.exports.RenderStats = RenderStats
