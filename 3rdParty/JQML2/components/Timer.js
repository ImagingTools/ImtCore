const { QtObject } = require('./QtObject')
const { QReal, QBool } = require('../utils/properties')

class Timer extends QtObject {
    static defaultProperties = {
        interval: { type: QReal, value: 1000, changed: 'timerChanged' },
        repeat: { type: QBool, value: false, changed: 'timerChanged' },
        running: { type: QBool, value: false, changed: 'timerChanged' },
        triggeredOnStart: { type: QBool, value: false, changed: 'timerChanged' },
    }

    static defaultSignals = {
        triggered: { params: [] },
    }

    constructor(parent){
        super(parent)
    }

    restart(){
        this.stop()
        this.start()
    }
    start(){
        if(!this.getPropertyValue('running')) this.getProperty('running').reset(true)
    }
    stop(){
        if(this.getPropertyValue('running')) this.getProperty('running').reset(false)
    }
    $triggered(){
        if(this.$signals.triggered) this.$signals.triggered()
        if(this.getPropertyValue('repeat') && this.getPropertyValue('running')) {
            if(this.$timer) clearTimeout(this.$timer)
            this.$timer = setTimeout(()=>{
                this.$triggered()
            }, this.getPropertyValue('interval'))
        } else {
            this.stop()
        }
    }

    timerChanged(){
        if(this.$timer) clearTimeout(this.$timer)
        if(this.getPropertyValue('triggeredOnStart') && this.triggered) this.triggered()

        if(this.getPropertyValue('running'))
        this.$timer = setTimeout(()=>{
            this.$triggered()
        }, this.getPropertyValue('interval'))
    }

    $destroy(){
        clearTimeout(this.$timer)
        delete this.$timer
        super.$destroy()
    }
    
}

module.exports.Timer = Timer