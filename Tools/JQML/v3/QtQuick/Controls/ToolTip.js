const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class ToolTip extends Popup {
    static meta = Object.assign({}, Popup.meta, {
        delay: { type: Int, value: 0 },
        text: { type: String, value: '' },
        timeout: { type: Int, value: -1 },
        
        delayChanged: { type: Signal, args: [] },
        textChanged: { type: Signal, args: [] },
        timeoutChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        return obj
    }

}


module.exports = ToolTip