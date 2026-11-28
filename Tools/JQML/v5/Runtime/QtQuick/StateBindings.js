const { Item } = require('./Item')
const { propertyWriteValue } = require('./Animation')
const { bindAliasField } = require('../QtQml/AliasValue')

function takeBinding(p){
    p.obj.$removeBindings(p.id, p.field)
}

function setBinding(p, binding){
    if(Array.isArray(binding)) p.obj.$restoreBindings(p.id, binding)
    else if(p.field !== undefined) bindAliasField(p.obj, p.id, p.field, p.valueKind, binding.fn, binding.scope, binding.self)
    else p.obj.$bind(p.id, binding.fn, binding.scope, binding.self)
}

// QQmlProperty::write: an ordinary write, Behaviors included
function writeValue(p, value){
    if(p.obj instanceof Item){
        if(p.id === Item.WIDTH) p.obj.$explicit |= 1
        if(p.id === Item.HEIGHT) p.obj.$explicit |= 2
    }
    p.obj.$write(p.id, propertyWriteValue(p, value))
}

module.exports = { takeBinding, setBinding, writeValue }
