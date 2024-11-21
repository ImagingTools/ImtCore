const Button = require("./Button")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class RoundButton extends Button {
    static meta = Object.assign({}, Button.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        obj.__getDOM().classList.add('RoundButton')

        return obj
    }

}

RoundButton.initialize()

module.exports = RoundButton