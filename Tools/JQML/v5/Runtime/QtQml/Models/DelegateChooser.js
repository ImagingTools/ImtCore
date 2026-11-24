// DelegateChooser and DelegateChoice (qqmldelegatecomponent.cpp, Qt 6.12; QtQml.Models since 6.9, Qt.labs.qmlmodels
// before): a component whose delegate for an item is the first choice that matches the item's role value, row and column.
// A chooser is a Component (QQmlAbstractDelegateComponent): a view asks it for the delegate of each cell.
const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { Component } = require('../Component')

class DelegateChoice extends QtObject {
    // QQmlDelegateChoice::match: no roleValue, row or column set matches anything
    $match(row, column, value){
        let roleValue = this.$v[DelegateChoice.ROLEVALUE]
        let wantRow = this.$v[DelegateChoice.ROW], wantColumn = this.$v[DelegateChoice.COLUMN]
        let hasValue = roleValue !== undefined
        if(!hasValue && wantRow < 0 && wantColumn < 0) return true
        let roleMatched = true
        if(hasValue){
            roleMatched = value === roleValue
            if(!roleMatched){
                // QVariant::toInt of both, then their strings
                let a = toInt(value), b = toInt(roleValue)
                roleMatched = a !== null && b !== null && a === b
            }
            if(!roleMatched) roleMatched = variantString(value) === variantString(roleValue)
        }
        return roleMatched && (wantRow < 0 || wantRow === row) && (wantColumn < 0 || wantColumn === column)
    }
}

// QVariant::toInt: numbers, booleans and strings of an integer
function toInt(value){
    if(typeof value === 'number') return Number.isFinite(value) ? Math.trunc(value) : null
    if(typeof value === 'boolean') return value ? 1 : 0
    if(typeof value === 'string' && /^\s*[+-]?\d+\s*$/.test(value)) return parseInt(value, 10)
    return null
}

function variantString(value){
    if(value === undefined || value === null) return ''
    return String(value)
}

const changed = (choice) => {
    let chooser = choice.$parent
    if(chooser instanceof DelegateChooser) engine.emitArgs(chooser, DelegateChooser.SIGNAL_delegateChanged, [])
}

defineType(DelegateChoice, {
    props: {
        roleValue: { kind: 'var', effect: changed },
        // index is row under another name (QQmlDelegateChoice: both read and write m_row)
        row: { kind: 'int', initial: -1, effect: (choice, value) => {
            if(choice.$v[DelegateChoice.INDEX] !== value) choice.$write(DelegateChoice.INDEX, value)
            changed(choice)
        } },
        index: { kind: 'int', initial: -1, effect: (choice, value) => {
            if(choice.$v[DelegateChoice.ROW] !== value) choice.$write(DelegateChoice.ROW, value)
        } },
        column: { kind: 'int', initial: -1, effect: changed },
        delegate: { kind: 'component', effect: changed },
    },
})
DelegateChoice.$defaultProperty = 'delegate'

class DelegateChooser extends Component {
    constructor(parent = null){
        super(parent, null)
    }

    // the choices: the DelegateChoices declared in it, or the list assigned to choices
    $choices(){
        let list = this.$v[DelegateChooser.CHOICES]
        if(Array.isArray(list)) return list
        return (this.$children || []).filter(child => child instanceof DelegateChoice)
    }

    // QQmlDelegateChooser::delegate: value(row, column) reads the role of a cell (the role of modelData when the cell
    // has only modelData, an object or a map)
    $delegateFor(value, row, column){
        let role = this.$v[DelegateChooser.ROLE]
        let v = role ? value(role) : undefined
        if(v === undefined){
            let data = value('modelData')
            if(data !== undefined && data !== null && typeof data === 'object') v = data[role]
        }
        for(let choice of this.$choices()) if(choice.$match(row, column, v)) return choice.$v[DelegateChoice.DELEGATE]
        return null
    }

    // a chooser is no component that makes an object of its own
    createObject(){ return null }
}
defineType(DelegateChooser, {
    props: {
        role: 'string',
        choices: 'var',
    },
    signals: ['delegateChanged'],
})
DelegateChooser.$defaultProperty = 'choices'

// the delegate of a cell: through choosers nested in choosers (QQmlTableInstanceModel::resolveDelegate)
function resolveDelegate(delegate, value, row, column){
    while(delegate instanceof DelegateChooser) delegate = delegate.$delegateFor(value, row, column)
    return delegate || null
}

module.exports = { DelegateChooser, DelegateChoice, resolveDelegate }
