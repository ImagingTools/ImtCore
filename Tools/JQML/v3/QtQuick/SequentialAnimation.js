const Animation = require("./Animation")

function childAnimations(self){
    return (self.__children || []).filter(o => o instanceof Animation)
}

class SequentialAnimation extends Animation {

    __animationContainer = true
    __currentIndex = -1

    __tick(){
        if(!this.running || this.paused) return
        const kids = childAnimations(this)
        if(kids.length === 0){
            this.running = false
            this.stopped()
            this.finished()
            return
        }
        if(this.__currentIndex < 0){
            this.__currentIndex = 0
            kids[0].start()
            return
        }
        const cur = kids[this.__currentIndex]
        if(cur && cur.running) return

        this.__currentIndex++
        if(this.__currentIndex < kids.length){
            kids[this.__currentIndex].start()
            return
        }

        const loops = this.__loopCounter + 1
        if(loops < this.loops || this.loops === Animation.Infinite){
            this.__loopCounter = loops
            this.__currentIndex = 0
            kids[0].start()
        } else {
            this.running = false
            this.stopped()
            this.finished()
        }
    }

    restart(){
        for(const k of childAnimations(this)) k.stop()
        this.__currentIndex = -1
        super.restart()
    }

    stop(){
        for(const k of childAnimations(this)) k.stop()
        this.__currentIndex = -1
        super.stop()
    }
}

module.exports = SequentialAnimation
