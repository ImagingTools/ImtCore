const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class AbstractButton extends Control {
    static meta = Object.assign({}, Control.meta, {

    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)
        proxy.__getDOM().classList.add('AbstractButton')

        return proxy
    }

}

module.exports = AbstractButton