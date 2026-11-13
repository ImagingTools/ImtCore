// Signals, bindings and the update phase of the runtime.
//
// One link per subscription: a Link for a connection or a QML handler, a DepLink for a binding dependency, which
// is also chained in the binding's dependency list. The subscribers of (source, signal) are a doubly linked list;
// source.$subs holds [signal, head, signal, head, ...] and the tail of a list is its head's prevSub.
// Emission order: geometry listeners (QQuickItemChangeListener), bindings of a bindable property
// (QPropertyObserver), then every other subscriber
// in the order it connected (QQmlNotifier).

const PRIORITY_GEOMETRY = 2
const PRIORITY_CROSS = 1
const PRIORITY_HANDLER = -1
const PRIORITY_NORMAL = 0


// object flags
const DESTROYED = 1
const BLOCKED = 2
const COMPLETED = 4
const DELETING = 8         // in a tree being destroyed (QQmlData::isQueuedForDeletion): no bindings, no guard notifications

// binding flags
const EVALUATING = 1
const UPDATING = 2
const DISPOSED = 4
const PENDING = 8          // created, not enabled yet (QQmlData pending binding bit)
const SETTER = 16          // written like a C++ setter (anchors), not like a QML binding
const PARITY = 32          // flips on every evaluation; a dependency marked with the other value was not read again
const DEPTH = 64           // nesting of updates, two bits: an anchor update may nest up to three levels

const stats = { bindings: 0, evaluations: 0, links: 0 }

// link flags
const GEOMETRY = 1      // runs before the other subscribers (PRIORITY_GEOMETRY)
const DEAD = 2          // unsubscribed, or its source is destroyed
const DEP = 4           // a DepLink
const SEEN = 8          // a DepLink read in the evaluation of the binding's current PARITY

// fn(receiver, ...args), or for a QML handler fn(receiver, scope, ...args)
class Link {
    constructor(source, signal, flags, fn, receiver, scope){
        this.source = source
        this.signal = signal
        this.flags = flags
        this.fn = fn
        this.receiver = receiver
        this.scope = scope
        this.prevSub = null
        this.nextSub = null
    }
}

// what a binding read: it is evaluated again when (source, signal) is emitted
class DepLink {
    constructor(source, signal, binding){
        this.source = source
        this.signal = signal
        this.flags = DEP
        this.binding = binding
        this.prevSub = null
        this.nextSub = null
        this.nextDep = null
    }
}

function slotOf(subs, signal){
    for(let i = 0; i < subs.length; i += 2){
        if(subs[i] === signal) return i
    }
    return -1
}

function headOf(obj, signal){
    let subs = obj.$subs
    if(!subs) return null
    let i = slotOf(subs, signal)
    return i < 0 ? null : subs[i + 1]
}

// push() would reserve about 16 slots on most objects, which have a few signals subscribed to
function addSlot(source, subs, signal, link){
    let size = subs ? subs.length : 0
    let grown = new Array(size + 2)
    for(let i = 0; i < size; i++) grown[i] = subs[i]
    grown[size] = signal
    grown[size + 1] = link
    source.$subs = grown
}

function subscribe(link){
    let source = link.source
    let subs = source.$subs
    let i = subs ? slotOf(subs, link.signal) : -1
    if(i < 0){
        link.prevSub = link
        addSlot(source, subs, link.signal, link)
    } else {
        let head = subs[i + 1]
        let tail = head.prevSub
        tail.nextSub = link
        link.prevSub = tail
        head.prevSub = link
    }
    stats.links++
    return link
}

function unsubscribe(link){
    if(link.flags & DEAD) return
    link.flags |= DEAD
    stats.links--
    let source = link.source
    if(source.$f & DESTROYED || !source.$subs) return
    let subs = source.$subs
    let i = slotOf(subs, link.signal)
    if(i < 0) return
    let head = subs[i + 1]
    let next = link.nextSub
    if(link === head){
        if(next){
            next.prevSub = link.prevSub
            subs[i + 1] = next
        } else {
            subs.splice(i, 2)
        }
    } else {
        link.prevSub.nextSub = next
        if(next) next.prevSub = link.prevSub
        else head.prevSub = link.prevSub
    }
}

