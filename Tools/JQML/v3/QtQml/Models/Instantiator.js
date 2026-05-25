const QtObject = require("../QtObject")
const Component = require("../Component")
const Bool = require("../Bool")
const Int = require("../Int")
const Var = require("../Var")
const Variant = require("../Variant")
const Signal = require("../Signal")
const JQApplication = require("../../core/JQApplication")

class Instantiator extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        active:       { type: Bool,    value: true      },
        asynchronous: { type: Bool,    value: false     },
        count:        { type: Int,     value: 0         },
        delegate:     { type: Variant, typeTarget: Component, value: undefined },
        model:        { type: Var,     value: 1         },
        object:       { type: Variant, typeTarget: QtObject, value: undefined  },

        activeChanged:       { type: Signal, args: [] },
        asynchronousChanged: { type: Signal, args: [] },
        countChanged:        { type: Signal, args: [] },
        delegateChanged:     { type: Signal, args: [] },
        modelChanged:        { type: Signal, args: [] },
        objectChanged:       { type: Signal, args: [] },

        objectAdded:   { type: Signal, args: ['index', 'object'] },
        objectRemoved: { type: Signal, args: ['index', 'object'] },
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        obj.__objects = []
        obj.__completed = false
        return obj
    }

    __complete() {
        super.__complete()
        this.__completed = true
        this.__rebuild()
    }

    // ── Slot handlers ────────────────────────────────────────────────────────

    SLOT_activeChanged()   { if (this.__completed) this.__rebuild() }
    SLOT_modelChanged()    { if (this.__completed) this.__rebuild() }
    SLOT_delegateChanged() { if (this.__completed) this.__rebuild() }

    // ── Internal ─────────────────────────────────────────────────────────────

    __destroyAll() {
        for (let i = this.__objects.length - 1; i >= 0; i--) {
            let obj = this.__objects[i]
            this.objectRemoved(i, obj)
            if (obj && !obj.__destroyed && typeof obj.destroy === 'function') obj.destroy()
        }
        this.__objects = []
        this.count  = 0
        this.object = undefined
    }

    __rebuild() {
        this.__destroyAll()

        if (!this.active)   return
        if (!this.delegate) return

        let model = this.model
        let count = 0

        if (model === undefined || model === null) return
        else if (typeof model === 'number')  count = Math.max(0, Math.floor(model))
        else if (Array.isArray(model))       count = model.length
        else if (typeof model === 'object') {
            // ListModel / TableModel / BaseModel
            if (model.count !== undefined)       count = model.count
            else if (model.__data !== undefined) count = model.__data.length
            else return
        } else return

        JQApplication.beginUpdate()

        for (let i = 0; i < count; i++) {
            let properties = {}

            if (typeof model === 'number') {
                properties.model = { index: i }
            } else if (Array.isArray(model)) {
                properties.modelData = model[i]
                properties.model     = { index: i }
            } else {
                let data = model.__data ? model.__data[i] : undefined
                properties.model     = data || { index: i }
                properties.modelData = data
            }

            let obj = this.delegate.createObject(this.__proxy, properties, true)
            this.__objects.push(obj)
            this.objectAdded(i, obj)
        }

        JQApplication.endUpdate()

        this.count  = this.__objects.length
        this.object = this.__objects.length > 0 ? this.__objects[0] : undefined
    }

    // ── Public API ────────────────────────────────────────────────────────────

    objectAt(index) {
        return (index >= 0 && index < this.__objects.length) ? this.__objects[index] : null
    }

    __destroy() {
        this.__destroyAll()
        super.__destroy()
    }
}

module.exports = Instantiator