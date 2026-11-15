const engine = require('../Core/engine')
const { QtObject, writeBypassingInterceptor } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')

const STOPPED = 0, PAUSED = 1, RUNNING = 2

// ---- animation clock (QQmlAnimationTimer) ---------------------------------------------------

const clock = {
    jobs: new Set(),
    handle: 0,
    now: () => (typeof performance !== 'undefined' ? performance.now() : Date.now()),
    manual: false,     // tests drive the clock with tick()
    tick: () => tick(),
}

function scheduleTick(){
    if(clock.manual || clock.handle || !clock.jobs.size) return
    if(typeof requestAnimationFrame === 'function') clock.handle = requestAnimationFrame(tick)
    else clock.handle = setTimeout(tick, 16)
}

function tick(){
    clock.handle = 0
    if(engine.isStopped()){
        clock.jobs.clear()
        return
    }
    let now = clock.now()
    engine.batch(() => {
        for(let job of Array.from(clock.jobs)){
            if(job.$state === RUNNING && clock.jobs.has(job)) job.$setCurrentTime(now - job.$startTime)
        }
    })
    scheduleTick()
}

function register(job){
    job.$startTime = clock.now() - job.$totalTime
    clock.jobs.add(job)
    scheduleTick()
}

function unregister(job){
    clock.jobs.delete(job)
}

// ---- property access for animations ---------------------------------------------------------

// "font.pixelSize" -> { obj: font group, id }
function resolveProperty(target, name){
    if(!target || !name) return null
    let parts = String(name).split('.')
    let obj = target
    for(let i = 0; i < parts.length - 1; i++){
        obj = obj[parts[i]]
        if(!obj) return null
    }
    let type = obj.constructor && obj.constructor.$type
    let prop = type ? type.propByName.get(parts[parts.length - 1]) : null
    if(!prop || prop.kind === 'group') return null
    return { obj, id: prop.id, kind: prop.kind, name: parts[parts.length - 1], target, path: name }
}

function readProperty(p){
    return p.obj.$v[p.id]
}

// QQmlPropertyPrivate::write(..., BypassInterceptor | DontRemoveBinding)
function writeProperty(p, value){
    if(p.obj instanceof Item){
        if(p.id === Item.WIDTH) p.obj.$explicit |= 1
        if(p.id === Item.HEIGHT) p.obj.$explicit |= 2
    }
    writeBypassingInterceptor(p.obj, p.id, value)
}

// ---- Animation ------------------------------------------------------------------------------

class Animation extends QtObject {
    constructor(parent = null){
        super(null)
        this.$state = STOPPED
        this.$time = 0
        this.$totalTime = 0
        this.$loop = 0
        this.$startTime = 0
        this.$group = null
        this.$userControlDisabled = false
        this.$needsDeferredRun = false
        this.$avoidValueSourceStart = false
        this.$defaultProperty = null
        this.$listener = null          // Behavior / Transition watching this job
        this.$loopLimit = -2           // alwaysRunToEnd: loops left for the current run
        if(parent) this.$setParent(parent)
        engine.scheduleComponentComplete(this)
    }

    $parentChanged(old, parent){
        this.$group = parent instanceof AnimationGroup ? parent : null
    }

    // ---- job ----

    $duration(){ return 0 }

    $loops(){
        return this.$loopLimit !== -2 ? this.$loopLimit : this.$v[Animation.LOOPS]
    }

    $totalDuration(){
        let dura = this.$duration(), loops = this.$loops()
        return dura <= 0 ? dura : loops < 0 ? -1 : dura * loops
    }