// owner: the object whose destruction removes the connection (QObjectPrivate::connect with a receiver)
function connect(source, signal, fn, receiver = null, priority = PRIORITY_NORMAL, owner = null){
    let link = subscribe(new Link(source, signal, priority === PRIORITY_GEOMETRY ? GEOMETRY : 0, fn, receiver, null))
    if(owner !== null && owner !== source) (owner.$inbound || (owner.$inbound = [])).push(link)
    return link
}

function disconnect(source, signal, fn, receiver = null, owner = null){
    for(let link = headOf(source, signal); link; link = link.nextSub){
        if(!(link.flags & DEP) && link.fn === fn && link.receiver === receiver){
            unsubscribe(link)
            let inbound = owner !== null ? owner.$inbound : null
            let at = inbound ? inbound.indexOf(link) : -1
            if(at >= 0) inbound.splice(at, 1)
            return true
        }
    }
    return false
}

// obj is destroyed: the connections it receives go (a destroyed receiver is disconnected)
function dropConnections(obj){
    let inbound = obj.$inbound
    if(!inbound) return
    obj.$inbound = null
    for(let i = 0; i < inbound.length; i++) unsubscribe(inbound[i])
}

function connectHandler(source, signal, fn, scope){
    return subscribe(new Link(source, signal, 0, fn, source, scope))
}

// whether (obj, signal) has a live subscriber
function hasSubscribers(obj, signal){
    for(let link = headOf(obj, signal); link; link = link.nextSub){
        if(!(link.flags & DEAD)) return true
    }
    return false
}

// obj is destroyed: the bindings that read it drop their links on their next evaluation
function dropSubscribers(obj){
    let subs = obj.$subs
    if(!subs) return
    for(let i = 1; i < subs.length; i += 2){
        for(let link = subs[i]; link; link = link.nextSub) link.flags |= DEAD
    }
    obj.$subs = null
}

// ---- cleared contexts ----------------------------------------------------------------

// A scope is a QML context: its bindings and handlers stop once it is cleared. QQmlContextData::clearContext
// clears one context, clearContextRecursively also the contexts created in it (component bodies and the
// documents of the QML types built there)
const CLEARED_OWN = 1, CLEARED_TREE = 2
let anyCleared = false

function clearScope(scope, recursive){
    if(!scope || scope.$cleared === CLEARED_TREE) return
    scope.$cleared = recursive ? CLEARED_TREE : CLEARED_OWN
    anyCleared = true
}

// contexts in the order they were made: QQmlContextData::emitDestruction goes from the newest child context
let scopes = 0
function nextScope(){
    return ++scopes
}

function scopeCleared(scope){
    if(!anyCleared) return false
    if(scope.$cleared) return true
    for(let s = parentScope(scope), depth = 0; s && depth < 64; s = parentScope(s), depth++){
        if(s.$cleared === CLEARED_TREE) return true
    }
    return false
}

function parentScope(scope){
    let parent = scope.$p || (scope[0] && scope[0].$cctx) || null
    return parent === scope ? null : parent
}

// ---- errors ------------------------------------------------------------------

// QQmlDelayedError: while objects are being created (inProgressCreations > 0) a binding error is kept;
// a later successful evaluation of the binding drops it, the outermost creation prints what is left
let inProgressCreations = 0
const erroredBindings = new Map()     // binding -> message
let runningBinding = null
let runningFailed = false

function bindingError(binding, error, where){
    if(inProgressCreations > 0){
        erroredBindings.set(binding, formatError(error, where))
        if(binding === runningBinding) runningFailed = true
        return
    }
    reportError(error, where)
}

// an "Unable to assign" of the value a binding produced is a binding error too
function writeError(target, prop, error, where){
    let binding = runningBinding
    if(binding && binding.target === target && binding.prop === prop) bindingError(binding, error, where + ' (' + (binding.fn.name || 'binding') + ')')
    else reportError(error, where)
}

function formatError(error, where){
    let at = error && error.stack ? String(error.stack).split('\n')[1] : ''
    return 'JQML: ' + where + ': ' + (error && error.message ? error.message : error) + (at ? ' ' + at.trim() : '')
}

function flushErroredBindings(){
    let list = Array.from(erroredBindings.values())
    erroredBindings.clear()
    for(let message of list) console.error(message)
}

