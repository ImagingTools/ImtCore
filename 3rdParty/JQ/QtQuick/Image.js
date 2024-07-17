const Item = require("./Item")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class Image extends Item {
    static Stretch = 0
    static PreserveAspectFit = 1
    static PreserveAspectCrop = 2
    static Tile = 3
    static TileVertically = 4
    static TileHorizontally = 5
    static Pad = 6
    static Stretch = 7

    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    static AlignLeft = 0
    static AlignRight = 1
    static AlignHCenter = 2

    static AlignTop = 0
    static AlignBottom = 1
    static AlignVCenter = 2

    static meta = Object.assign({}, Item.meta, {
        progress: {type: Real, value:0, signalName:'progressChanged'},
        source: {type: String, value:'', signalName:'sourceChanged'},
        sourceSize: {type: Var, value:0, signalName:'sourceSizeChanged'},
        fillMode: {type: Real, value:Image.Stretch, signalName:'fillModeChanged'},
        status: {type: Real, value:Image.Null, signalName:'statusChanged'},
        paintedWidth: {type:Real, value:0, signalName:'paintedWidthChanged'},
        paintedHeight: {type:Real, value:0, signalName:'paintedHeightChanged'},
        
        progressChanged: {type:Signal, slotName:'onProgressChanged', args:[]},
        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
        sourceSizeChanged: {type:Signal, slotName:'onSourceSizeChanged', args:[]},
        fillModeChanged: {type:Signal, slotName:'onFillModeChanged', args:[]},
        statusChanged: {type:Signal, slotName:'onStatusChanged', args:[]},
        paintedWidthChanged: {type:Signal, slotName:'onPaintedWidthChanged', args:[]},
        paintedHeightChanged: {type:Signal, slotName:'onPaintedHeightChanged', args:[]},
    })

}

module.exports = Image