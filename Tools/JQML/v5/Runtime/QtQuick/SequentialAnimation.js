const { defineType } = require('../Core/type')
const { AnimationGroup, PAUSED, RUNNING, STOPPED } = require('./Animation')

// QSequentialAnimationGroupJob
class SequentialAnimation extends AnimationGroup {
    $duration(){
        let total = 0
        for(let child of this.$animations()){
            let d = child.$totalDuration()
            if(d === -1) return -1
            total += d
        }
        return total
    }

    $stateChanged(state, old){
        if(state === RUNNING && old === STOPPED){
            this.$previousLoop = 0
            this.$current = null
            let first = this.$animations()[0]
            if(first) this.$setCurrent(first, false)
        } else if(state === STOPPED && this.$current){
            this.$current.$stopJob()
            this.$current = null
        } else if(state === PAUSED && this.$current){
            this.$current.$pauseJob()
        } else if(state === RUNNING && old === PAUSED && this.$current){
            this.$current.$resumeJob()
        }
    }

    $setCurrent(anim, intermediate){
        if(anim === this.$current) return
        if(this.$current) this.$current.$stopJob()
        this.$current = anim
        if(!intermediate && this.$state === RUNNING) anim.$startJob()
    }

    $indexForTime(time){
        let offset = 0
        let list = this.$animations()
        for(let i = 0; i < list.length; i++){
            let d = list[i].$totalDuration()
            if(d === -1 || time < offset + d || i === list.length - 1) return { anim: list[i], index: i, offset }
            offset += d
        }
        return { anim: null, index: -1, offset: 0 }
    }

    $updateCurrentTime(time){
        let list = this.$animations()
        if(!this.$current){
            if(list.length) this.$setCurrent(list[0], false)
            else return
        }
        let next = this.$indexForTime(time)
        let currentIndex = list.indexOf(this.$current)
        if(this.$previousLoop < this.$loop){
            for(let i = currentIndex; i >= 0 && i < list.length; i++){
                this.$setCurrent(list[i], true)
                list[i].$setCurrentTime(list[i].$totalDuration())
            }
            this.$current.$stopJob()
            this.$current = null
            this.$setCurrent(list[0], false)
            currentIndex = 0
        }
        for(let i = currentIndex; i >= 0 && i < next.index; i++){
            this.$setCurrent(list[i], true)
            list[i].$setCurrentTime(list[i].$totalDuration())
        }
        this.$setCurrent(next.anim, false)
        if(this.$current) this.$current.$setCurrentTime(time - next.offset)
        this.$previousLoop = this.$loop
    }
}
defineType(SequentialAnimation)

module.exports = { SequentialAnimation }
