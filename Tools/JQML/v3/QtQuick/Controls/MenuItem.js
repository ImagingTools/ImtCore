const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class MenuItem extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {
        action: { type: Var },
        arrow: { type: Var },
        highlighted: { type: Var, value: false },
        implicitTextPadding: { type: Real, value: 0 },
        implicitHeight: { type: Real, value: 0 },
        implicitWidth: { type: Real, value: 0 },
        menu: { type: Var },
        spacing: { type: Real, value: 0 },
        subMenu: { type: Var },
        text: { type: String, value: '' },
        textPadding: { type: Real, value: 0 },
        
        actionChanged: { type: Signal, args: [] },
        arrowChanged: { type: Signal, args: [] },
        highlightedChanged: { type: Signal, args: [] },
        implicitTextPaddingChanged: { type: Signal, args: [] },
        implicitHeightChanged: { type: Signal, args: [] },
        implicitWidthChanged: { type: Signal, args: [] },
        menuChanged: { type: Signal, args: [] },
        spacingChanged: { type: Signal, args: [] },
        subMenuChanged: { type: Signal, args: [] },
        textChanged: { type: Signal, args: [] },
        textPaddingChanged: { type: Signal, args: [] },
        
        triggered: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('MenuItem')

        return obj
    }

}



module.exports = MenuItem
