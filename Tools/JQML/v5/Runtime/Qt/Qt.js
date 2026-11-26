const engine = require('../Core/engine')
const { QtBinding } = require('../Core/QtObject')
const { isDefined } = require('../Core/type')
const { QPointF, QRectF, QSizeF } = require('../Core/ValueTypes')
const { Component } = require('../QtQml/Component')
const { files, findFile, load, resolveExact } = require('../QtQml/files')
const { qtModule, moduleTypeKey } = require('./modules')
const color = require('../Core/color')
const ValueTypes = require('../Core/ValueTypes')
const { QtObject: QtObjectType } = require('../Core/QtObject')

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
    // colors (QtObject::rgba / hsla / hsva / color / colorEqual / lighter / darker / alpha / tint)
    rgba: (r, g, b, a = 1) => color.rgbaColor(r, g, b, a),
    hsla: (h, s, l, a = 1) => color.hslaColor(h, s, l, a),
    hsva: (h, s, v, a = 1) => color.hsvaColor(h, s, v, a),
    color(name){
        let parsed = color.parseColor(String(name))
        if(!parsed) throw new Error('"' + name + '" is not a valid color name')
        return color.colorName(parsed)
    },
    colorEqual(lhs, rhs){
        let parse = (value) => {
            if(typeof value !== 'string') throw new Error('Qt.colorEqual(): Invalid arguments')
            let parsed = color.parseColor(value)
            if(!parsed) throw new Error('Qt.colorEqual(): Invalid color name')
            return parsed
        }
        let a = parse(lhs), b = parse(rhs)
        return a.a === b.a && a.r === b.r && a.g === b.g && a.b === b.b
    },
    // a value that names no color gives null (colorVariantFromJSValue)
    lighter: (base, factor = 1.5) => (isColor(base) ? color.lighterColor(base, factor) : null),
    darker: (base, factor = 2.0) => (isColor(base) ? color.darkerColor(base, factor) : null),
    alpha: (base, value) => (isColor(base) ? color.alphaColor(base, value) : null),
    tint: (base, tintColor) => (isColor(base) && isColor(tintColor) ? color.tintColor(base, tintColor) : null),
    // value types
    vector2d: (x, y) => new ValueTypes.QVector2D(x, y),
    vector3d: (x, y, z) => new ValueTypes.QVector3D(x, y, z),
    vector4d: (x, y, z, w) => new ValueTypes.QVector4D(x, y, z, w),
    quaternion: (scalar, x, y, z) => new ValueTypes.QQuaternion(scalar, x, y, z),
    // QtObject::matrix4x4: the identity, the 16 values row by row, or an array of them
    matrix4x4(...args){
        if(args.length === 0) return new ValueTypes.QMatrix4x4()
        let values = args.length === 1 && Array.isArray(args[0]) ? args[0] : args
        if(values.length !== 16) throw new Error('Qt.matrix4x4(): Invalid arguments')
        return new ValueTypes.QMatrix4x4(values.map(Number))
    },
    font(specifier){
        if(!specifier || typeof specifier !== 'object') throw new Error('Qt.font(): Invalid arguments')
        return ValueTypes.makeFont(specifier)
    },
    // a url of this runtime is its string (QUrl::toString)
    url: (url) => (url === undefined || url === null ? '' : String(url)),
    // QString::toHtmlEscaped
    escapeHtml: (text) => String(text).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;'),
    // QtObject::isQtObject: a QObject (the Qt object is one)
    isQtObject(value){
        return !!value && typeof value === 'object' && (value instanceof QtObjectType || value.$isQt === true)
    },
    // the fonts the document has (QFontDatabase::families: the browser lists only those it loaded)
    fontFamilies(){
        let families = new Set()
        if(typeof document !== 'undefined' && document.fonts && typeof document.fonts.forEach === 'function'){
            document.fonts.forEach((face) => families.add(String(face.family).replace(/^["']|["']$/g, '')))
        }
        for(let generic of ['sans-serif', 'serif', 'monospace']) families.add(generic)
        return Array.from(families).sort()
    },
    // QtObject::enumStringToValue / enumValueToString / enumValueToStrings of an enum given as its object of names
    enumStringToValue(enumType, name){
        let values = enumObject(enumType, 'enumStringToValue')
        if(!Object.prototype.hasOwnProperty.call(values, name)) throw new Error('Qt.enumStringToValue(): Invalid enum key')
        return values[name]
    },
    enumValueToString(enumType, value){
        let values = enumObject(enumType, 'enumValueToString')
        for(let key of Object.keys(values)) if(values[key] === value) return key
        throw new Error('Qt.enumValueToString(): Invalid enum value')
    },
    enumValueToStrings(enumType, value){
        let values = enumObject(enumType, 'enumValueToStrings')
        return Object.keys(values).filter(key => values[key] === value)
    },
    // QCoreApplication::exit: the page ends as with quit(); the code is kept for a test runner
    exit(code = 0){
        if(typeof window !== 'undefined') window.__jqmlExitCode = code
        Qt.quit()
    },
    // Qt.include (deprecated in Qt 6): the bundle has no loader of loose scripts; the callback hears NETWORK_ERROR
    include(url, callback){
        let result = { status: 2, OK: 0, LOADING: 1, NETWORK_ERROR: 2, EXCEPTION: 3, exception: 'Qt.include() is not supported: import the script instead' }
        console.warn('Qt.include(' + url + '): ' + result.exception)
        if(typeof callback === 'function') callback(result)
        return result
    },
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
    // QtObject::md5: the hex MD5 digest of the UTF-8 bytes
    md5: require('./md5').md5,
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
        // its own types and those of the modules it imports (QtQuick.Controls: the controls of QtQuick.Controls.Basic)
        let key = name === 'ListElement' ? 'QtObject' : moduleTypeKey(module, name)
        let Class = key ? Qt.$types[key] : null
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

// QtObject::colorVariantFromJSValue: a string that names a color, or null
const isColor = (value) => typeof value === 'string' && color.parseColor(value) !== null

function enumObject(enumType, name){
    if(!enumType || typeof enumType !== 'object') throw new Error('Qt.' + name + '(): Invalid enum type')
    return enumType
}

require('./QtGlobals').install(Qt)

module.exports = { Qt }
