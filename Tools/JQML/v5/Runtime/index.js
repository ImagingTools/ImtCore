// The runtime, exposed as window.JQ: the QML types by name and what generated code calls.
const engine = require('./Core/engine')
const { defineType } = require('./Core/type')
const { QtObject, GroupObject } = require('./Core/QtObject')
const { setRootProvider } = require('./Core/Resources')
const { Qt } = require('./Qt/Qt')
const { files, load } = require('./QtQml/files')
const { Component } = require('./QtQml/Component')
const { Timer } = require('./QtQml/Timer')
const { Connections } = require('./QtQml/Connections')
const { Binding } = require('./QtQml/Binding')
const { ListModel } = require('./QtQml/Models/ListModel')
const { Instantiator } = require('./QtQml/Models/Instantiator')
const { Item, Anchors } = require('./QtQuick/Item')
const { FocusScope } = require('./QtQuick/FocusScope')
const { Keys } = require('./QtQuick/Keys')
const { KeyNavigation } = require('./QtQuick/KeyNavigation')
require('./QtQuick/KeyDelivery')
const { Rectangle, Border } = require('./QtQuick/Rectangle')
const { Gradient, GradientStop } = require('./QtQuick/Gradient')
const { Text } = require('./QtQuick/Text')
const { Font } = require('./QtQuick/Font')
const { TextInput } = require('./QtQuick/TextInput')
const { TextEdit } = require('./QtQuick/TextEdit')
const { Image } = require('./QtQuick/Image')
const { Canvas } = require('./QtQuick/Canvas')
const { MouseArea, MouseDrag } = require('./QtQuick/MouseArea')
const { DropArea } = require('./QtQuick/DropArea')
const { Flickable } = require('./QtQuick/Flickable')
const { Column } = require('./QtQuick/Column')
const { Row } = require('./QtQuick/Row')
const { Flow } = require('./QtQuick/Flow')
const { Repeater } = require('./QtQuick/Repeater')
const { Loader } = require('./QtQuick/Loader')
const { ListView } = require('./QtQuick/ListView')
const { GridView } = require('./QtQuick/GridView')
const { Shortcut, StandardKey } = require('./QtQuick/Shortcut')
const { IntValidator } = require('./QtQuick/IntValidator')
const { DoubleValidator } = require('./QtQuick/DoubleValidator')
const { RegularExpressionValidator, RegExpValidator } = require('./QtQuick/RegularExpressionValidator')
const { FontLoader } = require('./QtQuick/FontLoader')
const { State } = require('./QtQuick/State')
const { PropertyChanges } = require('./QtQuick/PropertyChanges')
const { StateChangeScript } = require('./QtQuick/StateChangeScript')
const { Transition } = require('./QtQuick/Transition')
const { StateGroup } = require('./QtQuick/StateGroup')
const { Animation } = require('./QtQuick/Animation')
const { PauseAnimation } = require('./QtQuick/PauseAnimation')
const { PropertyAnimation } = require('./QtQuick/PropertyAnimation')
const { NumberAnimation } = require('./QtQuick/NumberAnimation')
const { ColorAnimation } = require('./QtQuick/ColorAnimation')
const { RotationAnimation } = require('./QtQuick/RotationAnimation')
const { SmoothedAnimation } = require('./QtQuick/SmoothedAnimation')
const { XAnimator, YAnimator, ScaleAnimator, RotationAnimator, OpacityAnimator } = require('./QtQuick/Animators')
const { PropertyAction } = require('./QtQuick/PropertyAction')
const { ScriptAction } = require('./QtQuick/ScriptAction')
const { SequentialAnimation } = require('./QtQuick/SequentialAnimation')
const { ParallelAnimation } = require('./QtQuick/ParallelAnimation')
const { Behavior } = require('./QtQuick/Behavior')
const { Easing } = require('./QtQuick/Easing')
const { EasingGroup } = require('./QtQuick/EasingGroup')
const { LayoutAttached } = require('./QtQuick/Layouts/Layout')
const { RowLayout } = require('./QtQuick/Layouts/RowLayout')
const { ColumnLayout } = require('./QtQuick/Layouts/ColumnLayout')
const { GridLayout } = require('./QtQuick/Layouts/GridLayout')
const { StackLayout } = require('./QtQuick/Layouts/StackLayout')
const { LayoutItemProxy } = require('./QtQuick/Layouts/LayoutItemProxy')
const { Window } = require('./QtQuick/Window/Window')
const { FileDialog } = require('./QtQuick/Dialogs/FileDialog')
const { FolderDialog } = require('./QtQuick/Dialogs/FolderDialog')
const { Settings } = require('./QtCore/Settings')
const { QmlWebSocket } = require('./QtWebSockets/WebSocket')
const { DropShadow } = require('./Qt5Compat/GraphicalEffects/DropShadow')
const { TestCase } = require('./QtTest/TestCase')
const { SignalSpy } = require('./QtTest/SignalSpy')
const { BaseClass } = require('./ImtCore/BaseClass')
const { BaseModel } = require('./ImtCore/BaseModel')
const { TreeItemModel } = require('./ImtCore/TreeItemModel')
const { GqlModel } = require('./ImtCore/GqlModel')
const { GqlRequest } = require('./ImtCore/GqlRequest')
const { FileIO } = require('./ImtCore/FileIO')
const { Process } = require('./ImtCore/Process')
const { RemoteFileController } = require('./ImtCore/RemoteFileController')
const { NavigationHistory } = require('./ImtCore/NavigationHistory')