function reportError(error, where){
    let at = error && error.stack ? String(error.stack).split('\n')[1] : ''
    console.error('JQML: ' + where + ': ' + (error && error.message ? error.message : error) + (at ? ' ' + at.trim() : ''))
}

// ---- update phase -------------------------------------------------------------

let depth = 0
let dirty = []
let callLater = []
let deleteLater = []
let polish = []

function beginUpdate(){
    depth++
}

function endUpdate(){
    if(--depth > 0) return
    depth++
    try {
        // Order: deferred calls, DOM writes, deferred deletes. Polish runs with the next frame, and while one is
        // pending the DOM waits for it: a frame is rendered after its items are polished (QQuickWindow::polishItems)
        while(callLater.length){
            let calls = callLater
            callLater = []
            for(let fn of calls) fn()
        }
        if(dirty.length && !polish.length){
            let items = dirty
            dirty = []
            for(let item of items){
                let bits = item.$dirty
                item.$dirty = 0
                if(!(item.$f & DESTROYED)) item.$flush(bits)
            }
        }
        if(deleteLater.length){
            let objects = deleteLater
            deleteLater = []
            for(let obj of objects) obj.$destroyNow()
        }
    } finally {
        depth--
    }
}

function batch(fn){
    beginUpdate()
    try {
        return fn()
    } finally {
        endUpdate()
    }
}

function markDirty(item, bits){
    if(!item.$dirty) dirty.push(item)
    item.$dirty |= bits
    if(depth === 0) batch(() => {})
}

// QQuickItem::polish(): "When the scene graph processes the request, it will call updatePolish()";
// the render loop polishes items while it prepares the next frame (doc.qt.io scene graph render loop)
function queuePolish(item){
    if(item.$polishQueued) return
    item.$polishQueued = true
    polish.push(item)
    requestFrame()
}

function runPolish(){
    while(polish.length){
        let items = polish
        polish = []
        for(let item of items){
            item.$polishQueued = false
            if(!(item.$f & DESTROYED)) item.$polish()
        }
    }
}

// Work done once per frame (ListView: "Responding to changes in the model is usually batched
// to happen only once per frame", doc.qt.io/qt-6/qml-qtquick-listview.html)
// The frame runs before the browser paints (requestAnimationFrame); a hidden page gets no animation
// frames, so a timer runs it there
const FRAME_INTERVAL = 16
const HIDDEN_FRAME_INTERVAL = 100
let frameItems = []
let frameRequested = false
let frameHandle = 0
let frameTimer = 0

// Qt.quit(): "the application leaves the main event loop" (QCoreApplication::exit) —
// the running handler finishes, later frames and timers do not run
let stopped = false

function stop(){
    stopped = true
}

function isStopped(){
    return stopped
}

function runFrame(){
    if(!frameRequested) return
    frameRequested = false
    if(frameHandle) cancelAnimationFrame(frameHandle)
    clearTimeout(frameTimer)
    frameHandle = frameTimer = 0
    if(stopped) return
    let items = frameItems
    frameItems = []
    batch(() => {
        for(let item of items){
            item.$frameQueued = false
            if(!(item.$f & DESTROYED)) item.$frame()
        }
        runPolish()
    })
}

function requestFrame(){
    if(frameRequested) return
    frameRequested = true
    if(typeof requestAnimationFrame !== 'function'){
        frameTimer = setTimeout(runFrame, FRAME_INTERVAL)
        return
    }
    frameHandle = requestAnimationFrame(runFrame)
    frameTimer = setTimeout(runFrame, HIDDEN_FRAME_INTERVAL)
}

function queueFrame(item){
    if(item.$frameQueued) return
    item.$frameQueued = true
    frameItems.push(item)
    requestFrame()
}

// Runs pending frame work now (tests, forceLayout-like calls)
function flushFrame(){
    if(frameRequested || frameItems.length || polish.length) runFrame()
}

function queueCall(fn){
    callLater.push(fn)
    if(depth === 0) batch(() => {})
}

function queueDelete(obj){
    deleteLater.push(obj)
    if(depth === 0) batch(() => {})
}

// ---- emission ----------------------------------------------------------------

