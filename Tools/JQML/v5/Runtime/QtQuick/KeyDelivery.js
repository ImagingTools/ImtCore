// Focus and keys of the window (QQuickDeliveryAgent: setFocusInScope, clearFocusInScope, deliverKeyEvent;
// QQuickItem: setFocus, forceActiveFocus, the focus part of setParentItem and setEffectiveEnableRecur).
// The mounted root item is the window's contentItem, a focus scope that has focus while the page has.
const engine = require('../Core/engine')
const { Item } = require('./Item')
const { Keys } = require('./Keys')
const { KeyNavigation } = require('./KeyNavigation')
const { KeyEvent } = require('./KeyEvent')

const agent = { root: null, activeFocusItem: null, windowFocused: false }

// setFocusInScope / clearFocusInScope options
const DONT_CHANGE_FOCUS_PROPERTY = 1
const DONT_CHANGE_SUB_FOCUS_ITEM = 2

// key delivery results: a QML handler accepted the key, or a text field takes it as typing
const ACCEPTED = 1
const CONSUMED = 2

let syncingDom = false

function isEnabled(item){
    for(let it = item; it instanceof Item; it = it.$parent){
        if(!it.$v[Item.ENABLED]) return false
    }
    return true
}

// (a test page stands for JQMLHost, which builds an Item root without a window and shows a Window root)
let windowShown = false
const noWindow = () => !windowShown && typeof window !== 'undefined' && !!window.__jqmlNoWindow

function inWindow(item){
    if(noWindow()) return false
    for(let it = item; it; it = it.$parent){
        if(it === agent.root) return true
    }
    return false
}

// the focus scope of an item: the nearest focus scope above it, or the topmost item
function scopeOf(item){
    let scope = item.$parent instanceof Item ? item.$parent : null
    while(scope && !scope.$isFocusScope && scope.$parent instanceof Item) scope = scope.$parent
    return scope
}

// QQuickItemPrivate::updateSubFocusItem: the chain from the scope down to the item that has focus in it
function updateSubFocusItem(item, scope, focus){
    let old = scope.$subFocusItem
    if(old){
        for(let it = old.$parent; it && it !== scope; it = it.$parent) it.$subFocusItem = null
    }
    if(focus){
        scope.$subFocusItem = item
        for(let it = item.$parent; it && it !== scope; it = it.$parent) it.$subFocusItem = item
    } else {
        scope.$subFocusItem = null
    }
}

// notifyFocusChangesRecur: focusChanged, then activeFocusChanged, for each item in order whose value moved
// since it was last notified
function notify(changed){
    for(let item of changed){
        if(item.$f & engine.DESTROYED) continue
        let focus = item.$v[Item.FOCUS], active = item.$v[Item.ACTIVEFOCUS]
        if(item.$notifiedFocus !== focus){
            item.$notifiedFocus = focus
            engine.emit(item, Item.SIGNAL_focusChanged, !focus, focus)
        }
        if(item.$notifiedActiveFocus !== active){
            item.$notifiedActiveFocus = active
            engine.emit(item, Item.SIGNAL_activeFocusChanged, !active, active)
        }
    }
}

function dropActiveFocus(oldActive, scope, changed){
    agent.activeFocusItem = null
    for(let it = oldActive; it && it !== scope; it = it.$parent){
        if(it.$v[Item.ACTIVEFOCUS]){
            it.$v[Item.ACTIVEFOCUS] = false
            changed.push(it)
        }
    }
}

function setFocusInScope(scope, item, options = 0){
    let root = agent.root
    let changed = []
    let oldActive = agent.activeFocusItem, newActive = null
    if(item === root || (scope && scope.$v[Item.ACTIVEFOCUS])){
        if(isEnabled(item)){
            newActive = item
            while(newActive.$isFocusScope && newActive.$subFocusItem && isEnabled(newActive.$subFocusItem)) newActive = newActive.$subFocusItem
        } else {
            newActive = scope
        }
        if(oldActive) dropActiveFocus(oldActive, scope, changed)
    }
    if(item !== root && !(options & DONT_CHANGE_SUB_FOCUS_ITEM)){
        let oldSub = scope.$subFocusItem
        if(oldSub){
            oldSub.$v[Item.FOCUS] = false
            changed.push(oldSub)
        }
        updateSubFocusItem(item, scope, true)
    }
    if(!(options & DONT_CHANGE_FOCUS_PROPERTY) && (item !== root || agent.windowFocused)){
        item.$v[Item.FOCUS] = true
        changed.push(item)
    }
    if(newActive && root.$v[Item.FOCUS]){
        agent.activeFocusItem = newActive
        newActive.$v[Item.ACTIVEFOCUS] = true
        changed.push(newActive)
        for(let it = newActive.$parent; it && it !== scope; it = it.$parent){
            if(it.$isFocusScope){
                it.$v[Item.ACTIVEFOCUS] = true
                changed.push(it)
            }
        }
    }
    if(oldActive !== agent.activeFocusItem) syncDomFocus()
    notify(changed)
}

