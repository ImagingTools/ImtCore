const { QtObject } = require('./QtObject')
const { QVar, QReal } = require('../utils/properties')

class DelegateChoice extends QtObject {
    static defaultProperties = {
        roleValue: { type: QVar, value: undefined },
        column: { type: QReal, value: -1 },
        row: { type: QReal, value: -1 },
        delegate: { type: QVar, value: undefined },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
    }
}

class DelegateChooser extends QtObject {
    static defaultProperties = {
        role: { type: QVar, value: 'display' },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$exCtx = exCtx
        this.$choices = []
    }

    addResource(resource) {
        super.addResource(resource)
        if (resource instanceof DelegateChoice) {
            this.$choices.push(resource)
        }
    }

    // Called by TableView.$createCell to resolve which delegate to use for a given cell
    $chooseDelegate(row, col, cellData) {
        let role = this.getPropertyValue('role')
        let roleValue = cellData ? cellData[role] : undefined

        for (let i = 0; i < this.$choices.length; i++) {
            let choice = this.$choices[i]
            let choiceRow = choice.getPropertyValue('row')
            let choiceCol = choice.getPropertyValue('column')
            let choiceRoleValue = choice.getPropertyValue('roleValue')

            // Row filter: -1 means any row
            if (choiceRow >= 0 && choiceRow !== row) continue
            // Column filter: -1 means any column
            if (choiceCol >= 0 && choiceCol !== col) continue
            // Role value filter: undefined means any value
            if (choiceRoleValue !== undefined && choiceRoleValue !== roleValue) continue

            let delegate = choice.getPropertyValue('delegate')
            if (delegate) return delegate
        }

        // No match — return null
        return null
    }
}

module.exports.DelegateChooser = DelegateChooser
module.exports.DelegateChoice = DelegateChoice
