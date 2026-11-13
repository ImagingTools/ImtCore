// Qt.locale() and the QML Date extensions toLocaleDateString/TimeString/String(locale, format)

class Locale {
    constructor(name){
        this.name = name
        this.$tag = name.replace('_', '-')
        let parts = new Intl.NumberFormat(this.$tag).formatToParts(12345.6)
        let part = (type, fallback) => (parts.find(p => p.type === type) || { value: fallback }).value
        this.decimalPoint = part('decimal', '.')
        this.groupSeparator = part('group', ',')
    }

    $name(date, options){
        return new Intl.DateTimeFormat(this.$tag, options).format(date)
    }
}
Object.assign(Locale, { LongFormat: 0, ShortFormat: 1, NarrowFormat: 2 })

function defaultName(){
    let tag = typeof navigator !== 'undefined' && navigator.language ? navigator.language : 'en-US'
    return tag.replace('-', '_')
}

function locale(name){
    return new Locale(name ? String(name) : defaultName())
}

const pad = (value, size) => String(value).padStart(size, '0')

// QDateTime::toString format: letter runs are fields, 'text' is literal, '' is a quote
function formatQt(date, locale, format){
    let twelve = /a|A/.test(format.replace(/'[^']*'/g, ''))
    let out = ''
    for(let i = 0; i < format.length;){
        let c = format[i]
        if(c === "'"){
            let end = format.indexOf("'", i + 1)
            if(end === i + 1){ out += "'"; i += 2; continue }
            if(end < 0) end = format.length
            out += format.slice(i + 1, end)
            i = end + 1
            continue
        }
        let run = 1
        while(format[i + run] === c) run++
        let take = (max) => Math.min(run, max)
        let n
        switch(c){
        case 'd':
            n = take(4)
            out += n === 1 ? date.getDate() : n === 2 ? pad(date.getDate(), 2) : locale.$name(date, { weekday: n === 3 ? 'short' : 'long' })
            break
        case 'M':
            n = take(4)
            out += n === 1 ? date.getMonth() + 1 : n === 2 ? pad(date.getMonth() + 1, 2) : locale.$name(date, { month: n === 3 ? 'short' : 'long' })
            break
        case 'y':
            n = run >= 4 ? 4 : run >= 2 ? 2 : 1
            out += n === 4 ? pad(date.getFullYear(), 4) : n === 2 ? pad(date.getFullYear() % 100, 2) : 'y'
            break
        case 'h':
        case 'H': {
            n = take(2)
            let hours = date.getHours()
            if(c === 'h' && twelve) hours = hours % 12 || 12
            out += n === 2 ? pad(hours, 2) : hours
            break
        }
        case 'm':
            n = take(2)
            out += n === 2 ? pad(date.getMinutes(), 2) : date.getMinutes()
            break
        case 's':
            n = take(2)
            out += n === 2 ? pad(date.getSeconds(), 2) : date.getSeconds()
            break
        case 'z':
            n = run >= 3 ? 3 : 1
            out += n === 3 ? pad(date.getMilliseconds(), 3) : String(date.getMilliseconds() / 1000).slice(2) || '0'
            break
        case 'A':
        case 'a': {
            n = format[i + 1] === (c === 'A' ? 'P' : 'p') ? 2 : 1
            let text = date.getHours() < 12 ? 'AM' : 'PM'
            out += c === 'a' ? text.toLowerCase() : text
            break
        }
        case 't':
            n = 1
            out += locale.$name(date, { timeZoneName: 'short' }).split(' ').pop()
            break
        default:
            n = run
            out += format.slice(i, i + run)
        }
        i += n
    }
    return out
}

const STYLES = ['long', 'short', 'short']

function install(){
    const native = {
        toLocaleDateString: Date.prototype.toLocaleDateString,
        toLocaleTimeString: Date.prototype.toLocaleTimeString,
        toLocaleString: Date.prototype.toLocaleString,
    }
    const styleKey = { toLocaleDateString: 'dateStyle', toLocaleTimeString: 'timeStyle', toLocaleString: null }
    for(let method in native){
        Object.defineProperty(Date.prototype, method, {
            value: function(locale, format){
                if(!(locale instanceof Locale)) return native[method].apply(this, arguments)
                if(typeof format === 'string') return formatQt(this, locale, format)
                let style = STYLES[format === undefined ? 0 : format] || 'long'
                let key = styleKey[method]
                let options = key ? { [key]: style } : { dateStyle: style, timeStyle: style }
                return native[method].call(this, locale.$tag, options)
            },
            configurable: true,
            writable: true,
        })
    }
}

const DATE_FORMATS = { TextDate: 0, ISODate: 1, RFC2822Date: 8, ISODateWithMs: 9 }
const ISO = {
    toLocaleString: ['yyyy-MM-ddTHH:mm:ss', 'yyyy-MM-ddTHH:mm:ss.zzz'],
    toLocaleDateString: ['yyyy-MM-dd', 'yyyy-MM-dd'],
    toLocaleTimeString: ['HH:mm:ss', 'HH:mm:ss.zzz'],
}

// Qt.formatDateTime/formatDate/formatTime: a format string, a Qt::DateFormat, a locale object, or the default locale's short format
function formatValue(method, value, format, option){
    let date = value instanceof Date ? value : new Date(value)
    if(isNaN(date)) return ''
    if(format === DATE_FORMATS.ISODate || format === DATE_FORMATS.ISODateWithMs) return formatQt(date, locale(), ISO[method][format === DATE_FORMATS.ISODate ? 0 : 1])
    if(format instanceof Locale) return date[method](format, option === undefined ? Locale.ShortFormat : option)
    return date[method](locale(), typeof format === 'string' ? format : Locale.ShortFormat)
}

module.exports = { Locale, locale, install, formatValue, DATE_FORMATS }
