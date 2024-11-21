const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class ToolTip extends Popup {
    static meta = Object.assign({}, Popup.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)

        return obj
    }

}
ToolTip.initialize()

module.exports = ToolTip