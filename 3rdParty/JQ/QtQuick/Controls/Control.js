const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Control extends Item {
    static meta = Object.assign({}, Item.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Control')

        return obj
    }

}



module.exports = Control