require('./Core/ValueTypes').installConsole()

// Builds a component: bindings are evaluated and onCompleted handlers run once the whole tree exists.
function create(build){
    let previous = engine.beginCreate()
    try {
        let root = build()
        engine.creatingRoot(root)
        return root
    } finally {
        engine.endCreate(previous)
    }
}

// a handler written on a grouped attached property (Keys.onPressed) runs with the item as its object
// x.createQmlObject(...) written in QML (compiler/expr.js): when x is Qt, the object is created in the context of the
// call (QV4 callingQmlContext): the calling file, its scope and the names of the ids; any other x is called as is
function callCreateQmlObject(target, fileQt, scope, ids, ...args){
    if(target && target.$isQt === true) return Qt.$createQmlObject(fileQt.$base || '', scope, ids, ...args)
    return target.createQmlObject(...args)
}

// x.createComponent(...) written in QML: a component of Qt is made in the context of the call (its creation context)
function callCreateComponent(target, scope, ...args){
    let component = target.createComponent(...args)
    if(target && target.$isQt === true && component && component.$build !== undefined) component.$cctx = scope
    return component
}

function handleGroup(group, signal, fn, scope, owner){
    return engine.subscribe(new engine.Link(group, signal, 0, fn, owner, scope))
}

// property alias: the alias' change signal follows the target property
function forward(source, sourceSignal, target, targetSignal){
    engine.connect(source, sourceSignal, (a, b) => engine.emit(target, targetSignal, a, b))
}

// Binding written on an alias goes to the aliased property, evaluated with the alias owner as self
function bindAlias(owner, name, fn, scope){
    let [target, id] = owner.constructor.$aliases[name](owner)
    return target.$bind(id, fn, scope, owner)
}

// QML methods are bound to their object, also when read as a value: obj.ping.connect(handler).
// The bound function is made on first access and kept on the object.
function defineMethod(Class, name, fn){
    let key = '$m_' + name
    Object.defineProperty(Class.prototype, name, {
        get(){
            return Object.prototype.hasOwnProperty.call(this, key) ? this[key] : (this[key] = fn.bind(this))
        },
        set(value){
            this[key] = value
        },
        configurable: true,
    })
}

// Parent for the children of obj: the target of a default property alias of Class, or obj itself
function defaultParent(Class, obj){
    return Class.$defaultParent ? Class.$defaultParent(obj) : obj
}

// Root context properties: "context" is CClientUserContextComp on the desktop, the web context in a browser
const rootContext = { context: typeof window !== 'undefined' ? new (require('./ImtCore/WebContext').WebContext)() : null }

// A name the compiler could not resolve: the context chain from the scope up through the scopes the
// components were created in (QQmlContext parents). Each context: delegate roles, model / index / modelData, its ids
// ($s.$i, codegen scopeNames), the properties of its object (the scope root); then the root context
function ctx(s, name){
    let found = lookup(s, name)
    if(found === MISSING) throw new ReferenceError(name + ' is not defined')
    return found
}

// `typeof name`: undefined for a name that is nowhere (compiler/expr.js)
function ctxValue(s, name){
    let found = lookup(s, name)
    return found === MISSING ? undefined : found
}

const MISSING = {}

