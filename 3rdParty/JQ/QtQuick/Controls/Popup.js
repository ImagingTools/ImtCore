const QtObject = require("../../QtQml/QtObject")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Popup extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        return obj
    }

}



module.exports = Popup