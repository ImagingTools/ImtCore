const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Switch extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {
        position: { type: Real, value: 0 },
        visualPosition: { type: Real, value: 0 },
        
        positionChanged: { type: Signal, args: [] },
        visualPositionChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Switch')

        return obj
    }

}



module.exports = Switch