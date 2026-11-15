const engine = require('../Core/engine')
const { Animation, RUNNING, STOPPED, readProperty, writeProperty } = require('./Animation')
const { setBinding, takeBinding, writeValue } = require('./StateBindings')

// QQuickTransitionManager
class TransitionManager {
    constructor(state){
        this.state = state
        this.transitionInstance = null
        this.bindingsList = []
        this.completeList = []
        this.cancelling = false
    }

    transition(list, transition, defaultTarget){
        this.cancel()
        let applyList = list.slice()
        for(let action of applyList){
            if(action.event) continue
            if(action.toBinding) this.bindingsList.push(action)
            takeBinding(action.p)
        }

        // "Animated transitions need both the start and the end value for each property change": apply
        // every change, read the end values, roll back
        if(transition && this.bindingsList.length){
            for(let action of applyList){
                if(action.event) continue
                if(action.toBinding) setBinding(action.p, action.toBinding)
                else writeProperty(action.p, action.to)
            }
            for(let action of applyList){
                if(action.event) continue
                if(action.toBinding || action.to === undefined) action.to = readProperty(action.p)
            }
            for(let action of applyList){
                if(action.event) continue
                if(action.toBinding) takeBinding(action.p)
                writeProperty(action.p, action.from)
            }
        }

        if(transition){
            let touched = []
            this.transitionInstance = transition
            transition.$prepare(applyList, touched, this, defaultTarget)
            applyList = applyList.filter(action => {
                if(action.event || touched.indexOf(action) < 0) return true
                if(action.to !== action.from) this.completeList.push(action)
                return false
            })
        }

        // what the transition does not animate is applied now; bindings are set at the end
        for(let action of applyList){
            if(action.event) action.event()
            else if(!action.toBinding) writeValue(action.p, action.to)
        }
        if(!transition) this.complete()
    }

    applyBindings(){
        let list = this.bindingsList
        this.bindingsList = []
        for(let action of list){
            if(action.toBinding && !(action.p.obj.$f & engine.DESTROYED)) setBinding(action.p, action.toBinding)
        }
    }

    complete(){
        this.applyBindings()
        let list = this.completeList
        this.completeList = []
        for(let action of list){
            if(!(action.p.obj.$f & engine.DESTROYED)) writeValue(action.p, action.to)
        }
    }

    // the Transition job reports its state (running) here
    $jobState(transition, state){
        let running = state === RUNNING
        if(transition.$v[Animation.RUNNING] !== running){
            transition.$v[Animation.RUNNING] = running
            engine.emit(transition, Animation.SIGNAL_runningChanged, !running, running)
        }
        if(state === STOPPED && !this.cancelling && this.transitionInstance === transition){
            this.transitionInstance = null
            this.complete()
        }
    }

    cancel(){
        let instance = this.transitionInstance
        if(instance && instance.$state !== STOPPED){
            this.cancelling = true
            try {
                instance.$stopJob()
            } finally {
                this.cancelling = false
            }
        }
        this.transitionInstance = null
        for(let action of this.bindingsList){
            if(action.toBinding) takeBinding(action.p)
        }
        this.bindingsList = []
        this.completeList = []
    }
}

module.exports = { TransitionManager }