let active = null          // binding being evaluated; null while a handler runs
// of the binding being evaluated: its notify signal, the next dependency expected, the last one
let activeSignal = 0, activeCursor = null, activeTail = null
let scratch = []           // links of the emissions in progress (stack discipline)

// args: null for property change signals (a = old value, b = new value), an array for declared signals
function fire(link, args, a, b){
    if(link.flags & DEP){
        runBinding(link.binding)
        return
    }
    if(link.scope && scopeCleared(link.scope)) return
    let previous = active
    active = null
    try {
        if(link.scope !== null){
            if(args) link.fn.call(link.receiver, link.receiver, link.scope, ...args)
            else link.fn.call(link.receiver, link.receiver, link.scope, a, b)
        } else {
            if(args) link.fn.apply(link.receiver, args)
            else link.fn.call(link.receiver, a, b)
        }
    } catch(error) {
        reportError(error, 'handler of signal #' + link.signal)
    } finally {
        active = previous
    }
}

function emit(obj, signal, a, b){
    emitArgs(obj, signal, null, a, b)
}

function emitArgs(obj, signal, args, a, b){
    if(obj.$f & (DESTROYED | BLOCKED)) return
    let head = headOf(obj, signal)
    let handler = obj.constructor.$type.handlers[signal]
    if(!head && !handler) return

    beginUpdate()
    let start = scratch.length
    try {
        for(let link = head; link; link = link.nextSub) scratch.push(link)
        let end = scratch.length
        for(let i = start; i < end; i++){
            let link = scratch[i]
            if((link.flags & (GEOMETRY | DEAD)) === GEOMETRY) fire(link, args, a, b)
        }
        // a bindable property (QQuickItem x/y/width/height) set through its setter updates its bindings
        // (QPropertyObserver) before the notify signal; set by its own QML binding it notifies in connection order
        let bindable = obj.$bindableSignals !== undefined && obj.$bindableSignals.has(signal)
            && !(runningBinding && runningBinding.target === obj && selfSignal(runningBinding) === signal && !(runningBinding.flags & SETTER))
        if(bindable){
            for(let i = start; i < end; i++){
                let link = scratch[i]
                if((link.flags & (DEP | GEOMETRY | DEAD)) === DEP) fire(link, args, a, b)
            }
        }
        if(handler){
            let previous = active
            active = null
            try {
                if(args) handler(obj, ...args)
                else handler(obj, a, b)
            } catch(error) {
                reportError(error, obj.constructor.name + ' handler of signal #' + signal)
            } finally {
                active = previous
            }
        }
        // QQmlNotifier::emitNotify: the endpoints in the order they connected; QML handlers connect when the
        // object is created, bindings when they are first evaluated
        for(let i = start; i < end; i++){
            let link = scratch[i]
            if(!(link.flags & (GEOMETRY | DEAD)) && !(bindable && link.flags & DEP)) fire(link, args, a, b)
        }
    } finally {
        scratch.length = start
        endUpdate()
    }
}

// ---- bindings ----------------------------------------------------------------

const NO_VALUE = {}

// self is the object the expression is written in: the target itself, or the owner element
// for grouped properties (anchors.left) and aliases
class Binding {
    constructor(target, prop, fn, scope, self = target){
        this.target = target
        this.self = self
        this.prop = prop
        this.fn = fn
        this.scope = scope
        this.next = null        // next binding of the same target
        this.deps = null
        this.flags = 0
        stats.bindings++
    }
}

function selfSignal(binding){
    return binding.target.constructor.$type.props[binding.prop].signal
}

function mark(link, binding){
    link.flags = binding.flags & PARITY ? link.flags | SEEN : link.flags & ~SEEN
}

function track(obj, signal){
    let binding = active
    if(!binding) return
    if(obj === binding.target && signal === activeSignal) return

    let link = activeCursor
    if(link && link.source === obj && link.signal === signal){
        mark(link, binding)
        activeCursor = link.nextDep
        return
    }
    for(link = binding.deps; link; link = link.nextDep){
        if(link.source === obj && link.signal === signal){
            mark(link, binding)
            return
        }
    }
    link = new DepLink(obj, signal, binding)
    mark(link, binding)
    let tail = activeTail
    if(!tail) for(tail = binding.deps; tail && tail.nextDep; tail = tail.nextDep);
    if(tail) tail.nextDep = link
    else binding.deps = link
    activeTail = link
    subscribe(link)
}