function clearFocusInScope(scope, item, options = 0){
    if(scope && !scope.$subFocusItem) return
    let root = agent.root
    let changed = []
    let oldActive = agent.activeFocusItem, newActive = null
    if(item === root || (scope && scope.$v[Item.ACTIVEFOCUS])){
        newActive = scope
        if(oldActive) dropActiveFocus(oldActive, scope, changed)
    }
    if(item !== root && !(options & DONT_CHANGE_SUB_FOCUS_ITEM)){
        let oldSub = scope.$subFocusItem
        if(!(options & DONT_CHANGE_FOCUS_PROPERTY)){
            oldSub.$v[Item.FOCUS] = false
            changed.push(oldSub)
        }
        updateSubFocusItem(oldSub, scope, false)
    } else if(item === root && !(options & DONT_CHANGE_FOCUS_PROPERTY)){
        item.$v[Item.FOCUS] = false
        changed.push(item)
    }
    if(newActive) agent.activeFocusItem = newActive
    if(oldActive !== agent.activeFocusItem) syncDomFocus()
    notify(changed)
}

// The DOM follows the active focus: a text field's input is focused, any other item blurs the input that has it
function syncDomFocus(){
    if(typeof document === 'undefined') return
    let input = agent.activeFocusItem && agent.activeFocusItem.$input
    let current = document.activeElement
    syncingDom = true
    try {
        if(input){
            if(current !== input) input.focus({ preventScroll: true })
        } else if(current && current.classList && current.classList.contains('jq2-input')){
            current.blur()
        }
    } finally {
        syncingDom = false
    }
}

// QQuickWindow::focusInEvent / focusOutEvent: the contentItem takes or loses focus with the window
function setWindowFocused(focused){
    if(!agent.root || agent.windowFocused === focused) return
    if(focused && noWindow()) return
    agent.windowFocused = focused
    engine.batch(() => agent.root.$setFocus(focused))
}

function windowCreated(){
    windowShown = true
}

// a shown Window is activated (QWindow::show -> requestActivate)
function activateWindow(){
    windowShown = true
    setWindowFocused(true)
}

// ---- keys -------------------------------------------------------------------------------------

// QQuickItemPrivate::deliverKeyEvent for one item: its key filters before the item, the item itself, the
// filters after it (Keys and KeyNavigation, each in its priority's phase)
function deliverTo(item, event, kind){
    if(filterKey(item, event, kind, false)) return ACCEPTED
    if(item.$keyEvent){
        let result = item.$keyEvent(event, kind)
        if(result) return result
    }
    if(filterKey(item, event, kind, true)) return ACCEPTED
    return 0
}

// QQuickItemKeyFilter chain: the attached object made last comes first
function filterKey(item, event, kind, post){
    let filters = item.$keyFilters
    if(filters){
        for(let filter of filters) if(filter.$filterKey(event, kind, post)) return true
    }
    return false
}

// from the item with active focus up the parents until one accepts; a Shortcut that matched has the key already
function deliverKey(e, kind){
    if(engine.isStopped() || e.defaultPrevented) return
    let item = agent.activeFocusItem
    if(!item) return
    let event = new KeyEvent(e)
    let result = 0
    engine.batch(() => {
        for(let it = item; it instanceof Item && !result; it = it.$parent) result = deliverTo(it, event, kind)
    })
    if(result === ACCEPTED) e.preventDefault()
}

let listening = false
function listen(){
    if(listening || typeof window === 'undefined') return
    listening = true
    window.addEventListener('keydown', (e) => deliverKey(e, 'pressed'))
    window.addEventListener('keyup', (e) => deliverKey(e, 'released'))
    window.addEventListener('focus', () => setWindowFocused(true))
    window.addEventListener('blur', () => setWindowFocused(false))
}

// ---- Item API -------------------------------------------------------------------------------------

Item.prototype.$isFocusScope = false

// QQuickItem::setFocus
Item.prototype.$setFocus = function(value){
    let focus = !!value
    let scope = scopeOf(this)
    if(this.$v[Item.FOCUS] === focus && (!focus || !scope || scope.$subFocusItem === this)) return
    if(inWindow(this)){
        if(focus) setFocusInScope(scope, this)
        else clearFocusInScope(scope, this)
        return
    }
    let changed = []
    if(scope){
        let oldSub = scope.$subFocusItem
        if(oldSub){
            updateSubFocusItem(oldSub, scope, false)
            oldSub.$v[Item.FOCUS] = false
            changed.push(oldSub)
        } else if(!scope.$isFocusScope && scope.$v[Item.FOCUS]){
            scope.$v[Item.FOCUS] = false
            changed.push(scope)
        }
        updateSubFocusItem(this, scope, focus)
    } else {
        let oldSub = this.$subFocusItem
        if(!this.$isFocusScope && oldSub){
            updateSubFocusItem(oldSub, this, false)
            oldSub.$v[Item.FOCUS] = false
            changed.push(oldSub)
        }
    }
    this.$v[Item.FOCUS] = focus
    changed.push(this)
    engine.emit(this, Item.SIGNAL_focusChanged, !focus, focus)
    notify(changed)
}

