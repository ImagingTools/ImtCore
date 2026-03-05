import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Timer extends QtObject {
    constructor(args) {
        super(args)

        this.$cP('interval', 1000, this.$timerChanged)
        this.$cP('repeat', false, this.$timerChanged)
        this.$cP('running', false, this.$timerChanged)
        this.$cP('triggeredOnStart', false, this.$timerChanged)

        this.$cS('triggered')
    
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
        this.triggered()
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

    $destroy(){
        clearTimeout(this.$timer)
        super.$destroy()
    }

}

QML.Timer = Timer