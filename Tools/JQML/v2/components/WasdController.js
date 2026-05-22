const { QtObject } = require('./QtObject')
const { QReal, QVar, QBool } = require('../utils/properties')

/**
 * WasdController - mirrors Qt Quick 3D Helpers' WasdController.
 *
 * Provides keyboard (WASD + mouse) based first-person camera control
 * for the web. Listens for DOM keyboard/pointer events when enabled.
 */
class WasdController extends QtObject {
    static defaultProperties = {
        controlledObject: { type: QVar,  value: undefined, changed: '$controlChanged' },
        speed:            { type: QReal, value: 1,         changed: '$controlChanged' },
        shiftSpeed:       { type: QReal, value: 3,         changed: '$controlChanged' },
        mouseEnabled:     { type: QBool, value: true,      changed: '$controlChanged' },
        xSpeed:           { type: QReal, value: 0.1 },
        ySpeed:           { type: QReal, value: 0.1 },
        xInvert:          { type: QBool, value: false },
        yInvert:          { type: QBool, value: false },
    }

    $controlChanged(){
        // rebind when controlled object changes
    }
}

module.exports.WasdController = WasdController
