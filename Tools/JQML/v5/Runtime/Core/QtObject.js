const engine = require('./engine')
const { defineType } = require('./type')

// set for one $write: animations write past the Behavior of the property (BypassInterceptor)
let bypassInterceptor = false
function writeBypassingInterceptor(obj, id, value){
    bypassInterceptor = true
    obj.$write(id, value)
    bypassInterceptor = false
}

// Qt.binding(fn) result; assigning it to a property creates a binding instead of a value
class QtBinding {
    constructor(fn){
        this.fn = fn
    }
}

// Base of every QML object: property values in $v, subscribers in $subs,
// active bindings chained in $bindings, tree in $parent/$children.
class QtObject {
    constructor(parent = null){
        this.$v = this.constructor.$type.initial.slice()
        this.$f = 0
        this.$subs = null
        this.$bindings = null
        this.$parent = null
        this.$children = null
        this.$scope = null
        if(parent) this.$setParent(parent)
    }

    // Assignment from QML code: breaks the binding of the property.
    // Assignment from QML code breaks the binding of the property, except when it happens while that
    // binding is updating (a handler of its own change notification, WebDiff_137)
    $assign(id, value){
        if(value instanceof QtBinding){
            let fn = value.fn
            this.$bind(id, (self) => fn.call(self))
            return
        }
        let binding = this.$findBinding(id)
        if(binding && !engine.isUpdating(binding)){
            this.$takeBinding(id)
            engine.disposeBinding(binding)
        }
        this.$write(id, value)
    }

    // Write without touching the binding (used by bindings and the runtime).
    $write(id, value){
        // a Behavior on the property gets the write instead (QQmlPropertyValueInterceptor)
        let skip = bypassInterceptor
        bypassInterceptor = false
        if(!skip && this.$icpt !== undefined && this.$icpt[id]){
            this.$icpt[id].$intercept(value)
            return
        }
        // parent is a property for bindings (parent: someItem); writing it reparents the object
        if(id === 1){
            this.$setParent(value === undefined ? null : value)
            return
        }
        let prop = this.constructor.$type.props[id]
        if(prop.coerce){
            try {
                value = prop.coerce(value)
            } catch(error) {
                engine.writeError(this, id, error, this.constructor.name + '.' + prop.name)
                return
            }
        }
        let old = this.$v[id]
        if(old === value || (old !== old && value !== value)) return
        this.$v[id] = value
        if(prop.guarded){
            if(old instanceof QtObject) unguard(old, this, id)
            if(value instanceof QtObject) guard(value, this, id)
        }
        if(prop.effect) prop.effect(this, value, old)
        engine.emit(this, prop.signal, old, value)
    }

    $bind(id, fn, scope, self = this){
        let previous = this.$takeBinding(id)
        if(previous) engine.disposeBinding(previous)
        let binding = new engine.Binding(this, id, fn, scope, self)
        binding.next = this.$bindings
        this.$bindings = binding
        engine.scheduleBinding(binding)
        return binding
    }

    $findBinding(id){
        for(let binding = this.$bindings; binding; binding = binding.next){
            if(binding.prop === id) return binding
        }
        return null
    }

    $takeBinding(id){
        let previous = null
        for(let binding = this.$bindings; binding; binding = binding.next){
            if(binding.prop === id){
                if(previous) previous.next = binding.next
                else this.$bindings = binding.next
                binding.next = null
                return binding
            }
            previous = binding
        }
        return null
    }

    // Handler of one of this object's signals, declared in QML (onXxx:)
    $on(signal, fn, scope){
        return engine.connectHandler(this, signal, fn, scope === undefined ? this.$scope : scope)
    }

    // QObjectWrapper::virtualGetOwnProperty: the properties, signals and methods of the object are its own
    hasOwnProperty(name){
        if(Object.prototype.hasOwnProperty.call(this, name)) return true
        if(typeof name !== 'string' || name[0] === '$' || name === 'constructor') return false
        for(let proto = Object.getPrototypeOf(this); proto && proto !== Object.prototype; proto = Object.getPrototypeOf(proto)){
            if(Object.prototype.hasOwnProperty.call(proto, name)) return true
        }
        return false
    }

    // attached Component object: obj.Component.destruction.connect(fn)
    get Component(){
        return this.$attachedComponent || (this.$attachedComponent = { completed: this.completed, destruction: this.destruction })
    }

    get children(){
        return this.$children ? this.$children.slice() : []
    }

