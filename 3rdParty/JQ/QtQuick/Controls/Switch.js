const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Switch extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        obj.__getDOM().classList.add('Switch')

        return obj
    }

}

Switch.initialize()

module.exports = Switch