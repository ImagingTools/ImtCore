// DelegateModel of QtQml.Models (qqmldelegatemodel.cpp): its enums (DelegateModelAccess, the access of the delegates
// of a view to the model, TableView.delegateModelAccess) and its basic properties. A DelegateModel as the model of a
// view (groups, filters, parts) is not there yet: one made says so.
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')

const ENUMS = { Qt5ReadWrite: 0, ReadOnly: 1, ReadWrite: 2 }

class DelegateModel extends QtObject {
    constructor(parent = null){
        super(parent)
        console.warn('DelegateModel: a DelegateModel as a model of a view is not supported yet; its enums are')
    }
}
defineType(DelegateModel, {
    props: {
        model: 'var',
        delegate: 'component',
        rootIndex: 'var',
        filterOnGroup: 'string',
        delegateModelAccess: { kind: 'int', initial: ENUMS.Qt5ReadWrite },
    },
})
Object.assign(DelegateModel, ENUMS)

module.exports = { DelegateModel }
