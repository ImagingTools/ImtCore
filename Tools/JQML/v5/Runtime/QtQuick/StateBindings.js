const engine = require('../Core/engine')
const { Item } = require('./Item')

// a binding taken off a property, kept to be set again on revert
function takeBinding(p){
    let binding = p.obj.$takeBinding(p.id)
    if(!binding) return null
    engine.disposeBinding(binding)
    return { fn: binding.fn, scope: binding.scope, self: binding.self }
}

function setBinding(p, binding){
    p.obj.$bind(p.id, binding.fn, binding.scope, binding.self)
}

// QQmlProperty::write: an ordinary write, Behaviors included
function writeValue(p, value){
    if(p.obj instanceof Item){
        if(p.id === Item.WIDTH) p.obj.$explicit |= 1
        if(p.id === Item.HEIGHT) p.obj.$explicit |= 2
    }
    p.obj.$write(p.id, value)
}

module.exports = { takeBinding, setBinding, writeValue }
