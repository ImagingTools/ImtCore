const engine = require('../Core/engine')
const { Item } = require('./Item')
const { State } = require('./State')
const { Transition } = require('./Transition')

// ---- state group ----------------------------------------------------------------------------

// host: an Item (state / states / transitions) or a StateGroup; ids: the host's property ids
class StateMachine {
    constructor(host, ids){
        this.$host = host
        this.ids = ids
        this.current = ''
        this.applying = false
        this.complete = false
        this.unnamed = 0
        this.nullState = null
    }

    states(){
        let list = this.$host.$v[this.ids.STATES]
        return Array.isArray(list) ? list.filter(s => s instanceof State) : list instanceof State ? [list] : []
    }

    transitions(){
        let list = this.$host.$v[this.ids.TRANSITIONS]
        return Array.isArray(list) ? list.filter(t => t instanceof Transition) : list instanceof Transition ? [list] : []
    }

    $find(name){
        for(let state of this.states()) if(state.$v[State.NAME] === name) return state
        return null
    }

    statesChanged(){
        for(let state of this.states()) state.$group = this
        this.$updateAutoState()
    }

    componentComplete(){
        this.complete = true
        let names = []
        for(let state of this.states()){
            state.$group = this
            if(!state.$isNamed()) state.$v[State.NAME] = 'anonymousState' + (++this.unnamed)
            let name = state.$v[State.NAME]
            if(names.indexOf(name) >= 0) console.warn('Found duplicate state name: ' + name)
            else names.push(name)
        }
        if(this.$updateAutoState()) return
        if(this.current){
            let name = this.current
            this.current = ''
            this.setCurrentStateInternal(name, true)
        }
    }

    // the first state whose when is true wins; the active automatic state reverts to "" once false
    $updateAutoState(){
        if(!this.complete) return false
        let revert = false
        for(let state of this.states()){
            if(!state.$whenKnown || !state.$isNamed()) continue
            if(state.when){
                if(this.current !== state.$v[State.NAME]){
                    this.setState(state.$v[State.NAME])
                    return true
                }
                return false
            } else if(state.$v[State.NAME] === this.current){
                revert = true
            }
        }
        if(revert){
            let changed = this.current !== ''
            this.setState('')
            return changed
        }
        return false
    }

    setState(name){
        name = name === undefined || name === null ? '' : String(name)
        if(this.current === name) return
        this.setCurrentStateInternal(name, false)
    }

    findTransition(from, to){
        let highest = null, score = 0, reversed = false
        let split = (text) => String(text).split(',').map(s => s.trim())
        for(let t of this.transitions()){
            if(!t.$v[Transition.ENABLED]) continue
            let done = false
            for(let pass = 0; pass < 2; pass++){
                if(pass && (!t.$v[Transition.REVERSIBLE] || (t.$v[Transition.FROM] === '*' && t.$v[Transition.TO] === '*'))) break
                let fromStates = split(t.$v[Transition.FROM]), toStates = split(t.$v[Transition.TO])
                if(pass) [fromStates, toStates] = [toStates, fromStates]
                let s = 0
                if(fromStates.indexOf(from) >= 0) s += 2
                else if(fromStates.indexOf('*') >= 0) s += 1
                else continue
                if(toStates.indexOf(to) >= 0) s += 2
                else if(toStates.indexOf('*') >= 0) s += 1
                else continue
                if(s === 4){
                    highest = t
                    reversed = pass === 1
                    done = true
                    break
                } else if(s > score){
                    score = s
                    highest = t
                    reversed = pass === 1
                }
            }
            if(done) break
        }
        if(highest) highest.$reversed = reversed
        return highest
    }

    setCurrentStateInternal(name, ignoreTransition){
        let host = this.$host, id = this.ids.STATE
        if(!this.complete){
            this.current = name
            host.$v[id] = name
            return
        }
        if(this.applying){
            console.warn("Can't apply a state change as part of a state definition.")
            return
        }
        this.applying = true
        try {
            let transition = ignoreTransition ? null : this.findTransition(this.current, name)
            let oldState = this.current ? this.$find(this.current) : null
            let old = this.current
            this.current = name
            host.$v[id] = name
            engine.emit(host, host.constructor.$type.props[id].signal, old, name)
            let newState = this.$find(name)
            if(!oldState || !newState){
                if(!this.nullState){
                    this.nullState = new State(null)
                    this.nullState.$group = this
                }
                if(!oldState) oldState = this.nullState
                if(!newState) newState = this.nullState
            }
            newState.$apply(transition, oldState)
        } finally {
            this.applying = false
        }
    }
}

function machine(host, ids){
    return host.$stateMachine || (host.$stateMachine = new StateMachine(host, ids))
}

// Item.state / states / transitions are the item's own state group
Item.$states = {
    write(item, value){ machine(item, Item).setState(value) },
    statesChanged(item){
        machine(item, Item).statesChanged()
        engine.scheduleComponentComplete(item)
    },
    componentComplete(item){
        if(item.$stateMachine) item.$stateMachine.componentComplete()
    },
}

module.exports = { StateMachine, machine }
