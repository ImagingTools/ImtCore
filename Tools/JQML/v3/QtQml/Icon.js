const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Bool = require("./Bool")
const StringProp = require("./String")
const Var = require("./Var")
const Signal = require("./Signal")

/**
 * Icon — grouped property for AbstractButton.icon (and similar).
 * Mirrors Qt6 QQuickIcon:
 *   icon.name, icon.source, icon.width, icon.height, icon.color, icon.cache
 */
class Icon extends GroupProperty {
    static meta = {
        name:   { type: StringProp, value: '' },
        source: { type: StringProp, value: '' },
        width:  { type: Real, value: -1 },
        height: { type: Real, value: -1 },
        color:  { type: Var,  value: undefined },
        cache:  { type: Bool, value: true },

        nameChanged:   { type: Signal, args: [] },
        sourceChanged: { type: Signal, args: [] },
        widthChanged:  { type: Signal, args: [] },
        heightChanged: { type: Signal, args: [] },
        colorChanged:  { type: Signal, args: [] },
        cacheChanged:  { type: Signal, args: [] },
    }
}

module.exports = Icon
