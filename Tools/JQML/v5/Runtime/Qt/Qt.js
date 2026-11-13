const engine = require('../Core/engine')
const { QtBinding } = require('../Core/QtObject')
const { QPointF, QRectF, QSizeF } = require('../Core/ValueTypes')
const { Component } = require('../QtQml/Component')
const { findFile } = require('../QtQml/files')

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
    createComponent(url){
        let name = String(url).split(/[\\/]/).pop().replace(/\?.*$/, '')
        let Class = findFile(url, this.$base) || Qt.$types[name.replace(/\.qml$/, '')]
        // "Returns a Component object created using the QML file at the specified url"; a missing
        // file gives a component in the Error state
        let component = new Component(null, Class ? (parent) => new Class(parent) : null)
        component.$v[Component.URL] = String(url)
        if(!Class){
            component.$v[Component.STATUS] = Component.Error
            component.$errorString = String(url) + ': No such file or directory'
        }
        return component
    },
})

module.exports = { Qt }
