// StackView of QtQuick.Templates (qquickstackview.cpp, Qt 6.12). Checked against Qt 6.12 (tests/diff/cases/stackview.qml):
//  - push / pop / replace / clear with an item, a component (made with its properties, deleted when it leaves) or an
//    array of them; initialItem; an item of its own goes back to its parent when it leaves the stack, hidden;
//  - the items fill the view; the attached StackView.index, view, status, visible and their signals:
//    push Immediate: the new item activating, activated, the old one deactivating, deactivated, then currentItem;
//    with a transition: the old one deactivating, currentItem, and after the transition deactivated (busy meanwhile);
//    pop: the item below activating, currentItem, activated after the transition; a removed item gets removed();
//  - the transitions of the style (pushEnter, pushExit, popEnter...) move the items.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')

const Inactive = 0, Deactivating = 1, Activating = 2, Active = 3
const Transition = -1, Immediate = 0, PushTransition = 1, ReplaceTransition = 2, PopTransition = 3
const DontLoad = 0, ForceLoad = 1

// StackView.index / view / status / visible of an item in a stack
class StackViewAttached extends GroupObject {}
defineType(StackViewAttached, {
    props: { index: { kind: 'int', readonly: true, initial: -1 }, view: { kind: 'var', readonly: true, initial: null },
        status: { kind: 'int', readonly: true }, visible: 'bool' },
    signals: ['activated', 'activating', 'deactivated', 'deactivating', 'removed'],
})
const attachedOf = (item) => item.StackView
function setStatus(item, status){
    let a = attachedOf(item)
    if(a.$v[StackViewAttached.STATUS] === status) return
    GroupObject.prototype.$write.call(a, StackViewAttached.STATUS, status)
    let signal = status === Activating ? 'activating' : status === Active ? 'activated' : status === Deactivating ? 'deactivating' : 'deactivated'
    engine.emit(a, StackViewAttached['SIGNAL_' + signal])
}

class StackView extends Control {
    constructor(parent = null){
        super(parent)
        this.$entries = []          // { item, owned, parent }
        this.$running = 0
    }
    get depth(){
        engine.track(this, StackView.SIGNAL_depthChanged)
        return this.$entries.length
    }
    get empty(){
        engine.track(this, StackView.SIGNAL_depthChanged)
        return this.$entries.length === 0
    }
    $top(){ return this.$entries.length ? this.$entries[this.$entries.length - 1] : null }

    // an item to put on the stack: a component made in the view, a url, or an item of its own
    $make(target, properties){
        if(!target) return null
        if(target.$isItem){
            let entry = { item: target, owned: false, parent: target.$parent }
            if(target.$parent !== this) target.$setParent(this)
            if(properties) for(let key of Object.keys(properties)) target[key] = properties[key]
            return entry
        }
        let component = target
        if(typeof target === 'string'){
            let { Qt } = require('../../Qt/Qt')
            component = Qt.createComponent(target)
        }
        if(!component || !component.createObject) return null
        let item = component.createObject(this, properties || null)
        return item ? { item, owned: true, parent: null } : null
    }
    // the items fill the view (QQuickStackElement::initialize)
    $fit(item){
        if(!(item.$explicit & 1) && !item.$stackWidth){ item.$stackWidth = true; item.$bind(Item.WIDTH, () => this.width, null, item) }
        if(!(item.$explicit & 2) && !item.$stackHeight){ item.$stackHeight = true; item.$bind(Item.HEIGHT, () => this.height, null, item) }
    }
    $attach(entry, index){
        let a = attachedOf(entry.item)
        GroupObject.prototype.$write.call(a, StackViewAttached.INDEX, index)
        GroupObject.prototype.$write.call(a, StackViewAttached.VIEW, this)
        this.$fit(entry.item)
    }
    // an item that leaves: removed; a made one deleted, one of its own back to its parent, hidden
    $release(entry){
        let item = entry.item
        if(item.$f & engine.DESTROYED) return
        let a = attachedOf(item)
        GroupObject.prototype.$write.call(a, StackViewAttached.INDEX, -1)
        GroupObject.prototype.$write.call(a, StackViewAttached.VIEW, null)
        GroupObject.prototype.$write.call(a, StackViewAttached.STATUS, Inactive)
        engine.emit(a, StackViewAttached.SIGNAL_removed)
        if(entry.owned) item.$releaseLater ? item.$releaseLater() : item.destroy()
        else {
            item.$write(Item.VISIBLE, false)
            item.$setParent(entry.parent || null)
        }
    }
    $changed(oldTop){
        engine.emit(this, StackView.SIGNAL_depthChanged)
        let top = this.$top()
        this.$plain(StackView.CURRENTITEM, top ? top.item : null)
    }