    // QAbstractAnimationJob::setCurrentTime (forward direction)
    $setCurrentTime(msecs){
        msecs = Math.max(msecs, 0)
        let dura = this.$duration(), loops = this.$loops()
        let totalDura = dura <= 0 ? dura : loops < 0 ? -1 : dura * loops
        if(totalDura !== -1) msecs = Math.min(totalDura, msecs)
        this.$totalTime = msecs
        let oldLoop = this.$loop
        let loop = dura <= 0 ? 0 : Math.floor(msecs / dura)
        if(loop === loops){
            this.$time = Math.max(0, dura)
            loop = Math.max(0, loops - 1)
        } else {
            this.$time = dura <= 0 ? msecs : msecs % dura
        }
        this.$loop = loop
        if(loop !== oldLoop && !this.$group) this.$topLevelLoopChanged()
        this.$updateCurrentTime(this.$time)
        if(this.$totalTime === totalDura) this.$stopJob(true)
    }

    $updateCurrentTime(time){}

    // a top level loop starts: animations with one loop take their start value again
    $topLevelLoopChanged(){
        if(this.$v[Animation.LOOPS] === 1) this.$fromIsSourced = false
    }

    // QAbstractAnimationJob::setState(Running) from Stopped
    $startJob(){
        if(this.$state === RUNNING) return
        if(this.$loops() === 0) return
        let old = this.$state
        if(old === STOPPED){
            this.$time = this.$totalTime = 0
            this.$loop = 0
        }
        this.$state = RUNNING
        let topLevel = !this.$group || this.$group.$state === STOPPED
        if(topLevel) register(this)
        if(old === STOPPED && !this.$group) this.$topLevelLoopChanged()
        this.$stateChanged(RUNNING, old)
        if(this.$state !== RUNNING) return
        if(this.$listener) this.$listener.$jobState(this, RUNNING)
        if(old === STOPPED && topLevel) this.$setCurrentTime(this.$totalTime)
    }

    $stopJob(natural = false){
        if(this.$state === STOPPED) return
        let old = this.$state
        this.$state = STOPPED
        unregister(this)
        this.$stateChanged(STOPPED, old)
        if(this.$listener) this.$listener.$jobState(this, STOPPED)
        if(natural) this.$jobFinished()
    }

    $pauseJob(){
        if(this.$state !== RUNNING) return
        this.$state = PAUSED
        unregister(this)
        this.$stateChanged(PAUSED, RUNNING)
    }

    $resumeJob(){
        if(this.$state !== PAUSED) return
        this.$state = RUNNING
        if(!this.$group) register(this)
        this.$stateChanged(RUNNING, PAUSED)
    }

    $stateChanged(state, old){}

    // QQuickAbstractAnimationPrivate::animationFinished: only for root animations under user control
    $jobFinished(){
        if(this.$group || this.$userControlDisabled) return
        this.$setRunning(false)
        if(this.$v[Animation.ALWAYSRUNTOEND]){
            engine.emit(this, Animation.SIGNAL_stopped)
            this.$loopLimit = -2
        }
        engine.emit(this, Animation.SIGNAL_finished)
    }

    // Builds the run for a set of state actions (QQuickAbstractAnimation::transition)
    $transition(actions, modified, defaultTarget){}

    // ---- QQuickAbstractAnimation ----

    $componentComplete(){
        super.$componentComplete()
        if(this.$needsDeferredRun){
            if(this.$v[Animation.RUNNING]){
                this.$v[Animation.RUNNING] = false
                this.$setRunning(true)
            }
            if(this.$v[Animation.PAUSED]){
                this.$v[Animation.PAUSED] = false
                this.$setPaused(true)
            }
        }
    }

