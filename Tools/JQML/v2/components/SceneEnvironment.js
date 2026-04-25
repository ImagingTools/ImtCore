const { QtObject } = require('./QtObject')
const { QColor, QReal, QVar } = require('../utils/properties')

/**
 * SceneEnvironment - mirrors Qt Quick 3D's SceneEnvironment.
 *
 * Controls the background, ambient lighting (light probe / IBL), and
 * basic post-processing. View3D calls $applyToScene() whenever its
 * `environment` property changes.
 */
class SceneEnvironment extends QtObject {
    static Color       = 0
    static SkyBox      = 1
    static Transparent = 2

    static NoTonemap   = 0
    static TonemapModeLinear = 1
    static TonemapModeAces   = 2
    static TonemapModeFilmic = 3

    static NoAA        = 0
    static SSAA        = 1
    static MSAA        = 2

    static Medium      = 0
    static High        = 1
    static VeryHigh    = 2

    static defaultProperties = {
        clearColor:        { type: QColor, value: '#000000ff', changed: '$envChanged' },
        backgroundMode:    { type: QReal,  value: SceneEnvironment.Color, changed: '$envChanged' },
        lightProbe:        { type: QVar,   value: undefined,   changed: '$envChanged' },
        tonemapMode:       { type: QReal,  value: SceneEnvironment.TonemapModeLinear, changed: '$envChanged' },
        antialiasingMode:  { type: QReal,  value: SceneEnvironment.NoAA, changed: '$envChanged' },
        antialiasingQuality:{ type: QReal, value: SceneEnvironment.Medium, changed: '$envChanged' },
    }

    $applyToScene(THREE, scene, renderer){
        if(!THREE || !scene) return
        // background
        switch(this.backgroundMode){
            case SceneEnvironment.Transparent:
                scene.background = null
                if(renderer) renderer.setClearAlpha(0)
                break
            case SceneEnvironment.SkyBox:
                if(this.lightProbe && this.lightProbe.$build){
                    let tex = this.lightProbe.$build(THREE)
                    scene.background = tex || null
                } else {
                    scene.background = null
                }
                break
            case SceneEnvironment.Color:
            default:
                try {
                    scene.background = new THREE.Color(this.clearColor || '#000000')
                } catch (e){
                    scene.background = null
                }
                break
        }
        // light probe (IBL)
        if(this.lightProbe && this.lightProbe.$build){
            let env = this.lightProbe.$build(THREE)
            scene.environment = env || null
        } else {
            scene.environment = null
        }
        // tone mapping
        if(renderer){
            switch(this.tonemapMode){
                case SceneEnvironment.NoTonemap:
                    renderer.toneMapping = 0 // NoToneMapping
                    break
                case SceneEnvironment.TonemapModeLinear:
                    renderer.toneMapping = 1 // LinearToneMapping
                    break
                case SceneEnvironment.TonemapModeAces:
                    renderer.toneMapping = 4 // ACESFilmicToneMapping
                    break
                case SceneEnvironment.TonemapModeFilmic:
                    renderer.toneMapping = 3 // CineonToneMapping (closest)
                    break
            }
        }
    }

    $envChanged(){
        if(this.$onEnvChanged) this.$onEnvChanged()
    }
}

module.exports.SceneEnvironment = SceneEnvironment
