const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Animation, RUNNING, STOPPED, readProperty, resolveProperty, writeProperty } = require('./Animation')

// ---- Behavior -------------------------------------------------------------------------------

// `Behavior on x { NumberAnimation {} }`: writes to the property go to $intercept (QQuickBehavior::write)
class Behavior extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$property = null
        this.$finalized = false
        this.$animation = null
        engine.scheduleCompleted(this)
    }

    $animationObject(){
        if(this.$animation) return this.$animation
        for(let child of this.$children || []){
            if(child instanceof Animation){
                this.$animation = child
                child.$userControlDisabled = true
                child.$listener = this
                if(this.$property) child.$setDefaultTarget(this.$property.target, this.$property.path)
                break
            }
        }
        return this.$animation
    }

    $setTargetProperty(target, name){
        let p = resolveProperty(target, name)
        if(!p){
            engine.reportError('Cannot assign a Behavior to non-existent property "' + name + '"', 'Behavior')
            return
        }
        this.$property = p
        if(!p.obj.$icpt) p.obj.$icpt = []
        p.obj.$icpt[p.id] = this
        let animation = this.$animationObject()
        if(animation) animation.$setDefaultTarget(target, name)
    }

    $setTarget(target, name){ this.$setTargetProperty(target, name) }

    $complete(){
        this.$finalized = true
        super.$complete()
    }

    $intercept(value){
        let p = this.$property
        let prop = p.obj.constructor.$type.props[p.id]
        if(prop.coerce){
            try { value = prop.coerce(value) } catch(error) {
                engine.reportError(error, p.obj.constructor.name + '.' + prop.name)
                return
            }
        }
        let changed = this.$v[Behavior.TARGETVALUE] !== value
        if(changed) this.$write(Behavior.TARGETVALUE, value)
        let animation = this.$animationObject()
        let bypass = !this.$v[Behavior.ENABLED] || !this.$finalized
        if(!animation || bypass || (animation.$f & engine.DESTROYED)){
            if(animation && animation.$state !== STOPPED) animation.$stopJob()
            writeProperty(p, value)
            return
        }
        let active = animation.$v[Animation.RUNNING]
        if(active && !changed) return
        if(animation.$state !== STOPPED){
            this.$blockRunningChanged = true
            animation.$stopJob()
        }
        let current = readProperty(p)
        if(!active && value === current){
            this.$blockRunningChanged = false
            writeProperty(p, value)
            return
        }
        let actions = [{ p, from: current, to: value }]
        let after = []
        animation.$transition(actions, after, null)
        animation.$startJob()
        this.$blockRunningChanged = false
        if(!after.some(action => action.p.obj === p.obj && action.p.id === p.id)) writeProperty(p, value)
    }

    // the animation of a Behavior reports running through notifyRunningChanged
    $jobState(animation, state){
        if(this.$blockRunningChanged) return
        let running = state === RUNNING
        if(animation.$v[Animation.RUNNING] !== running){
            animation.$v[Animation.RUNNING] = running
            engine.emit(animation, Animation.SIGNAL_runningChanged, !running, running)
        }
    }
}

defineType(Behavior, {
    props: {
        enabled: { kind: 'bool', initial: true },
        targetValue: { kind: 'var', readonly: true },
        animation: { kind: 'var', readonly: true, get(){ return this.$animationObject() } },
    },
})

{
    Object.defineProperty(Behavior.prototype, 'targetProperty', {
        get(){ return this.$property ? { object: this.$property.target, name: this.$property.path } : { object: null, name: '' } },
        configurable: true,
    })
}

module.exports = { Behavior }