function evaluate(binding){
    if(binding.flags & EVALUATING){
        console.warn('JQML: ' + binding.target.constructor.name + ': Binding loop detected for property "' + binding.target.constructor.$type.props[binding.prop].name + '"' + (binding.fn.name ? ' (' + binding.fn.name + ')' : ''))
        return NO_VALUE
    }
    stats.evaluations++
    binding.flags |= EVALUATING
    binding.flags ^= PARITY
    let previous = active, previousSignal = activeSignal, previousCursor = activeCursor, previousTail = activeTail
    activeSignal = selfSignal(binding)
    activeCursor = binding.deps
    activeTail = null
    active = binding
    let value = NO_VALUE
    try {
        value = binding.fn.call(binding.self, binding.self, binding.scope)
    } catch(error) {
        bindingError(binding, error, binding.target.constructor.name + '.' + binding.target.constructor.$type.props[binding.prop].name)
    } finally {
        active = previous
        activeSignal = previousSignal
        activeCursor = previousCursor
        activeTail = previousTail
        binding.flags &= ~EVALUATING
        sweep(binding)
    }
    return value
}

function sweep(binding){
    let previous = null
    let link = binding.deps
    while(link){
        let next = link.nextDep
        if(!(link.flags & SEEN) !== !(binding.flags & PARITY)){
            unsubscribe(link)
            if(previous) previous.nextDep = next
            else binding.deps = next
        } else {
            previous = link
        }
        link = next
    }
}

// Like QQmlBinding::update: the flag covers evaluation, the write and the notification.
function runBinding(binding){
    if(binding.flags & DISPOSED || binding.target.$f & (DESTROYED | DELETING)) return
    if(binding.scope && scopeCleared(binding.scope)){
        binding.target.$takeBinding(binding.prop)
        disposeBinding(binding)
        return
    }
    if(binding.flags & UPDATING){
        let name = binding.target.constructor.$type.props[binding.prop].name
        // QQuickAnchorsPrivate::updateHorizontalAnchors: an anchor update may nest up to three levels
        if(!(binding.flags & SETTER)){
            console.warn('JQML: ' + binding.target.constructor.name + ': Binding loop detected for property "' + name + '"' + (binding.fn.name ? ' (' + binding.fn.name + ')' : ''))
            return
        }
        if(binding.flags >= 3 * DEPTH){
            console.warn('JQML: Possible anchor loop detected on ' + (name === 'x' || name === 'width' ? 'horizontal' : 'vertical') + ' anchor.')
            return
        }
    }
    binding.flags += DEPTH
    binding.flags |= UPDATING
    let previousBinding = runningBinding, previousFailed = runningFailed
    runningBinding = binding
    runningFailed = false
    try {
        let value = evaluate(binding)
        // an error further down the notification does not stop the other subscribers of the signal
        if(value !== NO_VALUE){
            try {
                binding.target.$write(binding.prop, value)
            } catch(error) {
                reportError(error, binding.target.constructor.name + '.' + binding.target.constructor.$type.props[binding.prop].name)
            }
        }
        if(!runningFailed && erroredBindings.size) erroredBindings.delete(binding)
    } finally {
        binding.flags -= DEPTH
        if(binding.flags < DEPTH) binding.flags &= ~UPDATING
        runningBinding = previousBinding
        runningFailed = previousFailed
    }
}

function isUpdating(binding){
    return !!(binding.flags & UPDATING)
}

function disposeBinding(binding){
    if(erroredBindings.size) erroredBindings.delete(binding)
    if(binding.flags & PENDING){
        binding.flags &= ~PENDING
        binding.target.$pending--
    }
    binding.flags |= DISPOSED
    for(let link = binding.deps; link; link = link.nextDep) unsubscribe(link)
    binding.deps = null
    stats.bindings--
}

// Bindings created while a component is being built are evaluated once the whole tree exists.
let creating = null

function beginCreate(){
    let previous = creating
    creating = { bindings: [], componentComplete: [], completed: [], roots: [] }
    inProgressCreations++
    beginUpdate()
    return previous
}

