const { SceneEnvironment } = require('./SceneEnvironment')
const { QReal, QBool, QVar, QColor } = require('../utils/properties')

/**
 * ExtendedSceneEnvironment - mirrors Qt Quick 3D Helpers'
 * ExtendedSceneEnvironment.
 *
 * Extends SceneEnvironment with extra post-processing controls such
 * as glow, depth-of-field, color adjustments, and vignette.
 */
class ExtendedSceneEnvironment extends SceneEnvironment {
    static defaultProperties = {
        ...SceneEnvironment.defaultProperties,
        glowEnabled:      { type: QBool,  value: false, changed: '$envChanged' },
        glowStrength:     { type: QReal,  value: 1,     changed: '$envChanged' },
        glowBloom:        { type: QReal,  value: 0,     changed: '$envChanged' },
        depthOfFieldEnabled: { type: QBool, value: false, changed: '$envChanged' },
        depthOfFieldFocusDistance: { type: QReal, value: 600, changed: '$envChanged' },
        depthOfFieldFocusRange:   { type: QReal, value: 100, changed: '$envChanged' },
        depthOfFieldBlurAmount:   { type: QReal, value: 4,   changed: '$envChanged' },
        vignetteEnabled:  { type: QBool,  value: false, changed: '$envChanged' },
        vignetteStrength: { type: QReal,  value: 0.15,  changed: '$envChanged' },
        vignetteColor:    { type: QColor, value: '#000000ff', changed: '$envChanged' },
        colorAdjustmentsEnabled: { type: QBool, value: false, changed: '$envChanged' },
        adjustmentBrightness:    { type: QReal, value: 1,     changed: '$envChanged' },
        adjustmentContrast:      { type: QReal, value: 1,     changed: '$envChanged' },
        adjustmentSaturation:    { type: QReal, value: 1,     changed: '$envChanged' },
    }
}

module.exports.ExtendedSceneEnvironment = ExtendedSceneEnvironment
