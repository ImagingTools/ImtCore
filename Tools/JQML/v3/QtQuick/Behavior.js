const QtObject = require("../QtQml/QtObject")
const Animation = require("./Animation")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

// Behavior on <property>: { <Animation> }
// Watches a property on its parent and runs `animation` on changes.
// Without dedicated compiler support, the implementation exposes the API and
// attaches once `targetProperty` is set.

class Behavior extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        enabled: { type: Bool, value: true },
        targetProperty: { type: String, value: '', signalName: 'targetPropertyChanged' },
        targetValue: { type: Var, value: undefined },
        animation: { type: Var, value: undefined },

        enabledChanged: { type: Signal, args: [] },
        targetPropertyChanged: { type: Signal, args: [] },
        targetValueChanged: { type: Signal, args: [] },
        animationChanged: { type: Signal, args: [] },
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__parentItem = parent
        return obj
    }

    __complete(){
        super.__complete && super.__complete()
        if(!this.animation){
            const a = (this.__children || []).find(o => o instanceof Animation)
            if(a) this.animation = a
        }
        this.__attach()
    }

    SLOT_targetPropertyChanged(){
        this.__attach()
    }

    __attach(){
        const prop = this.targetProperty
        if(!prop || !this.__parentItem || this.__attached) return
        const parent = this.__parentItem
        let lastValue = parent[prop]
        const slot = () => {
            if(!this.enabled) return
            const anim = this.animation
            const newVal = parent[prop]
            if(anim && anim instanceof Animation){
                if('target' in anim) anim.target = parent
                if('property' in anim) anim.property = prop
                if('from' in anim) anim.from = lastValue
                if('to' in anim) anim.to = newVal
                anim.restart()
            }
            lastValue = newVal
        }
        try {
            const sig = parent[prop + 'Changed']
            if(sig && typeof sig.connect === 'function'){
                sig.connect(slot)
                this.__attached = true
            }
        } catch(e) {}
    }
}

module.exports = Behavior
