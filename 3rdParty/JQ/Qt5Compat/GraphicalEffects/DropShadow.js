const Item = require("../../QtQuick/Item")
const Color = require("../../QtQml/Color")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class DropShadow extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'black', signalName:'colorChanged'},
        horizontalOffset: {type: Real, value:0, signalName:'horizontalOffsetChanged'},
        verticalOffset: {type: Real, value:0, signalName:'verticalOffsetChanged'},
        radius: {type: Real, value:4, signalName:'radiusChanged'},
        samples: {type: Real, value:9, signalName:'samplesChanged'},
        spread: {type: Real, value:0, signalName:'spreadChanged'},
        source: {type: Var, value:undefined, signalName:'sourceChanged'},
        
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        horizontalOffsetChanged: {type:Signal, slotName:'onHorizontalOffsetChanged', args:[]},
        verticalOffsetChanged: {type:Signal, slotName:'onVerticalOffsetChanged', args:[]},
        radiusChanged: {type:Signal, slotName:'onRadiusChanged', args:[]},
        samplesChanged: {type:Signal, slotName:'onSamplesChanged', args:[]},
        spreadChanged: {type:Signal, slotName:'onSpreadChanged', args:[]},
        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
    })

}

module.exports = DropShadow