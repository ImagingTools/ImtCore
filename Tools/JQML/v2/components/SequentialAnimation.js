const { Animation } = require('./Animation')

function childAnimations(self){
    const data = self.getPropertyValue('data') || []
    return data.filter(o => o instanceof Animation)
}

class SequentialAnimation extends Animation {

    $animationContainer = true

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$currentIndex = -1
    }

    tick(){
        if(!this.getPropertyValue('running') || this.getPropertyValue('paused')) return
        const kids = childAnimations(this)
        if(kids.length === 0){
            this.getProperty('running').reset(false)
            if(this.$signals.stopped) this.$signals.stopped()
            if(this.$signals.finished) this.$signals.finished()
            return
        }
        if(this.$currentIndex < 0){
            this.$currentIndex = 0
            kids[0].start()
            return
        }
        const cur = kids[this.$currentIndex]
        if(cur && cur.getPropertyValue('running')) return

        this.$currentIndex++
        if(this.$currentIndex < kids.length){
            kids[this.$currentIndex].start()
            return
        }

        const loops = this.$loops + 1
        if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
            this.$loops = loops
            this.$currentIndex = 0
            kids[0].start()
        } else {
            this.getProperty('running').reset(false)
            if(this.$signals.stopped) this.$signals.stopped()
            if(this.$signals.finished) this.$signals.finished()
        }
    }

    restart(){
        for(const k of childAnimations(this)) k.stop()
        this.$currentIndex = -1
        super.restart()
    }

    stop(){
        for(const k of childAnimations(this)) k.stop()
        this.$currentIndex = -1
        super.stop()
    }
}

module.exports.SequentialAnimation = SequentialAnimation
