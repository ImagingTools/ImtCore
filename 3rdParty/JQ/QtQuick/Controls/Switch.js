const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Switch extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {

    })

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)
        proxy.__getDOM().classList.add('Switch')

        return proxy
    }

}

module.exports = Switch