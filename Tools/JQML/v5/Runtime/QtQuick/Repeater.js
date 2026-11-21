const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel, moveEntries } = require('../QtQml/Models/ModelContext')
const { Item } = require('./Item')

// ---- Repeater: delegates go to the parent right after the Repeater --------------------------

class Repeater extends Item {
    constructor(parent = null){
        super(parent)
        this.$items = []
        this.$contexts = []
        this.$model = null
        if(this.$dom) this.$dom.style.display = 'none'
        // delegates are created when the component is complete, like QQuickRepeater::componentComplete
        engine.scheduleComponentComplete(this)
    }

    itemAt(index){ return this.$items[index] || null }

    $sync(){
        let model = this.$v[Repeater.MODEL]
        if(model !== this.$model){
            if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
            this.$model = model
            if(model && model.$addView) model.$addView(this)
        }
        this.$rebuild()
    }

    $clear(){
        for(let i = this.$items.length - 1; i >= 0; i--){
            let item = this.$items[i]
            engine.emitArgs(this, Repeater.SIGNAL_itemRemoved, [i, item])
            // an item of an ObjectModel stays the model's (QQmlObjectModel::release)
            if(!this.$objectItems) item.$releaseLater()
        }
        this.$items = []
        this.$contexts = []
    }

    $rebuild(){
        this.$clear()
        let delegate = this.$v[Repeater.DELEGATE]
        let parent = this.$parent
        this.$objectItems = !!(this.$model && this.$model.$isObjectModel)
        if((!delegate && !this.$objectItems) || !parent || this.$f & engine.DESTROYED){
            this.$write(Repeater.COUNT, 0)
            return
        }
        let rows = modelRows(this.$model)
        for(let i = 0; i < rows.count; i++) this.$create(i, rows)
        this.$write(Repeater.COUNT, this.$items.length)
        for(let i = 0; i < this.$items.length; i++) engine.emitArgs(this, Repeater.SIGNAL_itemAdded, [i, this.$items[i]])
    }

    $create(index, rows){
        let delegate = this.$v[Repeater.DELEGATE]
        let ctx = new ModelContext(rows.row(index), index, rows.data(index))
        ctx.$view = this
        // an ObjectModel: its item itself, put in the parent of the Repeater
        let item = rows.object ? rows.object(index) : delegate.createObject(this.$parent, null, ctx)
        if(item && rows.object && item.$parent !== this.$parent) item.$setParent(this.$parent)
        if(!item) return null
        this.$placeAfter(item, index)
        this.$items.splice(index, 0, item)
        this.$contexts.splice(index, 0, ctx)
        return item
    }

    // Children order (and DOM order) of the parent: Repeater, delegate 0, delegate 1, ...
    $placeAfter(item, index){
        let siblings = this.$parent.$children
        let anchor = index === 0 ? this : this.$items[index - 1]
        // a delegate made at the end, right after the one before it: already in its place (children and DOM)
        let last = siblings.length - 1
        if(siblings[last] === item && siblings[last - 1] === anchor) return
        let from = siblings.indexOf(item)
        if(from >= 0) siblings.splice(from, 1)
        let to = siblings.indexOf(anchor) + 1
        siblings.splice(to, 0, item)
        if(item.$dom && this.$parent.$dom){
            let next = siblings[to + 1]
            let nextDom = null
            for(let i = to + 1; i < siblings.length && !nextDom; i++) nextDom = siblings[i].$dom || null
            this.$parent.$dom.insertBefore(item.$dom, nextDom)
        }
    }

    $modelChanged(kind, index, count, roles = null, to = -1){
        if(!this.$v[Repeater.DELEGATE] && !this.$objectItems) return
        let rows = modelRows(this.$model)
        // QQuickRepeater::modelUpdated: the items of moved rows are the same, in their new place among the parent's
        // children; no itemRemoved, no itemAdded
        if(kind === 'move'){
            moveEntries([this.$items, this.$contexts], index, to, count)
            this.$reindex()
            if(this.$parent) for(let i = 0; i < this.$items.length; i++) this.$placeAfter(this.$items[i], i)
        } else if(kind === 'insert'){
            for(let i = 0; i < count; i++) this.$create(index + i, rows)
            this.$reindex()
            this.$write(Repeater.COUNT, this.$items.length)
            for(let i = 0; i < count; i++) engine.emitArgs(this, Repeater.SIGNAL_itemAdded, [index + i, this.$items[index + i]])
        } else if(kind === 'remove'){
            let removed = this.$items.splice(index, count)
            this.$contexts.splice(index, count)
            this.$reindex()
            this.$write(Repeater.COUNT, this.$items.length)
            for(let i = 0; i < removed.length; i++){
                engine.emitArgs(this, Repeater.SIGNAL_itemRemoved, [index + i, removed[i]])
                if(!this.$objectItems) removed[i].$releaseLater()
            }
        } else if(kind === 'change'){
            for(let i = index; i < index + count; i++){
                let ctx = this.$contexts[i]
                if(!ctx) continue
                ctx.$row = rows.row(i)
                ctx.$write(ModelContext.MODELDATA, rows.data(i))
                ctx.$rowChanged(roles)
            }
        } else {
            this.$rebuild()
        }
    }

    $reindex(){
        for(let i = 0; i < this.$contexts.length; i++) this.$contexts[i].$write(ModelContext.INDEX, i)
    }

    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        // also called from the Item constructor, before the fields of this class exist
        if(this.$f & engine.COMPLETED || (this.$items && this.$items.length)) engine.queueCall(() => this.$sync())
    }

    $componentComplete(){
        super.$componentComplete()
        this.$sync()
    }

    $disposeNative(){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        super.$disposeNative()
    }
}

const resync = (repeater) => { if(repeater.$f & engine.COMPLETED) repeater.$sync() }
const modelSet = (repeater, value, old) => { if(!sameModel(value, old)) resync(repeater) }

defineType(Repeater, {
    props: {
        model: { kind: 'var', effect: modelSet },
        delegate: { kind: 'component', effect: resync },
        count: { kind: 'int', readonly: true },
    },
    signals: ['itemAdded(index,item)', 'itemRemoved(index,item)'],
})
Repeater.$defaultProperty = 'delegate'

module.exports = { Repeater }
