import {Animation} from './Animation'

export class PauseAnimation extends Animation {

    constructor(parent) {
        super(parent)

        this.$cP('duration', 250).connect(this.$durationChanged.bind(this))
        
    }   

    $tick(){
        if(this.$p.running.val && !this.$p.paused.val && this.$duration > 0){
            this.$duration -= 1000/60

            if(this.$duration < 0) {
                this.running = false
                this.$s.finished()
            }
        }
    }

    $durationChanged(){
        this.$duration = this.duration
    }

    restart() { 
        this.$duration = this.duration
        super.restart()
    }
}

QML.PauseAnimation = PauseAnimation