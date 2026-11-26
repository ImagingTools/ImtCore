const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- Settings (QtCore / Qt.labs.settings) ---------------------------------------------------------

// "The value of each property is read when the component is completed and written on change"
// (QQmlSettings: load in componentComplete, store after changes); localStorage per category.
function storage(){
    try { return typeof localStorage !== 'undefined' ? localStorage : null } catch(error) { return null }
}

class Settings extends QtObject {
    constructor(parent = null){
        super(parent)
        engine.scheduleComponentComplete(this)
    }

    // the keys of the current web runtime (Qt/labs/settings/QSettings.js): "<category or General>/<key>"
    $key(name){
        return (this.$v[Settings.CATEGORY] || 'General') + '/' + name
    }

    $componentComplete(){
        super.$componentComplete()
        let store = storage()
        let own = this.constructor.$type.props.slice(Settings.$type.props.length)
        for(let prop of own){
            if(prop.kind === 'group') continue
            if(store){
                try {
                    let saved = store.getItem(this.$key(prop.name))
                    if(saved !== null) this.$write(prop.id, JSON.parse(saved))
                } catch(error) {}
            }
            engine.connect(this, prop.signal, () => this.setValue(prop.name, this.$v[prop.id]))
        }
    }

    value(key, defaultValue){
        let store = storage()
        try {
            let saved = store ? store.getItem(this.$key(key)) : null
            if(saved === null || saved === undefined) return defaultValue
            try { return JSON.parse(saved) } catch(error) { return saved }
        } catch(error) {
            return defaultValue
        }
    }

    remove(key){
        let store = storage()
        try { if(store) store.removeItem(this.$key(key)) } catch(error) {}
    }

    clear(){
        let store = storage()
        if(!store) return
        try {
            let prefix = this.$key('')
            let keys = []
            for(let i = 0; i < store.length; i++){
                let key = store.key(i)
                if(key && key.startsWith(prefix)) keys.push(key)
            }
            for(let key of keys) store.removeItem(key)
        } catch(error) {}
    }

    setValue(key, value){
        let store = storage()
        try {
            if(store) store.setItem(this.$key(key), JSON.stringify(value === undefined ? null : value))
        } catch(error) {}
    }

    sync(){}
}
defineType(Settings, { props: { category: 'string', fileName: 'string', location: 'string' } })

module.exports = { Settings }
