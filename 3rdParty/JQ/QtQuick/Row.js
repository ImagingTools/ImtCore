const Item = require("./Item")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class Row extends Item {
    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
    })

}

module.exports = Row