    $setParent(parent){
        let old = this.$parent
        if(old === parent) return
        if(old && old.$children){
            let index = old.$children.indexOf(this)
            if(index >= 0) old.$children.splice(index, 1)
        }
        this.$parent = parent
        if(parent){
            let children = parent.$children
            // most objects have a few children: not the 16 slots push() would reserve
            if(!children) parent.$children = [this]
            else if(children.length < 8) parent.$children = [...children, this]
            else children.push(this)
        }
        this.$parentChanged(old, parent)
        engine.emit(this, QtObject.SIGNAL_parentChanged, old, parent)
    }

    $parentChanged(old, parent){}

    $componentComplete(){
        this.$f |= engine.COMPLETED
    }

    $complete(){
        this.$f |= engine.COMPLETED
        engine.emit(this, QtObject.SIGNAL_completed)
    }

    // QObject::destroy(int delay): deleted after delay milliseconds, or after the current event
    destroy(delay){
        if(typeof delay === 'number' && delay > 0) setTimeout(() => engine.queueDelete(this), delay)
        else engine.queueDelete(this)
    }

    // A view lets go of a delegate (QQmlDelegateModelItem::destroyObject clears the delegate's context) or a Loader of
    // its item (QQuickLoaderPrivate::clear, recursively): those bindings and handlers stop at once, the object is
    // deleted later, so the other handlers of the signal that caused it still run (QQuickRepeater::clear releases
    // the items first and unparents them after that)
    $releaseLater(hide = false, recursive = false){
        if(this.$f & engine.DESTROYED) return
        // QQmlContextData::clearContext emits Component.onDestruction of the context first
        let own = this.$ownScope()
        if(own) emitContextDestruction(this, own)
        engine.clearScope(own, recursive)
        if(recursive){
            for(let key in this){
                if(key.startsWith('$s_')) engine.clearScope(this[key], true)
            }
        }
        if(hide && this.constructor.VISIBLE !== undefined) this.$write(this.constructor.VISIBLE, false)
        if(this.$parent) this.$setParent(null)
        engine.queueDelete(this)
    }

    // the context the object was created in as a component root: its body, or its document when built from a file
    $ownScope(){
        return this.$own || (this.constructor.$scopeKey ? this[this.constructor.$scopeKey] : null) || null
    }

    $destroyNow(){
        if(this.$f & engine.DESTROYED) return
        // qdeclarativeelement_destructor: the context the object owns emits Component.onDestruction while its bindings
        // still run, then QQmlData::markAsDeleted goes down the tree and stops them
        if(!(this.$f & engine.DELETING)){
            let own = this.$ownScope()
            if(own) invalidateContext(this, own)
            markAsDeleted(this)
        }
        // QQmlData::destroyed fires the guards before ~QObject deletes the children
        dropHolder(this)
        releaseGuards(this)
        if(this.$idScope) releaseId(this, this.$idScope)
        if(this.$children){
            for(let i = this.$children.length - 1; i >= 0; i--) this.$children[i].$destroyNow()
        }
        for(let binding = this.$bindings; binding; binding = binding.next) engine.disposeBinding(binding)
        this.$bindings = null
        // grouped properties (anchors, font, border) are not children: their bindings would stay subscribed and keep the object
        let groups = this.constructor.$type.groups
        for(let i = 0; i < groups.length; i++){
            let group = this.$v[groups[i]]
            if(group) group.$destroyNow()
        }
        engine.dropSubscribers(this)
        engine.dropConnections(this)
        if(this.$parent && !(this.$parent.$f & engine.DESTROYED)) this.$setParent(null)
        this.$f |= engine.DESTROYED
        let creator = this.$own && (this.$own.$p || this.$cctx)
        if(creator && creator.$made) creator.$made.delete(this)
        this.$disposeNative()
    }

    $disposeNative(){}
}

// QQmlVMEMetaObject guards the object a var (or object typed) property holds: its destruction sets the property to
// null and notifies, so the bindings on it see the change. target.$guards: holder, slot pairs, one per property that
// holds it ($write guards only a changed value); a guard goes when the property takes another value or the holder is destroyed.
function guard(target, holder, slot){
    let guards = target.$guards
    if(guards) guards.push(holder, slot)
    else target.$guards = [holder, slot]
}

function unguard(target, holder, slot){
    let guards = target.$guards
    if(!guards) return
    for(let i = guards.length - 2; i >= 0; i -= 2){
        if(guards[i] !== holder || guards[i + 1] !== slot) continue
        let last = guards.length - 2
        guards[i] = guards[last]
        guards[i + 1] = guards[last + 1]
        guards.length = last
        return
    }
}

