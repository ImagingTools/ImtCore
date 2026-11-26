// ObjectModel of QtQml.Models (QQmlObjectModel): a model of items that the views (ListView, Repeater...) show themselves,
// with no delegate. The items declared in it are its items, in their order; append, insert, move, remove and clear change
// it, and the views follow as for a ListModel (insert, remove, move). A view puts an item in its own content; the
// item stays the model's (a view that lets it go hides it). The models of Container (contentModel) are ObjectModels.
const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')

class ObjectModel extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$items = []
        this.$views = null
        engine.scheduleComponentComplete(this)
    }
    // the items declared in it (its children at its completion)
    $componentComplete(){
        super.$componentComplete()
        let declared = (this.$children || []).filter(child => child.$isItem && !this.$items.includes(child))
        if(declared.length) this.$insertItems(this.$items.length, declared)
    }
    $notify(kind, index, count, to = -1){
        this.$write(ObjectModel.COUNT, this.$items.length)
        engine.emit(this, ObjectModel.SIGNAL_childrenChanged)
        if(this.$views) for(let view of this.$views.slice()) view.$modelChanged(kind, index, count, null, to)
    }
    $insertItems(index, items){
        this.$items.splice(index, 0, ...items)
        for(let item of items) item.$objectModel = this
        this.$notify('insert', index, items.length)
    }
    get children(){ return this.$items.slice() }
    get(index){ return this.$items[index] || null }
    append(item){ if(item) engine.batch(() => this.$insertItems(this.$items.length, [item])) }
    insert(index, item){ if(item) engine.batch(() => this.$insertItems(Math.max(0, Math.min(index, this.$items.length)), [item])) }
    move(from, to, count = 1){
        if(from === to || count <= 0) return
        engine.batch(() => {
            let moved = this.$items.splice(from, count)
            this.$items.splice(to, 0, ...moved)
            this.$notify('move', from, moved.length, to)
        })
    }
    remove(index, count = 1){
        engine.batch(() => {
            let removed = this.$items.splice(index, count)
            for(let item of removed) if(item.$objectModel === this) item.$objectModel = null
            if(removed.length) this.$notify('remove', index, removed.length)
        })
    }
    clear(){ if(this.$items.length) this.remove(0, this.$items.length) }
    $addView(view){ (this.$views || (this.$views = [])).push(view) }
    $removeView(view){
        if(!this.$views) return
        let index = this.$views.indexOf(view)
        if(index >= 0) this.$views.splice(index, 1)
    }
}
ObjectModel.prototype.$isObjectModel = true
defineType(ObjectModel, { props: { count: { kind: 'int', readonly: true } }, signals: ['childrenChanged'] })

module.exports = { ObjectModel }
