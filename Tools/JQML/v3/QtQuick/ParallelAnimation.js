const Animation = require("./Animation")

function childAnimations(self){
    return (self.__children || []).filter(o => o instanceof Animation)
}

class ParallelAnimation extends Animation {

    __animationContainer = true

    __tick(){
        if(!this.running || this.paused) return
        const kids = childAnimations(this)
        if(kids.length === 0){
            this.running = false
            this.stopped()
            this.finished()
            return
        }
        let anyRunning = false
        for(const k of kids){
            if(k.running) { anyRunning = true; break }
        }
        if(!anyRunning){
            const loops = this.__loopCounter + 1
            if(loops < this.loops || this.loops === Animation.Infinite){
                this.__loopCounter = loops
                for(const k of kids) k.start()
            } else {
                this.running = false
                this.stopped()
                this.finished()
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

module.exports = ParallelAnimation
