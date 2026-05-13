import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Animation extends QtObject {
    $ready = true
    $loops = 0
    static Infinite = -1
    constructor(args) {
        super(args)

        this.$cP('loops', 1, this.$loopsChanged)
        this.$cP('paused', false, this.$pausedChanged)
        this.$cP('running', false, this.$runningChanged)
        this.$cP('alwaysRunToEnd', false, this.$runningChanged)

        this.$cS('finished')
        this.$cS('started')
        this.$cS('stopped')

        Core.animations[this.UID] = this
    }
    $destroy(){
        delete Core.animations[this.UID]
        super.$destroy()
    }
    $tick(){

    }
    $alwaysRunToEndChanged(){
        
    }
    $loopsChanged(){
        this.$loops = 0
    }
    $pausedChanged(){
        
    }
    $runningChanged(){
        
    }

    complete() { 

    }

	pause() { 
        this.paused = true
    }
	start() {
        this.restart()
        
    }
	stop() {
		this.running = false
        this.stopped()
    }
	resume() { 
        this.paused = false
    }
	restart() { 
        this.$loops = 0
		this.paused = false
        this.running = true
        this.started()
    }

}

QML.Animation = Animation