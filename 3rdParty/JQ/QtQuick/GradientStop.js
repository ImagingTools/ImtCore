const QtObject = require("../QtQml/QtObject")
const Color = require("../QtQml/Color")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class GradientStop extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        color: {type:Color, value:'black', signalName:'colorChanged'},
        position: {type:Real, value:0, signalName:'positionChanged'},

        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        positionChanged: {type:Signal, slotName:'onPositionChanged', args:[]},
    })
}

module.exports = GradientStop