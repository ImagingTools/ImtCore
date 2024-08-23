const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Control extends Item {
    static meta = Object.assign({}, Item.meta, {

    })

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)
        proxy.__getDOM().classList.add('Control')

        return proxy
    }

}

module.exports = Control