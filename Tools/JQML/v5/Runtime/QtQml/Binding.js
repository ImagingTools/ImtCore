const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// "When the binding becomes inactive again, any direct bindings that were previously set on the property will be restored."
const RestoreNone = 0, RestoreBinding = 1, RestoreValue = 2, RestoreBindingOrValue = 3

class Binding extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$saved = null      // { target, id, binding: { fn, scope, self } | null, value }
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
        return prop ? { target, id: prop.id } : null
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
        if(this.$saved && (!active || active.target !== this.$saved.target || active.id !== this.$saved.id)) this.$restore()
        if(!active) return
        if(!this.$saved){
            let previous = active.target.$takeBinding(active.id)
            this.$saved = {
                target: active.target,
                id: active.id,
                binding: previous ? { fn: previous.fn, scope: previous.scope, self: previous.self } : null,
                value: active.target.$v[active.id],
            }
            if(previous) engine.disposeBinding(previous)
        }
        active.target.$write(active.id, this.$v[Binding.VALUE])
    }

    $restore(){
        let saved = this.$saved
        this.$saved = null
        if(saved.target.$f & engine.DESTROYED) return
        let mode = this.$v[Binding.RESTOREMODE]
        if(saved.binding && mode & RestoreBinding){
            saved.target.$bind(saved.id, saved.binding.fn, saved.binding.scope, saved.binding.self)
        } else if(mode & RestoreValue){
            saved.target.$write(saved.id, saved.value)
        }
    }
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
