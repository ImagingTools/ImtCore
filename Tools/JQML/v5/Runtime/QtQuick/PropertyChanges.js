const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { readProperty, resolveProperty } = require('./Animation')

// ---- PropertyChanges ------------------------------------------------------------------------

// The compiler turns every other assignment into $change(target, path, isExpression, value|fn, scope).
// "Expressions are evaluated with the target as the scope object" (object() in actions()).
class PropertyChanges extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$changes = []
    }

    $change(target, path, expression, value, scope){
        this.$changes.push({ target, path, expression, value, scope })
    }

    $actions(){
        let list = []
        let target = this.$v[PropertyChanges.TARGET]
        let restore = this.$v[PropertyChanges.RESTOREENTRYVALUES]
        let explicit = this.$v[PropertyChanges.EXPLICIT]
        for(let change of this.$changes){
            let object = change.target || target
            let p = resolveProperty(object, change.path)
            if(!p){
                if(object) engine.reportError('Cannot assign to non-existent property "' + change.path + '"', 'PropertyChanges')
                continue
            }
            let action = { p, restore, from: readProperty(p), to: undefined, toBinding: null, specifiedObject: object, specifiedProperty: change.path }
            if(!change.expression) action.to = change.value
            else if(explicit) action.to = change.value.call(object, object, change.scope)
            else action.toBinding = { fn: change.value, scope: change.scope, self: object }
            list.push(action)
        }
        return list
    }
}

defineType(PropertyChanges, {
    props: {
        target: 'var',
        explicit: 'bool',
        restoreEntryValues: { kind: 'bool', initial: true },
    },
})

module.exports = { PropertyChanges }
