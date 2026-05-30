const { QtObject } = require('./QtObject')
const { Animation } = require('./Animation')
const { QBool, QString, QVar } = require('../utils/properties')

// Behavior on <property>: { <Animation> }
// In QML, Behavior watches a property on its parent and runs `animation` on changes.
// Without dedicated compiler support, the implementation exposes the API and lets
// callers trigger the contained animation manually (or via a `targetProperty` watch).

class Behavior extends QtObject {

    static defaultProperties = {
        enabled: { type: QBool, value: true },
        targetProperty: { type: QString, value: '', changed: '$targetPropertyChanged' },
        targetValue: { type: QVar, value: undefined },
        animation: { type: QVar, value: undefined },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$parentItem = parent
        this.$lastValue = undefined
    }

    $complete(){
        super.$complete()
        // Use first Animation in `data` if `animation` not set explicitly
        if(!this.getPropertyValue('animation')){
            const data = this.getPropertyValue('data') || []
            const a = data.find(o => o instanceof Animation)
            if(a) this.getProperty('animation').reset(a)
        }
        this.$attach()
    }

    $targetPropertyChanged(){
        this.$attach()
    }

    $attach(){
        const prop = this.getPropertyValue('targetProperty')
        if(!prop || !this.$parentItem || !this.$parentItem.getProperty) return
        const p = this.$parentItem.getProperty(prop)
        if(!p) return
        this.$lastValue = this.$parentItem.getPropertyValue(prop)
        if(this.$watch) return
        this.$watch = () => {
            if(!this.getPropertyValue('enabled')) return
            const anim = this.getPropertyValue('animation')
            const newVal = this.$parentItem.getPropertyValue(prop)
            if(anim && anim instanceof Animation){
                anim.getProperty && anim.getProperty('target') && anim.getProperty('target').reset(this.$parentItem)
                if(anim.getProperty('property')) anim.getProperty('property').reset(prop)
                if(anim.getProperty('from')) anim.getProperty('from').reset(this.$lastValue)
                if(anim.getProperty('to')) anim.getProperty('to').reset(newVal)
                anim.restart()
            }
            this.$lastValue = newVal
        }
        const sig = p.getSignal && p.getSignal()
        if(sig && typeof sig.connect === 'function') sig.connect(this.$watch)
    }
}

module.exports.Behavior = Behavior
