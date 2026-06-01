const { Animation } = require('./Animation')

function childAnimations(self){
    const data = self.getPropertyValue('data') || []
    return data.filter(o => o instanceof Animation)
}

class ParallelAnimation extends Animation {

    $animationContainer = true

    tick(){
        if(!this.getPropertyValue('running') || this.getPropertyValue('paused')) return
        const kids = childAnimations(this)
        if(kids.length === 0){
            this.getProperty('running').reset(false)
            if(this.$signals.stopped) this.$signals.stopped()
            if(this.$signals.finished) this.$signals.finished()
            return
        }
        let anyRunning = false
        for(const k of kids){
            if(k.getPropertyValue('running')) { anyRunning = true; break }
        }
        if(!anyRunning){
            const loops = this.$loops + 1
            if(loops < this.getPropertyValue('loops') || this.getPropertyValue('loops') === Animation.Infinite){
                this.$loops = loops
                for(const k of kids) k.start()
            } else {
                this.getProperty('running').reset(false)
                if(this.$signals.stopped) this.$signals.stopped()
                if(this.$signals.finished) this.$signals.finished()
            }
        }
    }

    restart(){
        super.restart()
        for(const k of childAnimations(this)) k.start()
    }

    stop(){
        for(const k of childAnimations(this)) k.stop()
        super.stop()
    }

    pause(){
        for(const k of childAnimations(this)) k.pause()
        super.pause()
    }

    resume(){
        for(const k of childAnimations(this)) k.resume()
        super.resume()
    }
}

module.exports.ParallelAnimation = ParallelAnimation