// QQuickItem::forceActiveFocus: the item and every focus scope above it take focus
Item.prototype.forceActiveFocus = function(){
    engine.batch(() => {
        this.$setFocus(true)
        for(let parent = this.$parent; parent instanceof Item; parent = parent.$parent){
            if(parent.$isFocusScope) parent.$setFocus(true)
        }
    })
}

// the focus part of QQuickItem::setParentItem: a focused item keeps focus in its new scope unless that scope
// already has a focused item
{
    let parentChanged = Item.prototype.$parentChanged
    Item.prototype.$parentChanged = function(old, parent){
        parentChanged.call(this, old, parent)
        let scopeFocusedItem = null
        if(old instanceof Item){
            if(this.$v[Item.FOCUS] || old.$subFocusItem === this) scopeFocusedItem = this
            else if(!this.$isFocusScope && this.$subFocusItem) scopeFocusedItem = this.$subFocusItem
            if(scopeFocusedItem){
                let scopeItem = old
                while(!scopeItem.$isFocusScope && scopeItem.$parent instanceof Item) scopeItem = scopeItem.$parent
                if(inWindow(old)){
                    clearFocusInScope(scopeItem, scopeFocusedItem, DONT_CHANGE_FOCUS_PROPERTY)
                    if(scopeFocusedItem !== this) updateSubFocusItem(scopeFocusedItem, this, true)
                } else {
                    updateSubFocusItem(scopeFocusedItem, scopeItem, false)
                }
            }
        }
        if(parent instanceof Item){
            if(!scopeFocusedItem){
                if(this.$v[Item.FOCUS]) scopeFocusedItem = this
                else if(!this.$isFocusScope && this.$subFocusItem) scopeFocusedItem = this.$subFocusItem
            }
            if(scopeFocusedItem){
                let scopeItem = parent
                while(!scopeItem.$isFocusScope && scopeItem.$parent instanceof Item) scopeItem = scopeItem.$parent
                if(scopeItem.$subFocusItem || (!scopeItem.$isFocusScope && scopeItem.$v[Item.FOCUS])){
                    if(scopeFocusedItem !== this) updateSubFocusItem(scopeFocusedItem, this, false)
                    scopeFocusedItem.$v[Item.FOCUS] = false
                    engine.emit(scopeFocusedItem, Item.SIGNAL_focusChanged, true, false)
                } else if(inWindow(parent)){
                    setFocusInScope(scopeItem, scopeFocusedItem, DONT_CHANGE_FOCUS_PROPERTY)
                } else {
                    updateSubFocusItem(scopeFocusedItem, scopeItem, true)
                }
            }
        }
    }
}

// the focus part of setEffectiveEnableRecur: a disabled item gives its active focus to its scope and gets it back
// when enabled again
function enableChanged(item, scope, enabled){
    let options = DONT_CHANGE_FOCUS_PROPERTY | DONT_CHANGE_SUB_FOCUS_ITEM
    if(scope && !enabled && item.$v[Item.ACTIVEFOCUS]) clearFocusInScope(scope, item, options)
    let childScope = item.$isFocusScope && scope ? item : scope
    for(let child of item.$children || []){
        if(child instanceof Item && child.$v[Item.ENABLED]) enableChanged(child, childScope, enabled)
    }
    if(scope && enabled && item.$v[Item.FOCUS] && inWindow(item)) setFocusInScope(scope, item, options)
}

Item.prototype.$enabledChanged = function(){
    if(!agent.root || !inWindow(this)) return
    enableChanged(this, scopeOf(this), isEnabled(this))
}

// the Keys and KeyNavigation attached group properties of every Item, key filters of the item
Item.prototype.$keyFilters = null
for(let [name, Group] of [['Keys', Keys], ['KeyNavigation', KeyNavigation]]){
    let prop = Item.$type.propByName.get(name)
    prop.group = Group
    let id = prop.id
    Object.defineProperty(Item.prototype, name, {
        get(){
            if(this.$v[id]) return this.$v[id]
            let group = this.$v[id] = new Group(this)
            if(!this.hasOwnProperty('$keyFilters')) this.$keyFilters = []
            this.$keyFilters.unshift(group)
            return group
        },
        configurable: true,
    })
}

// the window's contentItem
{
    let mount = Item.prototype.$mount
    Item.prototype.$mount = function(element){
        mount.call(this, element)
        if(agent.root) return
        agent.root = this
        this.$isFocusScope = true
        listen()
        if(typeof document !== 'undefined' && document.hasFocus()) setWindowFocused(true)
    }
}

module.exports = { agent, deliverTo, isEnabled, setWindowFocused, activateWindow, windowCreated, isSyncingDom: () => syncingDom, ACCEPTED, CONSUMED }
