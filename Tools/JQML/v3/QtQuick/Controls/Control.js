const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Int = require("../../QtQml/Int")
const Signal = require("../../QtQml/Signal")
const Font = require("../../QtQml/Font")

class Control extends Item {
    static meta = Object.assign({}, Item.meta, {
        availableHeight: { type: Real, value: 0 },
        availableWidth: { type: Real, value: 0 },
        background: { type: Var },
        bottomInset: { type: Real, value: 0 },
        bottomPadding: { type: Real, value: 0 },
        contentItem: { type: Var },
        focusReason: { type: Int, value: 0 },
        font: { type: Font },
        horizontalPadding: { type: Real, value: 0 },
        hoverEnabled: { type: Var },
        hovered: { type: Var, value: false },
        implicitBackgroundHeight: { type: Real, value: 0 },
        implicitBackgroundWidth: { type: Real, value: 0 },
        implicitContentHeight: { type: Real, value: 0 },
        implicitContentWidth: { type: Real, value: 0 },
        leftInset: { type: Real, value: 0 },
        leftPadding: { type: Real, value: 0 },
        locale: { type: Var },
        mirrored: { type: Var, value: false },
        padding: { type: Real, value: 0 },
        rightInset: { type: Real, value: 0 },
        rightPadding: { type: Real, value: 0 },
        spacing: { type: Real, value: 0 },
        topInset: { type: Real, value: 0 },
        topPadding: { type: Real, value: 0 },
        verticalPadding: { type: Real, value: 0 },
        visualFocus: { type: Var, value: false },
        wheelEnabled: { type: Var, value: false },
        
        availableHeightChanged: { type: Signal, args: [] },
        availableWidthChanged: { type: Signal, args: [] },
        backgroundChanged: { type: Signal, args: [] },
        bottomInsetChanged: { type: Signal, args: [] },
        bottomPaddingChanged: { type: Signal, args: [] },
        contentItemChanged: { type: Signal, args: [] },
        focusReasonChanged: { type: Signal, args: [] },
        fontChanged: { type: Signal, args: [] },
        horizontalPaddingChanged: { type: Signal, args: [] },
        hoverEnabledChanged: { type: Signal, args: [] },
        hoveredChanged: { type: Signal, args: [] },
        implicitBackgroundHeightChanged: { type: Signal, args: [] },
        implicitBackgroundWidthChanged: { type: Signal, args: [] },
        implicitContentHeightChanged: { type: Signal, args: [] },
        implicitContentWidthChanged: { type: Signal, args: [] },
        leftInsetChanged: { type: Signal, args: [] },
        leftPaddingChanged: { type: Signal, args: [] },
        localeChanged: { type: Signal, args: [] },
        mirroredChanged: { type: Signal, args: [] },
        paddingChanged: { type: Signal, args: [] },
        rightInsetChanged: { type: Signal, args: [] },
        rightPaddingChanged: { type: Signal, args: [] },
        spacingChanged: { type: Signal, args: [] },
        topInsetChanged: { type: Signal, args: [] },
        topPaddingChanged: { type: Signal, args: [] },
        verticalPaddingChanged: { type: Signal, args: [] },
        visualFocusChanged: { type: Signal, args: [] },
        wheelEnabledChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Control')

        return obj
    }

}



module.exports = Control