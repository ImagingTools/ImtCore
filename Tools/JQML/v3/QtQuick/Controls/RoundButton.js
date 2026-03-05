const Button = require("./Button")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class RoundButton extends Button {
    static meta = Object.assign({}, Button.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('RoundButton')

        return obj
    }

}



module.exports = RoundButton