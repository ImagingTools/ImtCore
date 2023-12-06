const { QtObject } = require('./QtObject')
const { QInt, QBool } = require('../utils/properties')

class Animation extends QtObject {
    static Infinite = -1

    static defaultProperties = {
        loops: { type: QInt, value: 1, changed: '$loopsChanged' },
        running: { type: QBool, value: false, changed: '$runningChanged' },
        paused: { type: QBool, value: false, changed: '$pausedChanged' },
        alwaysRunToEnd: { type: QBool, value: false, changed: '$alwaysRunToEndChanged' },

    }

    static defaultSignals = {
        started: { params: [] },
        stopped: { params: [] },
        finished: { params: [] },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)
        this.$loops = 0

        AnimationController.add(this)
    }


    tick() {

    }
    $alwaysRunToEndChanged() {

    }
    $loopsChanged() {
        this.$loops = 0
    }
    $pausedChanged() {

    }
    $runningChanged() {

    }

    pause() {
        this.getProperty('paused').reset(true)
    }
    start() {
        this.restart()

    }
    stop() {
        this.getProperty('running').reset(false)
        if(this.$signals.stopped) this.$signals.stopped()
    }
    resume() {
        this.getProperty('paused').reset(false)
    }
    restart() {
        this.$loops = 0
        this.getProperty('paused').reset(false)
        this.getProperty('running').reset(true)
        if(this.$signals.started) this.$signals.started()
    }

    $destroy(){
        AnimationController.remove(this)
        super.$destroy()
    }

}

module.exports.Animation = Animation