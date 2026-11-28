const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { aliasValue, withAliasField } = require('./AliasValue')

// "When the binding becomes inactive again, any direct bindings that were previously set on the property will be restored."
const RestoreNone = 0, RestoreBinding = 1, RestoreValue = 2, RestoreBindingOrValue = 3

class Binding extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$saved = null      // { target, id, bindings, value }
        this.$delayedTimer = null
        engine.scheduleComponentComplete(this)
    }

    // the first application is immediate even when delayed (checked with JQMLHost)
    $componentComplete(){
        super.$componentComplete()
        this.$applyNow()
    }

    $targetProperty(){
        let target = this.$v[Binding.TARGET]
        let name = this.$v[Binding.PROPERTY]
        if(!target || !name || !target.constructor || !target.constructor.$type) return null
        let prop = target.constructor.$type.propByName.get(name)
        if(prop) return { target, id: prop.id }
        const resolve = target.constructor.$aliases?.[name]
        if(!resolve) return null
        const [owner, id, field, kind] = resolve(target)
        return { target: owner, id, field, kind }
    }

    $apply(){
        if(!(this.$f & engine.COMPLETED)) return
        // "A delayed binding will not immediately update the target, but rather wait until the event queue has been cleared"
        if(this.$v[Binding.DELAYED]){
            if(this.$delayedTimer) return
            this.$delayedTimer = setTimeout(() => {
                this.$delayedTimer = null
                if(!(this.$f & engine.DESTROYED)) engine.batch(() => this.$applyNow())
            }, 0)
            return
        }
        this.$applyNow()
    }

    $applyNow(){
        let active = this.$v[Binding.WHEN] && this.$targetProperty()
        if(this.$saved && (!active || active.target !== this.$saved.target || active.id !== this.$saved.id || active.field !== this.$saved.field)) this.$restore()
        if(!active) return
        if(!this.$saved){
            const value = active.target.$v[active.id]
            this.$saved = {
                ...active,
                bindings: active.target.$bindingSnapshot(active.id, active.field),
                value: active.field === undefined ? value : aliasValue(value, active.kind)[active.field],
            }
            active.target.$removeBindings(active.id, active.field)
        }
        writeValue(active, this.$v[Binding.VALUE])
    }

    $restore(){
        let saved = this.$saved
        this.$saved = null
        if(saved.target.$f & (engine.DESTROYED | engine.DELETING)) return
        let mode = this.$v[Binding.RESTOREMODE]
        if(saved.bindings && mode & RestoreBinding){
            saved.target.$restoreBindings(saved.id, saved.bindings)
        } else if(mode & RestoreValue){
            writeValue(saved, saved.value)
        }
    }

    $disposeNative(){
        if(this.$delayedTimer){
            clearTimeout(this.$delayedTimer)
            this.$delayedTimer = null
        }
        if(this.$saved) this.$restore()
        super.$disposeNative()
    }
}

function writeValue({target, id, field, kind}, value){
    if(field !== undefined) value = withAliasField(aliasValue(target.$v[id], kind), field, value)
    target.$write(id, value)
}

const reapply = (binding) => binding.$apply()

defineType(Binding, {
    props: {
        target: { kind: 'var', effect: reapply },
        property: { kind: 'string', effect: reapply },
        value: { kind: 'var', effect: reapply },
        when: { kind: 'bool', initial: true, effect: reapply },
        delayed: { kind: 'bool', effect: reapply },
        restoreMode: { kind: 'int', initial: RestoreBindingOrValue },
    },
})
Binding.RestoreNone = RestoreNone
Binding.RestoreBinding = RestoreBinding
Binding.RestoreValue = RestoreValue
Binding.RestoreBindingOrValue = RestoreBindingOrValue

module.exports = { Binding }
