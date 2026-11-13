const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { readProperty } = require('./Animation')
const { takeBinding } = require('./StateBindings')
const { TransitionManager } = require('./TransitionManager')

const same = (a, b) => a.obj === b.obj && a.id === b.id

function currentBinding(p){
    let binding = p.obj.$findBinding(p.id)
    return binding ? { fn: binding.fn, scope: binding.scope, self: binding.self } : null
}

// ---- State ----------------------------------------------------------------------------------

class State extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$group = null
        this.$whenKnown = false
        this.$revertList = []
        this.$manager = new TransitionManager(this)
        this.$inExtend = false
    }

    get changes(){ return (this.$children || []).filter(child => typeof child.$actions === 'function') }

    $assign(id, value){
        if(id === State.WHEN) this.$whenKnown = true
        super.$assign(id, value)
    }

    $bind(id, fn, scope, self){
        if(id === State.WHEN) this.$whenKnown = true
        return super.$bind(id, fn, scope, self)
    }

    $isNamed(){ return !!this.$v[State.NAME] }

    // QQuickStatePrivate::generateActionList: the extended state's actions first
    $actionList(){
        let list = []
        if(this.$inExtend) return list
        this.$inExtend = true
        try {
            let extend = this.$v[State.EXTEND]
            if(extend && this.$group){
                let base = this.$group.$find(extend)
                if(base) list = base.$actionList()
            }
            for(let change of this.changes) list.push(...change.$actions())
        } finally {
            this.$inExtend = false
        }
        return list
    }

    $cancel(){
        this.$manager.cancel()
    }

    // QQuickState::apply
    $apply(transition, revert){
        this.$cancel()
        if(revert) revert.$cancel()
        this.$revertList = []
        if(revert){
            this.$revertList = revert.$revertList
            revert.$revertList = []
        }

        let applyList = this.$actionList()
        let additionalReverts = []
        for(let action of applyList){
            if(action.event) continue
            action.fromBinding = currentBinding(action.p)
            let found = this.$revertList.some(r => same(r.p, action.p))
            if(!found && action.restore){
                additionalReverts.push({ p: action.p, value: action.from, binding: action.fromBinding, specifiedObject: action.specifiedObject, specifiedProperty: action.specifiedProperty })
            }
        }

        // reverts of the previous state that this state does not carry on become apply actions
        for(let r of this.$revertList){
            if(applyList.some(action => !action.event && same(action.p, r.p))) continue
            if(r.p.obj.$f & engine.DESTROYED) continue
            let current = readProperty(r.p)
            takeBinding(r.p)
            applyList.push({ p: r.p, from: current, to: r.value, toBinding: r.binding, specifiedObject: r.specifiedObject, specifiedProperty: r.specifiedProperty, reverting: true })
        }
        this.$revertList.push(...additionalReverts)

        this.$manager.transition(applyList, transition, this.$group ? this.$group.$host : null)
    }
}

defineType(State, {
    props: {
        name: 'string',
        when: { kind: 'bool', effect(state){ if(state.$group) state.$group.$updateAutoState() } },
        extend: 'string',
    },
})

module.exports = { State }
