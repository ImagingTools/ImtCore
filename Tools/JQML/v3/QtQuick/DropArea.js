const Item = require("./Item")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")
const JQApplication = require("../core/JQApplication")

class DropArea extends Item {
    static meta = Object.assign({}, Item.meta, {
        containsDrag: { type: Bool, value: false },
        keys:         { type: Var,  value: undefined },
        drag:         { type: Var,  value: undefined },

        containsDragChanged: { type: Signal, args: [] },
        keysChanged:         { type: Signal, args: [] },

        entered:         { type: Signal, args: ['drag'] },
        exited:          { type: Signal, args: [] },
        dropped:         { type: Signal, args: ['drop'] },
        positionChanged: { type: Signal, args: ['drag'] },
    })

    __drags = new Set()

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        JQApplication.MouseController.addDropArea(obj)
        return obj
    }

    __acceptsDragTarget(dragTarget) {
        let keys = this.keys
        if(!keys || (Array.isArray(keys) && keys.length === 0)) return true

        let dragKeys = []
        if(dragTarget && dragTarget.Drag && dragTarget.Drag.keys !== undefined && dragTarget.Drag.keys !== null){
            if(Array.isArray(dragTarget.Drag.keys)){
                dragKeys = dragTarget.Drag.keys
            } else if(typeof dragTarget.Drag.keys === 'string'){
                dragKeys = dragTarget.Drag.keys.split(',').map(v => v.trim()).filter(Boolean)
            } else {
                dragKeys = [dragTarget.Drag.keys]
            }
        }

        if(!Array.isArray(keys)){
            keys = [keys]
        }

        for(let key of keys){
            if(dragKeys.includes(key)) return true
        }

        return false
    }

    __enterOrMove(dragTarget){
        if(!dragTarget) return

        if(!this.__drags.has(dragTarget)){
            this.__drags.add(dragTarget)
            this.containsDrag = true
            this.drag = dragTarget
            this.entered(dragTarget)
        } else {
            this.drag = dragTarget
            this.positionChanged(dragTarget)
        }
    }

    __drop(dragTarget){
        if(!dragTarget) return

        this.__drags.delete(dragTarget)
        if(this.__drags.size === 0){
            this.containsDrag = false
            this.drag = undefined
        }

        this.dropped(dragTarget)
    }

    __exit(dragTarget){
        if(!dragTarget) return

        if(this.__drags.has(dragTarget)){
            this.__drags.delete(dragTarget)
            if(this.__drags.size === 0){
                this.containsDrag = false
                this.drag = undefined
            }
            this.exited()
        }
    }

    __destroy(){
        JQApplication.MouseController.removeDropArea(this)
        super.__destroy()
    }
}

module.exports = DropArea
