import {Animation} from './Animation'

export class PauseAnimation extends Animation {

    constructor(args) {
        super(args)

        this.$cP('duration', 250, this.$durationChanged)
        
    }   

    $tick(){
        if(this.running && !this.paused && this.$duration > 0){
            this.$duration -= 1000/60

            if(this.$duration < 0) {
                this.running = false
                this.finished()
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