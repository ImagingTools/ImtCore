const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class MenuSeparator extends Control {
    static meta = Object.assign({}, Control.meta, {
        implicitHeight: { type: Real, value: 0 },
        implicitWidth: { type: Real, value: 0 },
        
        implicitHeightChanged: { type: Signal, args: [] },
        implicitWidthChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('MenuSeparator')

        return obj
    }

}



module.exports = MenuSeparator
