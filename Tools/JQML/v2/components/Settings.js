const { QtObject } = require('./QtObject')
const { QString } = require('../utils/properties')

class Settings extends QtObject {

    static defaultProperties = {
        category: { type: QString, value: '', changed: '$categoryChanged' },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
    }

    $getKey(key) {
        let category = this.getPropertyValue('category')
        return category ? category + '/' + key : key
    }

    value(key, defaultValue) {
        let stored = localStorage.getItem(this.$getKey(key))
        if (stored === null) return defaultValue !== undefined ? defaultValue : null
        try {
            return JSON.parse(stored)
        } catch (e) {
            return stored
        }
    }

    setValue(key, value) {
        localStorage.setItem(this.$getKey(key), JSON.stringify(value))
    }

    remove(key) {
        localStorage.removeItem(this.$getKey(key))
    }

    clear() {
        let category = this.getPropertyValue('category')
        if (!category) {
            localStorage.clear()
            return
        }
        let prefix = category + '/'
        let keysToRemove = []
        for (let i = 0; i < localStorage.length; i++) {
            let k = localStorage.key(i)
            if (k.startsWith(prefix)) {
                keysToRemove.push(k)
            }
        }
        for (let k of keysToRemove) {
            localStorage.removeItem(k)
        }
    }

    $categoryChanged() {}
}

module.exports.Settings = Settings
