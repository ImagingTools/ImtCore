import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Animation extends QtObject {
    $ready = true
    $loops = 0
    static Infinite = -1
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
        this.$s.started()
    }
	stop() {
		this.running = false
        this.$s.stopped()
    }
	resume() { 
        this.paused = false
    }
	restart() { 
        this.$loops = 0
		this.paused = false
        this.running = true
    }

}

QML.Animation = Animation