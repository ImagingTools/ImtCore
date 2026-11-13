const { defineType } = require('../Core/type')
const { Animation, resolveProperty, writeProperty } = require('./Animation')
const { PropertyAnimation } = require('./PropertyAnimation')

// ---- PropertyAction / ScriptAction ----------------------------------------------------------

class PropertyAction extends Animation {
    $transition(actions, modified, defaultTarget){
        let props = PropertyAnimation.prototype.$properties.call(this)
        let targets = PropertyAnimation.prototype.$targets.call(this)
        if(defaultTarget && !targets.length) targets.push(defaultTarget)
        let hasValue = this.$valueDefined
        let result = []
        if(hasValue){
            for(let name of props){
                for(let target of targets){
                    let p = resolveProperty(target, name)
                    if(p) result.push({ p, to: this.$v[PropertyAction.VALUE] })
                    for(let action of actions){
                        if(p && action.p.obj === p.obj && action.p.id === p.id) modified.push(action)
                    }
                }
            }
        } else {
            for(let action of actions){
                let p = action.p
                if((targets.length && targets.indexOf(p.target) < 0) || (props.length && props.indexOf(p.path) < 0 && props.indexOf(p.name) < 0)) continue
                result.push({ p, to: action.to })
                modified.push(action)
            }
        }
        this.$actions = result
    }

    $updateCurrentTime(){
        for(let action of this.$actions) writeProperty(action.p, action.to)
    }
}
defineType(PropertyAction, {
    props: {
        target: 'var',
        targets: 'var',
        property: 'string',
        properties: 'string',
        value: { kind: 'var', effect(action){ action.$valueDefined = true } },
    },
})

module.exports = { PropertyAction }
