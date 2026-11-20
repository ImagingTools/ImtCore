const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { ListModel, RoleCell } = require('./ListModel')

// ---- delegate context: index, modelData and the roles of one row ------------------------

class ModelContext extends QtObject {
    constructor(row, index, modelData){
        super(null)
        this.$row = row
        this.$view = null
        this.$cells = null
        this.$v[ModelContext.INDEX] = index
        this.$v[ModelContext.MODELDATA] = modelData
    }

    // unqualified role name in a delegate: `text: name`. Every role notifies on its own, like the role
    // properties of QQmlDMAbstractItemModelData do on dataChanged(roles)
    $role(name){
        if(!this.$cells) this.$cells = new Map()
        let cell = this.$cells.get(name)
        if(!cell) this.$cells.set(name, cell = new RoleCell())
        engine.track(cell, RoleCell.SIGNAL_changed)
        return this.$row ? this.$row[name] : undefined
    }

    // QQmlDelegateModel with required properties: the required properties of the delegate get the row (the roles by
    // their name, index, modelData, model) when it is made, and again when the row, its data or its index change
    $attachRequired(item){
        let names = item && item.constructor && item.constructor.$qmlRequired
        if(!names || !names.length) return
        this.$requiredItem = item
        this.$requiredNames = names
        this.$applyRequired(null)
    }

    $applyRequired(roles){
        let item = this.$requiredItem
        if(!item || item.$f & engine.DESTROYED) return
        for(let name of this.$requiredNames){
            let value
            if(name === 'index') value = this.$v[ModelContext.INDEX]
            else if(name === 'modelData') value = this.$v[ModelContext.MODELDATA]
            else if(name === 'model') value = this.model
            else if(this.$row && typeof this.$row === 'object' && name in this.$row){
                if(roles && !roles.includes(name)) continue
                value = this.$row[name]
            } else continue
            if(item[name] !== value) item[name] = value
        }
    }

    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if((id === ModelContext.INDEX || id === ModelContext.MODELDATA) && old !== this.$v[id]) this.$applyRequired(null)
    }

    $rowChanged(roles){
        this.$applyRequired(roles)
        engine.emitArgs(this, ModelContext.SIGNAL_rowChanged, [])
        if(!this.$cells) return
        if(!roles){
            for(let cell of Array.from(this.$cells.values())) engine.emitArgs(cell, RoleCell.SIGNAL_changed, [])
            return
        }
        for(let role of roles){
            let cell = this.$cells.get(role)
            if(cell) engine.emitArgs(cell, RoleCell.SIGNAL_changed, [])
        }
    }

    // `model.name`, `model.index`
    get model(){
        if(!this.$model){
            let ctx = this
            this.$model = new Proxy({}, {
                get(target, key){
                    if(key === 'index') return ctx.index
                    if(key === 'modelData') return ctx.modelData
                    return ctx.$role(key)
                },
                set(target, key, value){
                    if(ctx.$view && ctx.$view.$model instanceof ListModel) ctx.$view.$model.setProperty(ctx.$v[ModelContext.INDEX], key, value)
                    else if(ctx.$row) ctx.$row[key] = value
                    return true
                },
            })
        }
        return this.$model
    }
}

defineType(ModelContext, {
    props: { index: 'int', modelData: 'var' },
    signals: ['rowChanged'],
})

function modelRows(model){
    if(model instanceof ListModel) return { count: model.$rows.length, row: (i) => model.$rows[i], data: (i) => model.$rows[i] }
    if(typeof model === 'number') return { count: Math.max(0, Math.floor(model)), row: () => null, data: (i) => i }
    if(Array.isArray(model)) return { count: model.length, row: (i) => (model[i] && typeof model[i] === 'object' ? model[i] : null), data: (i) => model[i] }
    if(model && typeof model.length === 'number') return { count: model.length, row: (i) => model[i], data: (i) => model[i] }
    return { count: 0, row: () => null, data: () => undefined }
}

// QQuickItemView::setModel, QQuickRepeater::setModel: a new value equal to the model is no change (QVariant compares
// JS values by content), the delegates stay
function sameModel(a, b){
    if(a === b) return true
    if(!a || !b || typeof a !== 'object' || typeof b !== 'object') return false
    if(Array.isArray(a)){
        if(!Array.isArray(b) || a.length !== b.length) return false
        return a.every((item, i) => sameModel(item, b[i]))
    }
    let proto = Object.getPrototypeOf(a)
    if((proto !== Object.prototype && proto !== null) || proto !== Object.getPrototypeOf(b)) return false
    let keys = Object.keys(a)
    return keys.length === Object.keys(b).length && keys.every(key => key in b && sameModel(a[key], b[key]))
}

// QQmlChangeSet move (rowsMoved): count entries of each array from from go to to (where they start afterwards)
function moveEntries(arrays, from, to, count){
    for(let array of arrays){
        let moved = array.splice(from, count)
        let tail = array.splice(to)
        for(let i = 0; i < moved.length; i++) array.push(moved[i])
        for(let i = 0; i < tail.length; i++) array.push(tail[i])
    }
}

// the index a row has after a move
function movedIndex(i, from, to, count){
    if(i >= from && i < from + count) return to + (i - from)
    let j = i < from ? i : i - count
    return j >= to ? j + count : j
}

module.exports = { sameModel, ModelContext, modelRows, moveEntries, movedIndex }
