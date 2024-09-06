const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Button extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {

    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)
        proxy.__getDOM().classList.add('Button')

        return proxy
    }

}

module.exports = Button