function endCreate(previous){
    let current = creating
    creating = previous
    try {
        // QQmlObjectCreator::finalize: bindings are enabled from the last created to the first
        // (allCreatedBindings is a stack), then componentComplete runs the same way
        // (allParserStatusCallbacks.back()/pop_back()), then the Component.onCompleted handlers
        for(let i = current.bindings.length - 1; i >= 0; i--){
            let binding = current.bindings[i]
            if(!(binding.flags & DISPOSED)) enablePending(binding)
        }
        for(let i = current.componentComplete.length - 1; i >= 0; i--){
            let obj = current.componentComplete[i]
            if(!(obj.$f & DESTROYED)) obj.$componentComplete()
        }
        for(let obj of completionOrder(current)) obj.$complete()
    } finally {
        endUpdate()
        if(--inProgressCreations === 0 && erroredBindings.size) flushErroredBindings()
    }
}

// Component.onCompleted order of the current runtime (confirmed against Qt by the tests):
// an object, then its children from the last to the first, depth first.
function completionOrder(current){
    if(!current.completed.length) return current.completed
    let pending = new Set(current.completed)
    let order = []
    let visit = (obj) => {
        if(pending.delete(obj)) order.push(obj)
        let children = obj.$children
        if(children){
            for(let i = children.length - 1; i >= 0; i--) visit(children[i])
        }
    }
    for(let root of current.roots) visit(root)
    for(let obj of current.completed){
        if(pending.has(obj)) order.push(obj)
    }
    return order
}

// The context a QML type is being built in: its document sees it while it is built (the parent of its QQmlContext)
let buildContext = null

function make(Class, s){
    let previous = buildContext
    buildContext = s
    let obj
    try {
        obj = new Class(null)
    } finally {
        buildContext = previous
    }
    obj.$cctx = s
    return obj
}

function inContext(root){
    if(!root.$cctx && buildContext) root.$cctx = buildContext
}

// builds with fn() an object of a component made in the context s
function buildIn(s, fn){
    let previous = buildContext
    buildContext = s
    try {
        return fn()
    } finally {
        buildContext = previous
    }
}

function scheduleComponentComplete(obj){
    if(obj.$ccScheduled) return
    obj.$ccScheduled = true
    if(creating) creating.componentComplete.push(obj)
    else obj.$componentComplete()
}

function creatingRoot(obj){
    if(creating && obj) creating.roots.push(obj)
}

function scheduleBinding(binding){
    if(creating){
        binding.flags |= PENDING
        binding.target.$pending = (binding.target.$pending || 0) + 1
        creating.bindings.push(binding)
    } else runBinding(binding)
}

// Enables a pending binding once: from finalize, or earlier when JS reads the property
function enablePending(binding){
    if(!(binding.flags & PENDING)) return
    binding.flags &= ~PENDING
    binding.target.$pending--
    runBinding(binding)
}

// QObjectWrapper::getProperty -> QQmlData::flushPendingBinding: reading a property whose binding is still
// pending enables that binding first. Anchors are bindings of anchors.*, not of x/width, so they stay pending
function flushPending(obj, prop){
    for(let binding = obj.$bindings; binding; binding = binding.next){
        if(binding.prop === prop && binding.flags & PENDING && !(binding.flags & SETTER)){
            enablePending(binding)
            return
        }
    }
}

function scheduleCompleted(obj){
    if(creating) creating.completed.push(obj)
    else obj.$complete()
}

module.exports = {
    PENDING,
    PRIORITY_GEOMETRY, PRIORITY_CROSS, PRIORITY_HANDLER, PRIORITY_NORMAL,
    DESTROYED, BLOCKED, COMPLETED, DELETING, SETTER,
    clearScope, scopeCleared, nextScope, parentScope, make, inContext, buildIn,
    Link, Binding, stats,
    connect, disconnect, connectHandler, subscribe, unsubscribe, hasSubscribers, dropSubscribers, dropConnections,
    emit, emitArgs, track, evaluate, runBinding, disposeBinding, isUpdating, flushPending,
    beginUpdate, endUpdate, batch, markDirty, queuePolish, queueCall, queueDelete, queueFrame, flushFrame, stop, isStopped,
    beginCreate, endCreate, creatingRoot, scheduleComponentComplete, scheduleBinding, scheduleCompleted,
    reportError,
    writeError, bindingError
}
