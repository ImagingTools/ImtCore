const AbstractButton = require("./AbstractButton")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Button extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {
        flat: { type: Var, value: false },
        highlighted: { type: Var, value: false },
        
        flatChanged: { type: Signal, args: [] },
        highlightedChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Button')

        return obj
    }

}



module.exports = Button