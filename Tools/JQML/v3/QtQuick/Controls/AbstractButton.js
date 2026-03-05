const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class AbstractButton extends Control {
    static meta = Object.assign({}, Control.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('AbstractButton')

        return obj
    }

}



module.exports = AbstractButton