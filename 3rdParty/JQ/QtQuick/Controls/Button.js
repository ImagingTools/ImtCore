const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Button extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        obj.__getDOM().classList.add('Button')

        return obj
    }

}

Button.initialize()

module.exports = Button