// QQmlData::markAsDeleted: the object, then its children from the last one; a context met on the way emits its
// destruction first (setQueuedForDeletion -> deepClearContextObject)
function markAsDeleted(object){
    let stack = [object]
    while(stack.length){
        let current = stack.pop()
        let own = current.$ownScope()
        if(own) invalidateContext(current, own)
        current.$f |= engine.DELETING
        let children = current.$children
        if(children) for(let i = 0; i < children.length; i++) stack.push(children[i])
    }
}

// QQmlContextData::emitDestruction, once per context: its objects in the reverse order of Component.onCompleted,
// then the contexts made in it from the newest; an object belongs to the nearest context owner up its tree
function emitContextDestruction(owner, scope){
    if(scope.$destructed) return
    scope.$destructed = true
    let owners = []
    emitOwnDestruction(owner, owners)
    if(scope.$made) for(let made of scope.$made) owners.push(made)
    if(owners.length > 1) owners.sort((a, b) => (b.$ownScope().$n || 0) - (a.$ownScope().$n || 0))
    for(let i = 0; i < owners.length; i++) emitContextDestruction(owners[i], owners[i].$ownScope())
}

// QQmlContextData::invalidate: the bindings of the context and of the contexts made in it stop, objects living elsewhere too
function invalidateContext(owner, scope){
    emitContextDestruction(owner, scope)
    engine.clearScope(scope, true)
}

function emitOwnDestruction(object, owners){
    let children = object.$children
    if(children){
        for(let i = 0; i < children.length; i++){
            let child = children[i]
            if(child.$ownScope()) owners.push(child)
            else emitOwnDestruction(child, owners)
        }
    }
    engine.emit(object, QtObject.SIGNAL_destruction)
}

// QQmlContextData id guard: the id of a destroyed object reads null; the bindings of a context living on, and of the
// contexts below it, are evaluated again (those reading the id see the change, the others keep their values)
function releaseId(object, scope){
    for(let i = 1; i < scope.length; i++){
        if(scope[i] === object) scope[i] = null
    }
    let root = scope[0]
    if(!root || root.$f & (engine.DESTROYED | engine.DELETING)) return
    let bindings = []
    let stack = [root]
    while(stack.length){
        let current = stack.pop()
        for(let binding = current.$bindings; binding; binding = binding.next){
            for(let s = binding.scope, depth = 0; s && depth < 64; s = engine.parentScope(s), depth++){
                if(s === scope){
                    bindings.push(binding)
                    break
                }
            }
        }
        let children = current.$children
        if(children) for(let i = 0; i < children.length; i++) stack.push(children[i])
    }
    engine.beginUpdate()
    try {
        for(let binding of bindings) engine.runBinding(binding)
    } finally {
        engine.endUpdate()
    }
}

// a destroyed holder lets go of the objects its var properties hold
function dropHolder(holder){
    let ids = holder.constructor.$type.varIds
    for(let i = 0; i < ids.length; i++){
        let value = holder.$v[ids[i]]
        if(value instanceof QtObject) unguard(value, holder, ids[i])
    }
}

function releaseGuards(target){
    let guards = target.$guards
    if(!guards) return
    target.$guards = null
    for(let i = 0; i < guards.length; i += 2){
        let holder = guards[i], slot = guards[i + 1]
        // QQmlVMEVariantQObjectPtr::objectDestroyedImpl: a holder that is being deleted is not told
        if(holder.$f & engine.DELETING || holder.$v[slot] !== target) continue
        holder.$v[slot] = null
        engine.emit(holder, holder.constructor.$type.props[slot].signal, target, null)
    }
}

defineType(QtObject, {
    props: {
        objectName: 'string',
        parent: {
            kind: 'var',
            get(){
                if(this.$pending) engine.flushPending(this, 1)
                engine.track(this, QtObject.SIGNAL_parentChanged)
                return this.$parent
            },
            set(parent){ this.$assign(1, parent) },
        },
    },
    signals: ['completed', 'destruction'],
})

// Grouped properties (anchors, font, border): owned by an object, created on first access.
class GroupObject extends QtObject {
    constructor(owner){
        super(null)
        this.$owner = owner
    }
}

defineType(GroupObject)

module.exports = { QtObject, GroupObject, QtBinding, writeBypassingInterceptor }
