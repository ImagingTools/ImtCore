const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class ToolTip extends Popup {
    static meta = Object.assign({}, Popup.meta, {

    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)
        proxy.__getDOM().classList.add('ToolTip')

        return proxy
    }

}

module.exports = ToolTip