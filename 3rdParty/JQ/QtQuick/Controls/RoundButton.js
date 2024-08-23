const Button = require("./Button")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class RoundButton extends Button {
    static meta = Object.assign({}, Button.meta, {

    })

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)
        proxy.__getDOM().classList.add('RoundButton')

        return proxy
    }

}

module.exports = RoundButton