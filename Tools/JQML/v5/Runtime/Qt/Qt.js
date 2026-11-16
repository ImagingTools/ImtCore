const engine = require('../Core/engine')
const { QtBinding } = require('../Core/QtObject')
const { isDefined } = require('../Core/type')
const { QPointF, QRectF, QSizeF } = require('../Core/ValueTypes')
const { Component } = require('../QtQml/Component')
const { files, findFile, load, resolveExact } = require('../QtQml/files')
const { qtModule } = require('./modules')

// ---- Qt global ------------------------------------------------------------------

const SAFE_SCHEMES = new Set(['http:', 'https:', 'mailto:', 'tel:', 'ftp:'])
const BASE64 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'

const { formatValue, DATE_FORMATS } = require('./Locale')

// Qt::WindowModality and Qt::Dialog (window type), missing in the shared enum table
const Qt = Object.assign({}, require('./enums'), { NonModal: 0, WindowModal: 1, ApplicationModal: 2, Dialog: 0x3 }, {
    quit(){
        engine.stop()
        if(typeof window !== 'undefined') window.__jqmlQuit = true
    },
    rgba: require('../Core/color').rgba,
    point(x, y){ return new QPointF(x, y) },
    size(width, height){ return new QSizeF(width, height) },
    rect(x, y, width, height){ return new QRectF(x, y, width, height) },
    callLater(fn, ...args){ engine.queueCall(() => fn(...args)) },
    binding(fn){ return new QtBinding(fn) },
    locale: require('./Locale').locale,
    ...DATE_FORMATS,
    formatDateTime(value, format, option){ return formatValue('toLocaleString', value, format, option) },
    formatDate(value, format, option){ return formatValue('toLocaleDateString', value, format, option) },
    formatTime(value, format, option){ return formatValue('toLocaleTimeString', value, format, option) },
    // QtObject::btoa: base64 of the UTF-8 bytes
    btoa(data){
        let bytes = new TextEncoder().encode(String(data))
        let binary = ''
        for(let i = 0; i < bytes.length; i += 0x8000) binary += String.fromCharCode.apply(null, bytes.subarray(i, i + 0x8000))
        return btoa(binary)
    },
    // QtObject::atob: QByteArray::fromBase64 of the Latin-1 chars, read as UTF-8; like Qt every char
    // outside the alphabet, '=' too, is skipped and the bit stream goes on
    atob(data){
        let text = String(data)
        let bytes = new Uint8Array(Math.ceil(text.length * 3 / 4))
        let length = 0
        let buffer = 0
        let bits = 0
        for(let i = 0; i < text.length; i++){
            let digit = BASE64.indexOf(text[i])
            if(digit < 0) continue
            buffer = (buffer << 6) | digit
            bits += 6
            if(bits >= 8){
                bits -= 8
                bytes[length++] = buffer >> bits
                buffer &= (1 << bits) - 1
            }
        }
        return new TextDecoder().decode(bytes.subarray(0, length))
    },
    // QtObject::openUrlExternally: the url resolved against the page; only schemes a browser opens safely
    openUrlExternally(url){
        let target
        try {
            target = new URL(String(url), typeof location === 'undefined' ? undefined : location.href)
        } catch(error){
            return false
        }
        if(!SAFE_SCHEMES.has(target.protocol) || typeof window === 'undefined'){
            console.warn('Qt.openUrlExternally: ' + target.protocol + ' urls are not opened')
            return false
        }
        window.open(target.href, '_blank', 'noopener')
        return true
    },
    // Qt of a file that creates components: relative urls resolve against the file's url
    $base: '',
    $at(url){
        let qt = Object.create(Qt)
        qt.$base = url
        return qt
    },
    // QtObject::resolvedUrl: a relative url against the file of the caller (QQmlContext::resolvedUrl), the bundle being
    // the qrc:/qml/ resources
    resolvedUrl(url){
        if(url === undefined || url === null) return ''
        let text = String(url)
        if(/^[a-z][a-z0-9+.-]*:/i.test(text)) return text
        return resolveExact(text, this.$base || '').url
    },
    // the Qt object and the ones of the files (JQ.callCreateQmlObject tells them from other objects)
    $isQt: true,
    // the calling scope while a document of Qt.createQmlObject is built ($s.$p of its root scope)
    $qmlCaller: null,
    // QtObject::createQmlObject: compiled in the browser (compiler/runtime.js joins the bundle of an application that
    // uses it). A call written in QML comes through JQ.callCreateQmlObject with the calling context; this one is a call
    // from JavaScript without a QML context
    createQmlObject(qml, parent, url){
        return Qt.$createQmlObject(this.$base || '', null, null, qml, parent, url)
    },
    // base: the url of the calling file; scope, ids: the calling scope and the names of its ids (codegen scopeInfo)
    $createQmlObject(base, scope, ids, qml, parent, url){
        let text = qml === undefined || qml === null ? '' : String(qml)
        if(text === '') return null
        let target = url === undefined || url === null || String(url) === '' ? null : resolveExact(url, base)
        if(!parent) throw new Error('Qt.createQmlObject(): Missing parent object')
        let compiler = Qt.$qmlCompiler
        if(!compiler) throw new Error('Qt.createQmlObject(): the QML compiler is not in this bundle')
        let key = target ? target.key : null
        let fileName = target ? target.url : ''
        // without a url the types of the caller's directory are visible, as in Qt
        let documentKey = key !== null ? key : (base ? base.slice(0, base.lastIndexOf('/') + 1) : '')
        let result = compiler.compile(text, fileName, documentKey, { files, modules: Qt.$qmlModules, scripts: Qt.$qmlScripts }, scope, ids)
        if(result.errors){
            let message = 'Qt.createQmlObject(): failed to create object: '
            let qmlErrors = result.errors.map(e => {
                // QQmlError::toString: the line and the column only when they are known
                message += '\n    ' + (fileName || '<Unknown File>') + (e.line > 0 ? ':' + e.line + (e.col > 0 ? ':' + e.col : '') : '') + ': ' + e.message
                return { lineNumber: e.line, columnNumber: e.col > 0 ? e.col : -1, fileName, message: e.message }
            })
            let error = new Error(message)
            error.qmlErrors = qmlErrors
            throw error
        }
        let caller = scope
        let component = new Component(null, (owner) => {
            let previous = Qt.$qmlCaller
            Qt.$qmlCaller = caller
            try {
                return new result.Class(owner)
            } finally {
                Qt.$qmlCaller = previous
            }
        })
        return component.createObject(parent)
    },
    // QtObject::createComponent: (url, mode, parent), (url, parent), and since Qt 6.5 (moduleUri, typeName, mode, parent).
    // The parent only owns the component, it is not its `parent` property.
    createComponent(...args){
        if(typeof args[1] === 'string') return moduleComponent(args[0], args[1], compilationMode(args[2]))
        let url = args[0]
        if(url === undefined || url === null || typeof url === 'object' && typeof url.toString !== 'function'){
            throw new Error('Passing incompatible arguments to C++ functions from JavaScript is not allowed.')
        }
        let mode = args[1] !== null && typeof args[1] === 'object' ? Component.PreferSynchronous : compilationMode(args[1])
        if(String(url) === '') return null
        let { url: resolved, type } = resolveExact(url, this.$base)
        // A call from QML always has its file ($base). Only the native code of the runtime has none (BaseModel.js stands
        // for BaseModel.qml): it finds a file of that name anywhere in the bundle, or a runtime type of that name.
        if(!type && !this.$base){
            let name = String(url).split(/[\\/]/).pop().replace(/[?#].*$/, '')
            type = findFile(url) || Qt.$types[name.replace(/\.qml$/, '')] || null
        }
        if(!type) return failedComponent(resolved, resolved + ':-1 No such file or directory\n')
        let component = new Component(null, (parent) => new (load(type))(parent))
        component.$v[Component.URL] = resolved
        // QQmlComponent::loadUrl(url, Asynchronous): a type the engine has not loaded yet is Loading until the event loop
        if(mode === Component.Asynchronous && type.$lazy && !isDefined(type.name.slice(0, -1))){
            component.$v[Component.STATUS] = Component.Loading
            component.$v[Component.PROGRESS] = 0
            setTimeout(() => engine.batch(() => {
                load(type)
                component.$write(Component.PROGRESS, 1)
                component.$write(Component.STATUS, Component.Ready)
            }), 0)
        }
        return component
    },
})

function compilationMode(mode){
    if(mode === undefined || mode === null) return Component.PreferSynchronous
    if(mode !== Component.PreferSynchronous && mode !== Component.Asynchronous) throw new Error('Invalid compilation mode ' + mode)
    return mode
}

function failedComponent(url, error){
    let component = new Component(null, null)
    component.$v[Component.URL] = url
    component.$v[Component.STATUS] = Component.Error
    component.$errorString = error
    return component
}

// QQmlComponent::loadFromModule: a Qt module, or a module of the application (qmldir)
function moduleComponent(moduleUri, typeName, mode){
    let module = String(moduleUri === undefined || moduleUri === null ? '' : moduleUri)
    let name = String(typeName)
    if(!module || !name) return null
    let qt = qtModule(module)
    if(qt){
        if(qt.uncreatable.has(name)) return failedComponent('', ':-1 Could not load ' + name + ', as the type is uncreatable\n')
        let Class = qt.types.has(name) ? Qt.$types[name === 'ListElement' ? 'QtObject' : name] : null
        if(!Class) return failedComponent('', ':-1 Module "' + module + '" contains no type named "' + name + '"\n')
        let component = new Component(null, (parent) => new Class(parent))
        component.$v[Component.URL] = ''
        return component
    }
    let types = Qt.$qmlModules[module]
    if(!types) return failedComponent('', ':-1 No module named "' + module + '" found\n')
    let type = Object.prototype.hasOwnProperty.call(types, name) ? types[name] : null
    if(!type) return failedComponent('', ':-1 Module "' + module + '" contains no type named "' + name + '"\n')
    let component = new Component(null, (parent) => new (load(type))(parent))
    // a type of a qmldir module is a file: the component has its url
    let key = Object.keys(files).find(path => files[path] === type)
    component.$v[Component.URL] = key ? 'qrc:/qml/' + key : ''
    return component
}

module.exports = { Qt }
