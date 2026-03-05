const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class ToolTip extends Popup {
    static meta = Object.assign({}, Popup.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        return obj
    }

}


module.exports = ToolTip