function lookup(s, name){
    for(let scope = s; scope;){
        let c = scope.$ctx
        if(c){
            if(name === 'model') return c.model
            if(name === 'index') return c.index
            if(name === 'modelData') return c.modelData
            if(c.$row && typeof c.$row === 'object' && name in c.$row) return c.$role(name)
        }
        let names = scope.$i
        if(names && Object.prototype.hasOwnProperty.call(names.ids, name)) return scope[names.ids[name]]
        let object = scope[0]
        if(object && typeof object === 'object' && name[0] !== '$' && name in object) return object[name]
        if(scope.$p) scope = scope.$p
        else scope = scope[0] && scope[0].$cctx || null
    }
    if(name in rootContext) return rootContext[name]
    return MISSING
}

// The nearest model context up the context chain: a component declared in a delegate and created by a Loader
// sees the delegate's model, index and roles
function mctx(s){
    for(let scope = s; scope;){
        if(scope.$ctx) return scope.$ctx
        if(scope.$p) scope = scope.$p
        else scope = scope[0] && scope[0].$cctx || null
    }
    return null
}

const modules = {}      // module -> type name -> class
const scripts = {}      // url of a JavaScript resource -> its script (compiler/scripts.js)

// A JavaScript resource: factory(Qt, the context scope, ...its imports) makes an instance and returns its qualifier
// pure: made on its first use (compiler/scripts.js isPure), any other script with each document instance
function script(url, library, factory, imports, pure = false){
    return { url, library, factory, imports, pure, shared: null }
}

// QQmlScriptData::scriptValueForContext: a .pragma library script has one instance, any other one per document
// instance (s: the scope of the document), made on its first use
function si(ref, s){
    if(ref.library || !s) return ref.shared || (ref.shared = instantiate(ref, null))
    let made = s.$js || (s.$js = new Map())
    let instance = made.get(ref)
    if(!instance){
        instance = instantiate(ref, s)
        made.set(ref, instance)
    }
    return instance
}

// the scripts of a document instance, made with its context (QQmlObjectCreator::init): refs or lists of them
function sis(s, refs){
    for(let ref of refs){
        if(Array.isArray(ref)) sis(s, ref)
        else if(!ref.pure) si(ref, s)
    }
}

const moduleScripts = {}    // module -> its qmldir scripts

// a script imported by a script: a library is shared, another script gets an instance of its own in the same context
function instantiate(ref, s){
    let imports = ref.imports().map(i => i && i.factory ? (i.library ? si(i, null) : instantiate(i, s)) : i)
    return ref.factory(Qt.$at(ref.url), s, ...imports)
}

// a free name of a script: the importing context, then the global object
function sctx(s, name){
    let found = s ? lookup(s, name) : MISSING
    if(found !== MISSING) return found
    if(name in globalThis) return globalThis[name]
    throw new ReferenceError(name + ' is not defined')
}

function sctxValue(s, name){
    let found = s ? lookup(s, name) : MISSING
    if(found !== MISSING) return found
    return globalThis[name]
}

// `name = value` of a script: a property of an object of the importing context, else a global
function sset(s, name, value){
    for(let scope = s; scope;){
        let object = scope[0]
        if(object && typeof object === 'object' && name[0] !== '$' && name in object){
            object[name] = value
            return value
        }
        if(scope.$p) scope = scope.$p
        else scope = scope[0] && scope[0].$cctx || null
    }
    globalThis[name] = value
    return value
}
const namespaces = {}

function qmlModule(name){
    return modules[name] || (modules[name] = {})
}

// `import imtcontrols 1.0 as ImtControls`: ImtControls.X is the type X of that module; '' is a Qt module
function ns(name){
    if(!namespaces[name]){
        let types = qmlModule(name)
        namespaces[name] = new Proxy(types, {
            get(target, key){
                if(typeof key !== 'string') return undefined
                let Class = load(types[key]) || JQ.types[key]
                if(Class && Class.$singleton) return singleton(Class)
                return Class
            },
        })
    }
    return namespaces[name]
}

