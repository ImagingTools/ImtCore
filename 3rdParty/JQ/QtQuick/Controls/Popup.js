const QtObject = require("../../QtQml/QtObject")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Popup extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)

        return obj
    }

}

Popup.initialize()

module.exports = Popup