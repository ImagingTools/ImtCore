import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Timer extends QtObject {
    constructor(parent) {
        super(parent)

        this.$cP('interval', 1000).connect(this.$timerChanged.bind(this))
        this.$cP('repeat', false).connect(this.$timerChanged.bind(this))
        this.$cP('running', false).connect(this.$timerChanged.bind(this))
        this.$cP('triggeredOnStart', false).connect(this.$timerChanged.bind(this))

        this.$s.triggered = Signal()

        this.$timer = null
    }
    $domCreate(){
        super.$domCreate()

    }

    restart(){
        this.stop()
        this.start()
    }
    start(){
        if(!this.$p.running.val) this.running = true
    }
    stop(){
        if(this.$p.running.val) this.running = false
    }
    $triggered(){
        this.$s.triggered()
        if(this.$p.repeat.val && this.$p.running.val) {
            if(this.$timer) clearTimeout(this.$timer)
            this.$timer = setTimeout(()=>{
                this.$triggered()
            }, this.interval)
        } else {
            this.stop()
        }
    }

    $timerChanged(){
        if(this.$timer) clearTimeout(this.$timer)
        if(this.triggeredOnStart && this.triggered) this.triggered()

        if(this.$p.running.val)
        this.$timer = setTimeout(()=>{
            this.$triggered()
        }, this.interval)
    }

}

QML.Timer = Timer