const QtObject = require("../../../QtQml/QtObject")
const StringProp = require("../../../QtQml/String")
const Signal = require("../../../QtQml/Signal")

/**
 * Settings — stores key/value pairs in localStorage.
 * Keys are namespaced by `category` (defaults to "General").
 */
class Settings extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        category: { type: StringProp, value: '' },
        fileName: { type: StringProp, value: '' },

        categoryChanged: { type: Signal, args: [] },
        fileNameChanged: { type: Signal, args: [] },
    })

    __prefix() {
        let cat = this.category || 'General'
        return cat + '/'
    }

    value(key, defaultValue) {
        try {
            let raw = localStorage.getItem(this.__prefix() + key)
            if (raw === null || raw === undefined) return defaultValue !== undefined ? defaultValue : undefined
            try { return JSON.parse(raw) } catch(e) { return raw }
        } catch(e) { return defaultValue }
    }

    setValue(key, val) {
        try {
            localStorage.setItem(this.__prefix() + key, JSON.stringify(val))
        } catch(e) { /* quota / private mode */ }
    }

    remove(key) {
        try { localStorage.removeItem(this.__prefix() + key) } catch(e) {}
    }

    clear() {
        try {
            let prefix = this.__prefix()
            let toRemove = []
            for (let i = 0; i < localStorage.length; i++) {
                let k = localStorage.key(i)
                if (k && k.startsWith(prefix)) toRemove.push(k)
            }
            for (let k of toRemove) localStorage.removeItem(k)
        } catch(e) {}
    }

    sync() { /* localStorage is synchronous — no-op */ }
}

module.exports = Settings