const QtObject = require("../QtObject")
const Var = require("../Var")
const Signal = require("../Signal")

class DelegateChoice extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        roleValue: { type: Var, value: undefined },
        column:    { type: Var, value: undefined },
        row:       { type: Var, value: undefined },
        delegate:  { type: Var, value: undefined },

        roleValueChanged: { type: Signal, args: [] },
        columnChanged:    { type: Signal, args: [] },
        rowChanged:       { type: Signal, args: [] },
        delegateChanged:  { type: Signal, args: [] },
    })
}

module.exports = DelegateChoice
