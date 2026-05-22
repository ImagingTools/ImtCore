const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QInt } = require('../utils/properties')

class Action extends Item {
    static defaultProperties = {
        text: { type: QString, value: '' },
        icon: { type: QVar },
        enabled: { type: QBool, value: true },
        checkable: { type: QBool, value: false },
        checked: { type: QBool, value: false },
        shortcut: { type: QString, value: '' },
    }

    static defaultSignals = {
        triggered: { params: [] },
        toggled: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({ display: 'none' })
    }

    trigger(){
        if (!this.getPropertyValue('enabled')) return

        if (this.getPropertyValue('checkable')){
            let current = this.getPropertyValue('checked')
            this.getProperty('checked').reset(!current)
            if (this.$signals.toggled) this.$signals.toggled()
        }

        if (this.$signals.triggered) this.$signals.triggered()
    }
}

module.exports.Action = Action
