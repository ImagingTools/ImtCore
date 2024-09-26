const QtObject = require("../../QtQml/QtObject")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Popup extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)

        return proxy
    }

}

module.exports = Popup