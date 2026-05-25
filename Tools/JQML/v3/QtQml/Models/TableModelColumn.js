const QtObject = require("../QtObject")
const Var = require("../Var")
const Int = require("../Int")
const Signal = require("../Signal")

class TableModelColumn extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        display:               { type: Var, value: undefined },
        edit:                  { type: Var, value: undefined },
        decoration:            { type: Var, value: undefined },
        toolTip:               { type: Var, value: undefined },
        statusTip:             { type: Var, value: undefined },
        whatsThis:             { type: Var, value: undefined },
        font:                  { type: Var, value: undefined },
        textAlignment:         { type: Var, value: undefined },
        background:            { type: Var, value: undefined },
        foreground:            { type: Var, value: undefined },
        checkState:            { type: Var, value: undefined },
        accessibleText:        { type: Var, value: undefined },
        accessibleDescription: { type: Var, value: undefined },
        sizeHint:              { type: Var, value: undefined },

        displayChanged:               { type: Signal, args: [] },
        editChanged:                  { type: Signal, args: [] },
    })
}

module.exports = TableModelColumn
