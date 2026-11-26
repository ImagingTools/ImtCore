const { defineType } = require('../Core/type')
const { readProperty, writeProperty } = require('./Animation')
const { NumberAnimation } = require('./NumberAnimation')
const { PropertyAnimation } = require('./PropertyAnimation')

const EASED = 0, IMMEDIATE = 1, SYNC = 2

// SmoothedAnimation (qquicksmoothedanimation.cpp of Qt 6.12, QSmoothedAnimation): each property goes to its value
// with a velocity profile that accelerates and decelerates (over maximumEasingTime when it is set, else over the whole
// run), at `velocity` or in `duration` (the quicker of the two); a new target while running keeps the current velocity
// (restart: initialVelocity = trackVelocity), reversing as reversingMode says. The job has no duration: it stops when
// every property arrived.
class SmoothedAnimation extends NumberAnimation {
    constructor(parent = null){
        super(parent)
        this.$v[PropertyAnimation.DURATION] = -1
        // the velocity each property moves at now (trackVelocity), kept for a restart
        this.$tracks = new Map()
    }

    $duration(){ return -1 }

    $trackOf(p){
        let key = p.obj
        let map = this.$tracks.get(key)
        if(!map){
            map = new Map()
            this.$tracks.set(key, map)
        }
        return map
    }

    $stateChanged(state, old){
        super.$stateChanged(state, old)
        if(state === 2 && old !== 1) this.$init()
    }

    // QSmoothedAnimation::init for each property
    $init(){
        let velocity = Number(this.$v[SmoothedAnimation.VELOCITY])
        let userDuration = Number(this.$v[PropertyAnimation.DURATION])
        let maximumEasingTime = Number(this.$v[SmoothedAnimation.MAXIMUMEASINGTIME])
        let reversingMode = this.$v[SmoothedAnimation.REVERSINGMODE] || EASED
        let running = 0
        for(let action of this.$actions){
            let tracks = this.$trackOf(action.p)
            let track = tracks.get(action.p.id) || { velocity: 0, invert: false }
            tracks.set(action.p.id, track)
            let trackVelocity = track.velocity
            let initialVelocity = trackVelocity
            action.lastTime = this.$time
            action.done = true
            if(velocity === 0) continue
            if(action.from !== undefined && this.$fromIsDefined) writeProperty(action.p, action.from)
            let initialValue = Number(readProperty(action.p))
            let to = Number(action.to)
            if(to === initialValue){
                track.velocity = 0
                continue
            }
            let hasReversed = trackVelocity !== 0 && (!track.invert) === (initialValue - to > 0)
            if(hasReversed){
                if(reversingMode === SYNC){
                    writeProperty(action.p, to)
                    track.velocity = 0
                    continue
                }
                initialVelocity = reversingMode === IMMEDIATE ? 0 : -trackVelocity
            }
            action.initialValue = initialValue
            action.invert = track.invert = to < initialValue
            track.velocity = initialVelocity
            if(!recalc(action, to, initialVelocity, velocity, userDuration, maximumEasingTime)){
                writeProperty(action.p, to)
                track.velocity = 0
                continue
            }
            action.done = false
            running++
        }
        if(!running) this.$finishSoon()
    }

    // QSmoothedAnimation::updateCurrentTime: each property where easeFollow puts it at the time since init
    $updateCurrentTime(time){
        if(this.$finishing) return
        let left = 0
        for(let action of this.$actions){
            if(action.done || (action.p.obj.$f & 4)) continue
            let track = this.$trackOf(action.p).get(action.p.id)
            let seconds = (time - action.lastTime) / 1000
            let { value, velocity, finished } = easeFollow(action, seconds)
            track.velocity = velocity
            writeProperty(action.p, action.initialValue + value * (action.invert ? -1 : 1))
            if(finished) action.done = true
            else left++
        }
        if(!left) this.$finishSoon()
    }

    // complete(): every property at its value now, the job stopped (the job has no duration to run to)
    complete(){
        if(!this.$v[PropertyAnimation.RUNNING] || this.$state === 0) return
        for(let action of this.$actions){
            if(action.done || (action.p.obj.$f & 4)) continue
            writeProperty(action.p, Number(action.to))
            let track = this.$trackOf(action.p).get(action.p.id)
            if(track) track.velocity = 0
            action.done = true
        }
        this.$stopJob(true)
    }

    // delayedStop: stopped once every property arrived (after the frame)
    $finishSoon(){
        if(this.$finishing) return
        this.$finishing = true
        Promise.resolve().then(() => {
            this.$finishing = false
            if(this.$state !== 2) return
            if(this.$actions.every(a => a.done)) this.$stopJob(true)
        })
    }
}

// QSmoothedAnimation::recalc: the times and the speeds of the profile for the distance s
function recalc(action, to, vi, velocity, userDuration, maximumEasingTime){
    let s = (to - action.initialValue) * (action.invert ? -1 : 1)
    let tf
    if(userDuration >= 0 && velocity > 0){
        tf = s / velocity
        if(tf > userDuration / 1000) tf = userDuration / 1000
    } else if(userDuration >= 0){
        tf = userDuration / 1000
    } else if(velocity > 0){
        tf = s / velocity
    } else return false
    let a, d, tp, td, vp, sp, sd
    if(maximumEasingTime === 0){
        a = 0; d = 0; tp = 0; td = tf; vp = velocity; sp = 0; sd = s
    } else if(maximumEasingTime !== -1 && tf > maximumEasingTime / 1000){
        let ta = maximumEasingTime / 1000 / 2
        a = (s - (vi * tf - 0.5 * vi * ta)) / (tf * ta - ta * ta)
        vp = vi + a * ta
        d = vp / ta
        tp = ta
        sp = vi * ta + 0.5 * a * tp * tp
        sd = sp + vp * (tf - 2 * ta)
        td = tf - ta
    } else {
        let c1 = 0.25 * tf * tf
        let c2 = 0.5 * vi * tf - s
        let c3 = -0.25 * vi * vi
        let a1 = (-c2 + Math.sqrt(c2 * c2 - 4 * c1 * c3)) / (2 * c1)
        let tp1 = 0.5 * tf - 0.5 * vi / a1
        let vp1 = a1 * tp1 + vi
        let sp1 = 0.5 * a1 * tp1 * tp1 + vi * tp1
        a = a1; d = a1; tp = tp1; td = tp1; vp = vp1; sp = sp1; sd = sp1
    }
    Object.assign(action, { s, vi, tf, a, d, tp, td, vp, sp, sd })
    return true
}

// QSmoothedAnimation::easeFollow: the distance gone at the time, and the velocity then
function easeFollow(action, t){
    let { s, vi, tf, a, d, tp, td, vp, sp, sd } = action
    if(t < tp) return { value: 0.5 * a * t * t + vi * t, velocity: vi + t * a, finished: false }
    if(t < td) return { value: sp + (t - tp) * vp, velocity: vp, finished: false }
    if(t < tf){
        let u = t - td
        return { value: sd - 0.5 * d * u * u + vp * u, velocity: vp - u * a, finished: false }
    }
    return { value: s, velocity: 0, finished: true }
}

defineType(SmoothedAnimation, {
    props: {
        velocity: { kind: 'real', initial: 200 },
        reversingMode: 'int',
        maximumEasingTime: { kind: 'real', initial: -1 },
    },
})
Object.assign(SmoothedAnimation, { Eased: EASED, Immediate: IMMEDIATE, Sync: SYNC })

module.exports = { SmoothedAnimation }
