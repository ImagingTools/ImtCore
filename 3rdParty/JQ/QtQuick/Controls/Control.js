const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Control extends Item {
    static meta = Object.assign({}, Item.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        obj.__getDOM().classList.add('Control')

        return obj
    }

}

Control.initialize()

module.exports = Control