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

    $rowChanged(roles){
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

module.exports = { sameModel, ModelContext, modelRows }