    // ---- the operations ----
    // the arguments of push / replace: [item or component, properties]... and an operation at the end
    $parse(args){
        let operation = Transition
        if(args.length && typeof args[args.length - 1] === 'number') operation = args.pop()
        let list = []
        let flat = args.length === 1 && Array.isArray(args[0]) ? args[0] : args
        for(let i = 0; i < flat.length; i++){
            let target = flat[i]
            let properties = flat[i + 1] && typeof flat[i + 1] === 'object' && !flat[i + 1].$isItem && !flat[i + 1].createObject && !Array.isArray(flat[i + 1]) ? flat[++i] : null
            list.push({ target, properties })
        }
        return { list, operation }
    }
    push(...args){
        let { list, operation } = this.$parse(args)
        let result = null
        engine.batch(() => {
            let old = this.$top()
            let made = []
            for(let { target, properties } of list){
                let entry = this.$make(target, properties)
                if(entry) made.push(entry)
            }
            if(!made.length) return
            for(let entry of made){
                this.$attach(entry, this.$entries.length)
                this.$entries.push(entry)
            }
            let top = made[made.length - 1]
            for(let entry of made) if(entry !== top) entry.item.$write(Item.VISIBLE, false)
            this.$switch(old, top, operation === Transition ? PushTransition : operation, 'push')
            result = top.item
        })
        return result
    }
    pop(...args){
        let operation = Transition
        if(args.length && typeof args[args.length - 1] === 'number') operation = args.pop()
        if(args.length && args[args.length - 1] === undefined) args.pop()
        let to = args[0] === undefined ? undefined : args[0]
        let result = null
        engine.batch(() => {
            if(this.$entries.length <= 1 && to === undefined) return
            let old = this.$top()
            let keep = to === undefined ? this.$entries.length - 2 : to === null ? 0 : this.$entries.findIndex(e => e.item === to)
            if(keep < 0 || keep >= this.$entries.length - 1) return
            let removed = this.$entries.splice(keep + 1)
            result = old.item
            let top = this.$top()
            this.$switch(old, top, operation === Transition ? PopTransition : operation, 'pop', removed)
        })
        return result
    }
    replace(...args){
        let target = undefined
        if(args.length && args[0] && args[0].$isItem && this.$entries.some(e => e.item === args[0]) && args.length > 1) target = args.shift()
        let { list, operation } = this.$parse(args)
        let result = null
        engine.batch(() => {
            let old = this.$top()
            let from = target === undefined ? Math.max(0, this.$entries.length - 1) : this.$entries.findIndex(e => e.item === target)
            let removed = this.$entries.splice(from)
            let made = []
            for(let { target: t, properties } of list){
                let entry = this.$make(t, properties)
                if(entry) made.push(entry)
            }
            for(let entry of made){
                this.$attach(entry, this.$entries.length)
                this.$entries.push(entry)
            }
            let top = this.$top()
            for(let entry of made) if(entry !== top) entry.item.$write(Item.VISIBLE, false)
            this.$switch(old, top, operation === Transition ? ReplaceTransition : operation, 'replace', removed)
            result = top ? top.item : null
        })
        return result
    }
    clear(operation = Immediate){
        engine.batch(() => {
            let removed = this.$entries.splice(0)
            // QQuickStackView::clear: the current item inactive without its signals, currentItem, then removed
            let old = removed[removed.length - 1] || null
            if(old) GroupObject.prototype.$write.call(attachedOf(old.item), StackViewAttached.STATUS, Inactive)
            this.$changed()
            for(let entry of removed.reverse()) this.$release(entry)
        })
    }
    // the functions of Qt 6.7
    pushItem(item, properties, operation){ return properties !== undefined && typeof properties !== 'number' ? this.push(item, properties, operation === undefined ? Transition : operation) : this.push(item, properties === undefined ? Transition : properties) }
    pushItems(items, operation = Transition){ return this.push(items, operation) }
    popToItem(item, operation = Transition){ return this.pop(item, operation) }
    popToIndex(index, operation = Transition){ let entry = this.$entries[index]; return entry ? this.pop(entry.item, operation) : null }
    popCurrentItem(operation = Transition){ return this.pop(undefined, operation) }
    replaceCurrentItem(items, properties, operation){
        if(Array.isArray(items)) return this.replace(items, properties === undefined ? Transition : properties)
        return properties !== undefined && typeof properties !== 'number' ? this.replace(items, properties, operation === undefined ? Transition : operation) : this.replace(items, properties === undefined ? Transition : properties)
    }
    get(index, behavior = DontLoad){
        let entry = this.$entries[index]
        return entry ? entry.item : null
    }
    find(callback, onlySearchLoadedItems = false){
        for(let i = this.$entries.length - 1; i >= 0; i--){
            let item = this.$entries[i].item
            if(callback(item, i)) return item
        }
        return null
    }
    // the item shown changes: the statuses, currentItem, and the transitions of the style when not Immediate
    $switch(old, top, operation, kind, removed = []){
        let enter = top ? top.item : null, exit = old && old !== top ? old.item : null
        let enterTransition = null, exitTransition = null
        if(operation !== Immediate){
            let names = { push: ['PUSHENTER', 'PUSHEXIT'], pop: ['POPENTER', 'POPEXIT'], replace: ['REPLACEENTER', 'REPLACEEXIT'] }[kind]
            enterTransition = this.$v[StackView[names[0]]]
            exitTransition = this.$v[StackView[names[1]]]
        }
        if(enter){
            enter.$write(Item.VISIBLE, true)
            enter.$write(Item.Z, kind === 'pop' ? 0 : 1)
        }
        if(exit) exit.$write(Item.Z, kind === 'pop' ? 1 : 0)
        let animated = !!(enterTransition || exitTransition) && !!exit
        if(!animated){
            // Immediate push: the new item activates, then the old one deactivates, then currentItem; pop and replace:
            // the old one deactivates and is removed, then the new one activates, then currentItem
            let activate = () => {
                if(!enter) return
                setStatus(enter, Activating)
                setStatus(enter, Active)
            }
            let deactivate = () => {
                if(!exit) return
                setStatus(exit, Deactivating)
                setStatus(exit, Inactive)
                exit.$write(Item.VISIBLE, false)
            }
            if(kind === 'push'){
                activate()
                deactivate()
                this.$changed()
            } else {
                deactivate()
                for(let entry of removed) this.$release(entry)
                removed = []
                activate()
                this.$changed()
            }
            for(let entry of removed) this.$release(entry)
            return
        }
        // with transitions: the item that leaves deactivating (push) / the item below activating (pop), currentItem, then
        // the rest once the transitions end
        if(kind === 'pop'){
            if(enter) setStatus(enter, Activating)
            if(exit) setStatus(exit, Deactivating)
        } else {
            if(exit) setStatus(exit, Deactivating)
            if(enter) setStatus(enter, Activating)
        }
        this.$changed()
        this.$setBusy(true)
        let pending = 0
        let finish = () => {
            if(--pending > 0) return
            engine.batch(() => {
                if(exit){
                    setStatus(exit, Inactive)
                    exit.$write(Item.VISIBLE, false)
                }
                if(enter) setStatus(enter, Active)
                for(let entry of removed) this.$release(entry)
                this.$setBusy(false)
            })
        }
        let run = (transition, target) => {
            if(!transition || !target) return
            pending++
            let { TransitionManager } = require('../TransitionManager')
            let manager = new TransitionManager(null)
            let done = false
            manager.complete = function(){
                TransitionManager.prototype.complete.call(this)
                if(!done){ done = true; finish() }
            }
            manager.transition([], transition, target)
        }
        pending++
        run(enterTransition, enter)
        run(exitTransition, exit)
        finish()
    }
    $setBusy(value){
        this.$running += value ? 1 : -1
        let busy = this.$running > 0
        if(this.$v[StackView.BUSY] !== busy) this.$plain(StackView.BUSY, busy)
    }
    $componentComplete(){
        super.$componentComplete()
        let initial = this.$v[StackView.INITIALITEM]
        if(initial && !this.$entries.length) this.push(initial, Immediate)
    }
    // QQuickStackView: the items above are hidden; a press on it is not its own
    $mousePress(){ return false }
}
defineType(StackView, {
    props: {
        busy: { kind: 'bool', readonly: true }, currentItem: { kind: 'var', readonly: true, initial: null }, initialItem: 'var',
        popEnter: { kind: 'var', initial: null }, popExit: { kind: 'var', initial: null }, pushEnter: { kind: 'var', initial: null },
        pushExit: { kind: 'var', initial: null }, replaceEnter: { kind: 'var', initial: null }, replaceExit: { kind: 'var', initial: null },
    },
    signals: ['depthChanged', 'emptyChanged'],
})
Object.assign(StackView, { Inactive, Deactivating, Activating, Active, Transition, Immediate, PushTransition, ReplaceTransition, PopTransition, DontLoad, ForceLoad })
StackView.prototype.$controlName = 'StackView'
// StackView.Immediate in an item: the attached object has the enums too
for(let [name, value] of Object.entries({ Inactive, Deactivating, Activating, Active, Transition, Immediate, PushTransition, ReplaceTransition, PopTransition, DontLoad, ForceLoad }))
    Object.defineProperty(StackViewAttached.prototype, name, { value, configurable: true })

{
    let prop = Item.$type.propByName.get('StackView')
    if(prop){
        prop.group = StackViewAttached
        Object.defineProperty(Item.prototype, 'StackView', {
            get(){ return this.$v[prop.id] || (this.$v[prop.id] = new StackViewAttached(this)) },
            configurable: true,
        })
    }
}

module.exports = { StackView, StackViewAttached }
