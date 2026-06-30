const GroupProperty = require("./GroupProperty")
const Bool = require("./Bool")
const Int = require("./Int")
const Real = require("./Real")
const String = require("./String")
const Var = require("./Var")
const Point = require("./Point")
const SourceSize = require("./SourceSize")
const Signal = require("./Signal")

class Drag extends GroupProperty {
    static None = 0
    static Automatic = 1
    static Internal = 2

    static XAxis = 0
    static YAxis = 1
    static XAndYAxis = 2

    static meta = {
        active: { type: Bool, value: false },
        dragType: { type: Int, value: Drag.Internal },
        hotSpot: { type: Point },
        imageSource: { type: Var, value: undefined },
        imageSourceSize: { type: SourceSize },
        keys: { type: String, value: "" },
        mimeData: { type: Var, value: null },
        proposedAction: { type: Int, value: Drag.None },
        source: { type: Var, value: null },
        supportedActions: { type: Real, value: 0 },
        target: { type: Var, value: null },

        axis: { type: Int, value: Drag.XAndYAxis },
        filterChildren: { type: Bool, value: false },
        maximumX: { type: Real, value: Number.POSITIVE_INFINITY },
        maximumY: { type: Real, value: Number.POSITIVE_INFINITY },
        minimumX: { type: Real, value: Number.NEGATIVE_INFINITY },
        minimumY: { type: Real, value: Number.NEGATIVE_INFINITY },
        smoothed: { type: Bool, value: false },
        threshold: { type: Real, value: 0 },

        activeChanged: { type: Signal, args: [] },
        dragTypeChanged: { type: Signal, args: [] },
        hotSpotChanged: { type: Signal, args: [] },
        imageSourceChanged: { type: Signal, args: [] },
        imageSourceSizeChanged: { type: Signal, args: [] },
        keysChanged: { type: Signal, args: [] },
        mimeDataChanged: { type: Signal, args: [] },
        proposedActionChanged: { type: Signal, args: [] },
        sourceChanged: { type: Signal, args: [] },
        supportedActionsChanged: { type: Signal, args: [] },
        targetChanged: { type: Signal, args: [] },

        axisChanged: { type: Signal, args: [] },
        filterChildrenChanged: { type: Signal, args: [] },
        maximumXChanged: { type: Signal, args: [] },
        maximumYChanged: { type: Signal, args: [] },
        minimumXChanged: { type: Signal, args: [] },
        minimumYChanged: { type: Signal, args: [] },
        smoothedChanged: { type: Signal, args: [] },
        thresholdChanged: { type: Signal, args: [] },
    }

    None = Drag.None
    Automatic = Drag.Automatic
    Internal = Drag.Internal

    XAxis = Drag.XAxis
    YAxis = Drag.YAxis
    XAndYAxis = Drag.XAndYAxis
}

module.exports = Drag