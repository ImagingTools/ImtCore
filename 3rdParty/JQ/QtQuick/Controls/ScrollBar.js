const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class ScrollBar extends Control {
    static meta = Object.assign({}, Control.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        obj.__getDOM().classList.add('ScrollBar')

        return obj
    }

}

ScrollBar.initialize()

module.exports = ScrollBar