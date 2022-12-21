import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Animation extends QtObject {
    $ready = true
    constructor(args) {
        super(args)

        this.$cP('loops', 1).connect(this.$loopsChanged.bind(this))
        this.$cP('paused', false).connect(this.$pausedChanged.bind(this))
        this.$cP('running', false).connect(this.$runningChanged.bind(this))
        this.$cP('alwaysRunToEnd', false).connect(this.$runningChanged.bind(this))

        this.$s.finished = Signal()
        this.$s.started = Signal()
        this.$s.stopped = Signal()

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
        
    }
    $pausedChanged(){
        
    }
    $runningChanged(){
        
    }

    complete() { 

    }

	pause() { 
        this.pause = true
    }
	start() {
        this.restart()
        this.$s.started()
    }
	stop() {
		this.running = false
        this.$s.stopped()
    }
	resume() { 
        this.pause = false
    }
	restart() { 
		this.pause = false
        this.running = true
    }

}

QML.Animation = Animation