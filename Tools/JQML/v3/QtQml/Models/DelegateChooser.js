const QtObject = require("../QtObject")
const Var = require("../Var")
const String = require("../String")
const Signal = require("../Signal")
const DelegateChoice = require("./DelegateChoice")

class DelegateChooser extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        role:    { type: String, value: '' },
        choices: { type: Var, value: undefined },

        roleChanged:    { type: Signal, args: [] },
        choicesChanged: { type: Signal, args: [] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__choices = []
        return obj
    }

    __addChild(child) {
        super.__addChild(child)
        if (child instanceof DelegateChoice) {
            this.__choices.push(child)
        }
    }

    /**
     * Find best matching delegate for given row/column/cellData.
     * Mirrors Qt DelegateChooser logic:
     *   - if `role` is set: match choices by roleValue against cellData[role]
     *   - else match by row / column
     */
    chooseDelegate(row, column, cellData) {
        let role = this.role
        for (let choice of this.__choices) {
            if (role) {
                // role-based matching
                let rv = choice.roleValue
                if (rv === undefined) return choice.delegate  // wildcard
                let dataVal = cellData ? cellData[role] : undefined
                if (dataVal === rv) return choice.delegate
            } else {
                // row / column matching
                let matchRow = choice.row !== undefined
                let matchCol = choice.column !== undefined
                if (!matchRow && !matchCol) return choice.delegate  // wildcard
                if (matchRow && matchCol) {
                    if (choice.row === row && choice.column === column) return choice.delegate
                } else if (matchRow) {
                    if (choice.row === row) return choice.delegate
                } else {
                    if (choice.column === column) return choice.delegate
                }
            }
        }
        return undefined
    }
}

module.exports = DelegateChooser
