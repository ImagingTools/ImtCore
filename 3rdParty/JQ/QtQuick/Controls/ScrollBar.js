const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class ScrollBar extends Control {
    static meta = Object.assign({}, Control.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('ScrollBar')

        return obj
    }

}



module.exports = ScrollBar