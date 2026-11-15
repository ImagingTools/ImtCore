const { defineType } = require('../Core/type')
const { AnimationGroup, PAUSED, RUNNING, STOPPED } = require('./Animation')

// QParallelAnimationGroupJob
class ParallelAnimation extends AnimationGroup {
    $duration(){
        let total = 0
        for(let child of this.$animations()){
            let d = child.$totalDuration()
            if(d === -1) return -1
            total = Math.max(total, d)
        }
        return total
    }

    $stateChanged(state, old){
        let list = this.$animations()
        if(state === RUNNING && old === STOPPED){
            this.$previousLoop = 0
            for(let child of list){
                child.$stopJob()
                child.$startJob()
            }
        } else if(state === STOPPED){
            for(let child of list) child.$stopJob()
        } else if(state === PAUSED){
            for(let child of list) child.$pauseJob()
        } else if(state === RUNNING && old === PAUSED){
            for(let child of list) child.$resumeJob()
        }
    }

    $updateCurrentTime(time){
        let list = this.$animations()
        if(this.$loop > this.$previousLoop){
            let dura = this.$duration()
            if(dura > 0){
                for(let child of list) if(child.$state !== STOPPED) child.$setCurrentTime(dura)
            }
        }
        for(let child of list){
            let dura = child.$totalDuration()
            if(this.$loop > this.$previousLoop || (child.$state === STOPPED && (dura === -1 || time < dura))){
                child.$stopJob()
                child.$startJob()
            }
            if(child.$state === this.$state){
                child.$setCurrentTime(time)
                if(dura > 0 && time > dura) child.$stopJob()
            }
        }
        this.$previousLoop = this.$loop
    }
}
defineType(ParallelAnimation)

module.exports = { ParallelAnimation }
