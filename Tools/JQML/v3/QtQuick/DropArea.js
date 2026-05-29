const Item = require("./Item")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

/**
 * DropArea — accepts drag-and-drop events on a rectangular region.
 * Wraps HTML5 dragover / drop / dragenter / dragleave events.
 */
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

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__installDropHandlers()
        return obj
    }

    __installDropHandlers() {
        let dom = this.__DOM
        dom.addEventListener('dragenter', (e) => {
            e.preventDefault()
            let dragInfo = this.__buildDragInfo(e)
            if (!this.__acceptsDrag(dragInfo)) return
            this.containsDrag = true
            this.drag = dragInfo
            this.entered(dragInfo)
        })

        dom.addEventListener('dragover', (e) => {
            e.preventDefault()
            let dragInfo = this.__buildDragInfo(e)
            if (!this.__acceptsDrag(dragInfo)) return
            this.drag = dragInfo
            this.positionChanged(dragInfo)
        })

        dom.addEventListener('dragleave', (e) => {
            // Only fire exited if truly leaving this element (not entering a child)
            if (!dom.contains(e.relatedTarget)) {
                this.containsDrag = false
                this.drag = undefined
                this.exited()
            }
        })

        dom.addEventListener('drop', (e) => {
            e.preventDefault()
            let dropInfo = this.__buildDropInfo(e)
            this.containsDrag = false
            this.drag = undefined
            this.dropped(dropInfo)
        })
    }

    __buildDragInfo(e) {
        let rect = this.__DOM.getBoundingClientRect()
        return {
            x: e.clientX - rect.left,
            y: e.clientY - rect.top,
            keys: this.__extractKeys(e),
            accepted: true,
            source: e.relatedTarget || null,
        }
    }

    __buildDropInfo(e) {
        let rect = this.__DOM.getBoundingClientRect()
        let data = {}
        if (e.dataTransfer && e.dataTransfer.types) {
            for (let t of e.dataTransfer.types) {
                data[t] = e.dataTransfer.getData(t)
            }
        }
        return {
            x: e.clientX - rect.left,
            y: e.clientY - rect.top,
            keys: this.__extractKeys(e),
            text: e.dataTransfer ? e.dataTransfer.getData('text/plain') : '',
            urls: e.dataTransfer ? e.dataTransfer.getData('text/uri-list').split('\n').filter(Boolean) : [],
            data: data,
            accepted: true,
        }
    }

    __extractKeys(e) {
        if (!e.dataTransfer || !e.dataTransfer.types) return []
        return Array.from(e.dataTransfer.types)
    }

    __acceptsDrag(dragInfo) {
        let keys = this.keys
        if (!keys || !keys.length) return true
        let dragKeys = dragInfo.keys || []
        for (let k of keys) {
            if (dragKeys.includes(k)) return true
        }
        return false
    }
}

module.exports = DropArea