    $setRunning(r){
        r = !!r
        if(!(this.$f & engine.COMPLETED)){
            this.$v[Animation.RUNNING] = r
            if(!r) this.$avoidValueSourceStart = true
            else this.$needsDeferredRun = true
            return
        }
        if(this.$v[Animation.RUNNING] === r) return
        if(this.$group || this.$userControlDisabled){
            console.warn('setRunning() cannot be used on non-root animation nodes.')
            return
        }
        this.$v[Animation.RUNNING] = r
        if(r){
            let suppress = false
            if(this.$v[Animation.ALWAYSRUNTOEND] && this.$v[Animation.LOOPS] !== 1 && this.$state === RUNNING){
                // restarted before the final loop finished: continue instead of restarting
                this.$loopLimit = this.$v[Animation.LOOPS] < 0 ? -1 : this.$loop + this.$v[Animation.LOOPS]
                suppress = true
            }
            if(!suppress) this.$commence()
        } else {
            if(this.$v[Animation.PAUSED]){
                this.$v[Animation.PAUSED] = false
                engine.emit(this, Animation.SIGNAL_pausedChanged, true, false)
            }
            if(this.$state !== STOPPED || this.$commenced){
                if(this.$v[Animation.ALWAYSRUNTOEND]){
                    if(this.$v[Animation.LOOPS] !== 1) this.$loopLimit = this.$loop + 1
                } else {
                    this.$stopJob(false)
                    engine.emit(this, Animation.SIGNAL_stopped)
                }
            }
        }
        if(r === this.$v[Animation.RUNNING]) engine.emit(this, Animation.SIGNAL_runningChanged, !r, r)
        else this.$v[Animation.RUNNING] = this.$state === RUNNING
    }

    $commence(){
        this.$transition([], [], null)
        this.$commenced = true
        this.$loopLimit = -2
        engine.emit(this, Animation.SIGNAL_started)
        this.$startJob()
    }

    $setPaused(p){
        p = !!p
        if(this.$v[Animation.PAUSED] === p) return
        if(!this.$v[Animation.RUNNING]){
            console.warn("setPaused() cannot be used when animation isn't running.")
            return
        }
        if(this.$group || this.$userControlDisabled){
            console.warn('setPaused() cannot be used on non-root animation nodes.')
            return
        }
        this.$v[Animation.PAUSED] = p
        if(!(this.$f & engine.COMPLETED)) return
        if(p) this.$pauseJob()
        else this.$resumeJob()
        engine.emit(this, Animation.SIGNAL_pausedChanged, !p, p)
    }

    // value source (`NumberAnimation on x`) and Behavior: the property to animate by default
    $setDefaultTarget(target, name){
        this.$defaultProperty = resolveProperty(target, name)
    }

    $setValueSource(target, name){
        this.$setDefaultTarget(target, name)
        if(!this.$avoidValueSourceStart) this.$setRunning(true)
    }

    // `NumberAnimation on x`: a property value source
    $setTarget(target, name){ this.$setValueSource(target, name) }

    start(){ this.$setRunning(true) }
    stop(){ this.$setRunning(false) }
    pause(){ this.$setPaused(true) }
    resume(){ this.$setPaused(false) }
    restart(){
        this.stop()
        this.start()
    }
    complete(){
        if(this.$v[Animation.RUNNING] && this.$state !== STOPPED) this.$setCurrentTime(this.$duration())
    }

    $disposeNative(){
        unregister(this)
        this.$state = STOPPED
        super.$disposeNative()
    }
}

defineType(Animation, {
    props: {
        running: { kind: 'bool', set(value){ this.$setRunning(value) } },
        paused: { kind: 'bool', set(value){ this.$setPaused(value) } },
        alwaysRunToEnd: 'bool',
        loops: { kind: 'int', initial: 1 },
    },
    signals: ['started', 'stopped', 'finished'],
})
Animation.Infinite = -1

// ---- groups ---------------------------------------------------------------------------------

class AnimationGroup extends Animation {
    $animations(){
        let list = []
        for(let child of this.$children || []){
            if(child instanceof Animation) list.push(child)
        }
        return list
    }

    get animations(){ return this.$animations() }

    $transition(actions, modified, defaultTarget){
        for(let child of this.$animations()) child.$transition(actions, modified, defaultTarget)
    }

    $topLevelLoopChanged(){
        for(let child of this.$animations()) child.$topLevelLoopChanged()
    }
}
defineType(AnimationGroup)

module.exports = { STOPPED, PAUSED, RUNNING, clock, resolveProperty, readProperty, writeProperty, Animation, AnimationGroup }
