const { Item } = require('./Item')
const { QBool, QVar, QPoint } = require('../utils/properties')

class DropArea extends Item {
    static defaultProperties = {
        containsDrag: { type: QBool, value: false, changed: '$containsDragChanged' },
        keys: { type: QVar, value: undefined, changed: '$keysChanged' },
        drag: { type: QPoint, changed: '$dragChanged' },
    }

    static defaultSignals = {
        dropped: { params: ['drop'] },
        entered: { params: ['drag'] },
        exited: { params: [] },
        positionChanged: { params: ['drag'] },

    }

    $droparea = true
    $drags = new Set()

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)

        MouseController.add(this)
        MouseController.addDropArea(this)
    }
    

    $enterOrMove(drag){
        if(!this.$drags.has(drag)){
            this.$drags.add(drag)
            this.getProperty('containsDrag').reset(true)
            if(this.$signals.entered) this.$signals.entered(drag)
        } else {
            if(this.$signals.positionChanged) this.$signals.positionChanged(drag)
        }
    }

    $drop(drop){
        this.$drags.delete(drop)
        if(this.$drags.size === 0) this.getProperty('containsDrag').reset(false)
        if(this.$signals.dropped) this.$signals.dropped(drop)
    }

    $exit(drag){
        this.$drags.delete(drag)
        if(this.$drags.size === 0) this.getProperty('containsDrag').reset(false)
        if(this.$signals.exited) this.$signals.exited(drag)
    }

    destroy(){
        MouseController.remove(this)
        MouseController.removeDropArea(this)
        super.destroy()
    }
}

module.exports.DropArea = DropArea