// `icon.width: 16` where icon is an alias or a property holding an object: the object's property is
// assigned or bound (the expression keeps the owner as its scope object); a plain value is rebuilt
function subAssign(owner, name, entries, scope){
    let target = owner[name]
    if(target && target.constructor && target.constructor.$type && target.$v){
        for(let [key, isExpression, value] of entries){
            let prop = target.constructor.$type.propByName.get(key)
            if(!prop){
                engine.reportError('Cannot assign to non-existent property "' + key + '"', owner.constructor.name + '.' + name)
                continue
            }
            if(prop.group){
                engine.reportError('Cannot assign to group property "' + key + '"', owner.constructor.name + '.' + name)
                continue
            }
            if(isExpression) target.$bind(prop.id, value, scope, owner)
            else target[key] = value
        }
        return
    }
    let prop = owner.constructor.$type.propByName.get(name)
    if(!prop){
        engine.reportError('Cannot assign to non-existent property "' + name + '"', owner.constructor.name)
        return
    }
    let base = target && typeof target === 'object' ? target : {}
    let build = (self, s) => {
        let value = Object.assign(Object.create(Object.getPrototypeOf(base)), base)
        for(let [key, isExpression, v] of entries) value[key] = isExpression ? v.call(self, self, s) : v
        return value
    }
    if(entries.some(entry => entry[1])) owner.$bind(prop.id, build, scope)
    else owner[name] = build(owner, scope)
}

// pragma Singleton types: one instance, created on first use
function singleton(Class){
    if(!Object.prototype.hasOwnProperty.call(Class, '$instance')) Class.$instance = create(() => new Class(null))
    return Class.$instance
}

const JQ = {
    forward, bindAlias, handleGroup, callCreateQmlObject, files, scripts, script, si, sis, moduleScripts, make: engine.make, inContext: engine.inContext, callCreateComponent, sctx, sctxValue, sset, singleton, defineMethod, defaultParent, ctx, ctxValue, mctx, ns, qmlModule, subAssign, rootContext,
    engine, defineType, create,
    batch: engine.batch,
    stats: engine.stats,
    Qt,
    // QML types by name
    types: {
        QtObject, Component, Timer, Connections, Binding, ListModel, Instantiator,
        Item, Anchors, FocusScope, Keys, KeyNavigation, Rectangle, Border, Gradient, GradientStop, Text, Font, TextInput, TextEdit, Image, Canvas,
        MouseArea, MouseDrag, DropArea, Flickable, Column, Row, Flow, Repeater, Loader, ListView, GridView, Shortcut, StandardKey,
        IntValidator, DoubleValidator, RegularExpressionValidator, RegExpValidator, FontLoader,
        State, PropertyChanges, StateChangeScript, Transition, StateGroup,
        Animation, PauseAnimation, PropertyAnimation, NumberAnimation, ColorAnimation, RotationAnimation, SmoothedAnimation,
        XAnimator, YAnimator, ScaleAnimator, RotationAnimator, OpacityAnimator, PropertyAction, ScriptAction,
        SequentialAnimation, ParallelAnimation, Behavior, Easing, EasingGroup,
        LayoutAttached, RowLayout, ColumnLayout, GridLayout, StackLayout, LayoutItemProxy,
        Window, FileDialog, FolderDialog, Settings, WebSocket: QmlWebSocket, DropShadow, TestCase, SignalSpy,
        BaseClass, BaseModel, TreeItemModel, GqlModel, GqlRequest, FileIO, Process, RemoteFileController, NavigationHistory,
    },
    mouse: require('./QtQuick/PointerDelivery'),
    QtObject, GroupObject, Item, Anchors, Rectangle, Text, MouseArea, Column, Row, Component, Timer, FocusScope,
}

if(typeof globalThis !== 'undefined'){
    // the web context translates (and makes qsTr bindings follow the language)
    globalThis.qsTr = (text) => rootContext.context ? rootContext.context.translate(String(text)) : String(text)
    globalThis.qsTranslate = (context, text) => globalThis.qsTr(text)
    if(!globalThis.qsTrId) globalThis.qsTrId = (id) => String(id)
    if(!globalThis.QT_TR_NOOP) globalThis.QT_TR_NOOP = (text) => text
}

require('./Qt/Locale').install()

if(!String.prototype.arg){
    // QString::arg: replaces the lowest-numbered %N
    Object.defineProperty(String.prototype, 'arg', {
        value: function(...args){
            let result = String(this)
            for(let value of args){
                let match = result.match(/%(\d+)/g)
                if(!match) break
                let lowest = match.map(m => parseInt(m.slice(1))).sort((a, b) => a - b)[0]
                result = result.split('%' + lowest).join(value === undefined || value === null ? '' : String(value))
            }
            return result
        },
        configurable: true,
        writable: true,
    })
}

Qt.$types = JQ.types
Qt.$qmlModules = modules
Qt.$qmlScripts = scripts

JQ.rootPath = ''

setRootProvider(() => JQ.rootPath)

if(typeof window !== 'undefined') window.JQ = JQ

module.exports = JQ
