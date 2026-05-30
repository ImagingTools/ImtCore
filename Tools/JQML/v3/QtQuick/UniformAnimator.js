const NumberAnimation = require("./NumberAnimation")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class UniformAnimator extends NumberAnimation {
    static meta = Object.assign({}, NumberAnimation.meta, {
        uniform: { type: String, value: '', signalName: 'uniformChanged' },

        uniformChanged: { type: Signal, args: [] },
    })

    SLOT_uniformChanged(){
        this.property = this.uniform
    }
}

module.exports = UniformAnimator
