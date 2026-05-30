const GroupProperty = require("../../QtQml/GroupProperty")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")
const QtEnums = require("../../Qt/enums")

class Layout extends GroupProperty {
    static SizePolicyImplicit = 0
    static SizePolicyExplicit = 1

    static meta = Object.assign({}, GroupProperty.meta, {
        alignment: { type: Int, value: QtEnums.AlignVCenter | QtEnums.AlignLeft},
        bottomMargin: { type: Real, value: 0 },
        column: { type: Int, value: -1 },
        columnSpan: { type: Int, value: 1 },
        fillHeight: { type: Var, value: false },
        fillWidth: { type: Var, value: false },
        horizontalStretchFactor: { type: Int, value: -1 },
        leftMargin: { type: Real, value: 0 },
        margins: { type: Real, value: 0 },
        maximumHeight: { type: Real, value: Infinity },
        maximumWidth: { type: Real, value: Infinity },
        minimumHeight: { type: Real, value: 0 },
        minimumWidth: { type: Real, value: 0 },
        preferredHeight: { type: Real, value: -1 },
        preferredWidth: { type: Real, value: -1 },
        rightMargin: { type: Real, value: 0 },
        row: { type: Int, value: -1 },
        rowSpan: { type: Int, value: 1 },
        topMargin: { type: Real, value: 0 },
        useDefaultSizePolicy: { type: Int, value: Layout.SizePolicyImplicit },
        verticalStretchFactor: { type: Int, value: -1 },

        alignmentChanged: {type:Signal, args:[]},
        bottomMarginChanged: {type:Signal, args:[]},
        columnChanged: {type:Signal, args:[]},
        columnSpanChanged: {type:Signal, args:[]},
        fillHeightChanged: {type:Signal, args:[]},
        fillWidthChanged: {type:Signal, args:[]},
        horizontalStretchFactorChanged: {type:Signal, args:[]},
        leftMarginChanged: {type:Signal, args:[]},
        marginsChanged: {type:Signal, args:[]},
        maximumHeightChanged: {type:Signal, args:[]},
        maximumWidthChanged: {type:Signal, args:[]},
        minimumHeightChanged: {type:Signal, args:[]},
        minimumWidthChanged: {type:Signal, args:[]},
        preferredHeightChanged: {type:Signal, args:[]},
        preferredWidthChanged: {type:Signal, args:[]},
        rightMarginChanged: {type:Signal, args:[]},
        rowChanged: {type:Signal, args:[]},
        rowSpanChanged: {type:Signal, args:[]},
        topMarginChanged: {type:Signal, args:[]},
        useDefaultSizePolicyChanged: {type:Signal, args:[]},
        verticalStretchFactorChanged: {type:Signal, args:[]},

    })

    // Trigger parent layout re-calculation when any layout hint changes
    _triggerParentLayout(){
        const self = this.__parent && this.__parent.__proxy
        if(self && self.parent) JQApplication.updateLater(self.parent)
    }

    SLOT_alignmentChanged()              { this._triggerParentLayout() }
    SLOT_fillWidthChanged()              { this._triggerParentLayout() }
    SLOT_fillHeightChanged()             { this._triggerParentLayout() }
    SLOT_preferredWidthChanged()         { this._triggerParentLayout() }
    SLOT_preferredHeightChanged()        { this._triggerParentLayout() }
    SLOT_minimumWidthChanged()           { this._triggerParentLayout() }
    SLOT_minimumHeightChanged()          { this._triggerParentLayout() }
    SLOT_maximumWidthChanged()           { this._triggerParentLayout() }
    SLOT_maximumHeightChanged()          { this._triggerParentLayout() }
    SLOT_horizontalStretchFactorChanged(){ this._triggerParentLayout() }
    SLOT_verticalStretchFactorChanged()  { this._triggerParentLayout() }
    SLOT_marginsChanged()                { this._triggerParentLayout() }
    SLOT_leftMarginChanged()             { this._triggerParentLayout() }
    SLOT_rightMarginChanged()            { this._triggerParentLayout() }
    SLOT_topMarginChanged()              { this._triggerParentLayout() }
    SLOT_bottomMarginChanged()           { this._triggerParentLayout() }
}



module.exports = Layout