const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

class Internal extends QtObject {
    constructor(parent = null){
        super(parent)
        this.__base = null
    }

    startTransaction(){
        if(this.isTransaction){
            console.error('Unable to start transaction. Error: transaction already started.')
            return
        }
        this.changeList = []
        this.isTransaction = true
    }

    stopTransaction(){
        if(!this.isTransaction){
            console.error('Unable to stop transaction. Error: there is no active transaction.')
            return
        }
        if(this.countChanges > 0){
            this.__base.modelChanged(this.changeList)
            this.countChanges = 0
        }
        this.isTransaction = false
        this.changeList = null
    }

    removeAt(key){
        let index = this.removed ? this.removed.indexOf(key) : -1
        if(index > -1){
            this.removed.splice(index, 1)
            if(this.removed.length === 0) this.removed = null
        }
    }

    containceInRemoved(key){
        return !!this.removed && this.removed.indexOf(key) > -1
    }
}

defineType(Internal, {
    props: {
        isTransaction: 'bool',
        countChanges: 'int',
        changeList: { kind: 'var', initial: null },
        removed: { kind: 'var', initial: null },
        __typename: 'string',
    },
    signals: ['internalModelChanged(name,sender)'],
    handlers: {
        internalModelChanged(internal, name, sender){
            if(internal.isTransaction){
                internal.changeList.push({ name, sender })
                internal.countChanges++
                return
            }
            internal.__base.modelChanged([{ name, sender }])
        },
    },
})

class BaseClass extends QtObject {
    constructor(parent = null){
        super(parent)
        let internal = new Internal(null)
        internal.__base = this
        this.$v[BaseClass._INTERNAL] = internal
    }

    // Plain JSON stored by fromObject becomes objects on first read, like the Proxy getter of BaseClass.js
    $materialize(id, key, pureData){
        if(Array.isArray(pureData)){
            let component = this.createComponent(key)
            if(!component) return pureData
            let model = component.createObject(this)
            for(let data of pureData){
                let obj = this.createElement(key, data && data.__typename).createObject(this)
                obj.fromObject(data)
                model.append({ item: obj })
                obj.owner = this
            }
            this.$v[id] = model
            model.owner = this
            model.finished()
            return model
        }
        let obj = this.createComponent(key, pureData.__typename).createObject(this)
        obj.fromObject(pureData)
        this.$v[id] = obj
        obj.owner = this
        return obj
    }

    beginChanges(){ this._internal.startTransaction() }
    endChanges(){ this._internal.stopTransaction() }

    removeKey(key){
        if(this.getProperties().has(key)){
            // as imtcontrols/Base/BaseClass.qml: only object values are dropped, scalars stay and are marked removed
            let value = this[key]
            if(value !== null && typeof value === 'object'){
                if(value.destroy) value.destroy()
                this[key] = null
            }
        }
        if(!this._internal.removed) this._internal.removed = []
        if(this._internal.removed.indexOf(key) === -1) this._internal.removed.push(key)
    }

    connectProperties(){}
    createMe(){ return new this.constructor(null) }
    refresh(){}

    isEqualWithModel(model){
        if(typeof this !== typeof model) return false
        let selfKeys = this.getProperties()
        let sourceKeys = model.getProperties()
        if(selfKeys.size !== sourceKeys.size) return false
        for(let key of selfKeys){
            if(!sourceKeys.has(key)) return false
            if(typeof this[key] !== typeof model[key]) return false
            if(typeof this[key] === 'object'){
                if(this[key] && this[key].isEqualWithModel){
                    if(!this[key].isEqualWithModel(model[key])) return false
                } else if(this[key] !== model[key]){
                    return false
                }
            } else if(this[key] !== model[key]){
                return false
            }
        }
        return true
    }

    copy(item){ return this.copyFrom(item) }

    copyMe(){
        let obj = this.createMe()
        obj.copyFrom(this)
        return obj
    }

    copyFrom(item){
        this.fromJSON(item.toJson())
        return true
    }

    copyTo(item){
        item.copyFrom(this)
        return true
    }

    createComponent(propertyId, typename){}
    createElement(propertyId, typename){}
    getJSONKeyForProperty(propertyId){ return propertyId }
    getProperties(){ return this.constructor.cachedPoperties }
    isArrayRequired(propertyId){ return false }
    areArrayElementsRequired(propertyId){ return false }

    isArrayValueValid(propertyId, value){
        if(value === null || value === undefined) return !this.isArrayRequired(propertyId)
        if(!this.areArrayElementsRequired(propertyId)) return true
        if(Array.isArray(value)){
            for(let element of value){
                if(element === null || element === undefined) return false
            }
        } else if(value.hasNullElements && value.hasNullElements()){
            return false
        }
        return true
    }

    createFromJson(json){ return this.fromJSON(json) }

    // Returns '' if a required array is null or has null elements; scalars are not validated
    toJson(){
        let json = '{'
        let isFirst = true
        for(let key of this.getProperties()){
            let value = this[key]
            if(key === '__typename' && value === '') continue
            if(value == null){
                if(!this.isArrayValueValid(key, value)) return ''
                if(this._internal.containceInRemoved(key)) continue
            }
            if(!isFirst) json += ','
            isFirst = false
            if(typeof value === 'object'){
                if(Array.isArray(value)){
                    if(!this.isArrayValueValid(key, value)) return ''
                    json += '"' + this.getJSONKeyForProperty(key) + '":['
                    for(let j = 0; j < value.length; j++){
                        if(j !== 0) json += ', '
                        json += typeof value[j] === 'string' ? JSON.stringify(value[j]) : value[j]
                    }
                    json += ']'
                } else if(value !== null){
                    if(!this.isArrayValueValid(key, value)) return ''
                    let serialized = value.toJson()
                    if(serialized === '') return ''
                    json += '"' + this.getJSONKeyForProperty(key) + '":' + serialized
                } else {
                    json += '"' + this.getJSONKeyForProperty(key) + '": null'
                }
            } else if(value === undefined){
                json += '"' + this.getJSONKeyForProperty(key) + '":null'
            } else if(typeof value === 'string'){
                json += '"' + this.getJSONKeyForProperty(key) + '":"' + this.escapeSpecialChars(value) + '"'
            } else {
                json += '"' + this.getJSONKeyForProperty(key) + '":' + value
            }
        }
        return json + '}'
    }

    // Returns '' if a required array is null or has null elements; scalars are not validated
    toGraphQL(){
        let graphQL = '{'
        let isFirst = true
        for(let key of this.getProperties()){
            let value = this[key]
            if(value == null){
                if(!this.isArrayValueValid(key, value)) return ''
                if(this._internal.containceInRemoved(key)) continue
            }
            if(!isFirst) graphQL += ','
            isFirst = false
            if(typeof value === 'object'){
                if(Array.isArray(value)){
                    if(!this.isArrayValueValid(key, value)) return ''
                    graphQL += this.getJSONKeyForProperty(key) + ':['
                    for(let j = 0; j < value.length; j++){
                        if(j !== 0) graphQL += ', '
                        graphQL += typeof value[j] === 'string' ? '"' + this.escapeSpecialChars(value[j]) + '"' : value[j]
                    }
                    graphQL += ']'
                } else if(value !== null){
                    if(!this.isArrayValueValid(key, value)) return ''
                    let serialized = value.toGraphQL()
                    if(serialized === '') return ''
                    graphQL += this.getJSONKeyForProperty(key) + ':' + serialized
                } else {
                    graphQL += this.getJSONKeyForProperty(key) + ':null'
                }
            } else {
                graphQL += this.getJSONKeyForProperty(key) + ':'
                if(value === undefined) graphQL += 'null'
                else if(typeof value === 'string') graphQL += '"' + this.escapeSpecialChars(value) + '"'
                else graphQL += value
            }
        }
        return graphQL + '}'
    }

    fromJSON(json){
        let obj
        try {
            obj = JSON.parse(json.replace(/\n/g, '\\n').replace(/\r/g, '\\r').replace(/\t/g, '\\t').replace(/\f/g, '\\f'))
        } catch(error) {
            console.error(error)
            return false
        }
        return this.fromObject(obj)
    }

    escapeSpecialChars(text){
        return text.replace(/\\/g, '\\\\').replace(/"/g, '\\"').replace(/\n/g, '\\n')
            .replace(/\r/g, '\\r').replace(/\t/g, '\\t').replace(/\f/g, '\\f')
    }

    fromObject(sourceObject){
        for(let key of this.getProperties()){
            if(!(this.getJSONKeyForProperty(key) in sourceObject)){
                let value = this.$rawValue(key)
                if(value && typeof value === 'object' && value instanceof QtObject){
                    if(value.clear) value.clear()
                    if(value.destroy) value.destroy()
                }
                if(value && typeof value === 'object') this[key] = null
            }
        }
        for(let key in sourceObject){
            let property = 'm_' + key[0].toLowerCase() + key.slice(1)
            this._internal.removeAt(property)
            if(sourceObject[key] === null) this.$unset(property)
            else this[property] = sourceObject[key]
        }
        this.finished()
        return true
    }

    // A null field is not set (an optional field of CItemModelBase); hasX() reads it as null even on an int property
    $unset(name){
        let prop = this.constructor.$type.propByName.get(name)
        if(!prop || prop.kind === 'var'){
            this[name] = null
            return
        }
        let binding = this.$takeBinding(prop.id)
        if(binding) engine.disposeBinding(binding)
        let old = this.$v[prop.id]
        if(old === null) return
        this.$v[prop.id] = null
        engine.emit(this, prop.signal, old, null)
    }

    $rawValue(key){
        let prop = this.constructor.$type.propByName.get(key)
        return prop ? this.$v[prop.id] : undefined
    }

    destroy(){
        if(this._internal) this._internal.destroy()
        super.destroy()
    }
}

defineType(BaseClass, {
    props: {
        enableNotifications: { kind: 'bool', initial: true },
        propertiesIsConnected: 'bool',
        owner: { kind: 'var', initial: null },
        _internal: { kind: 'var', initial: null },
    },
    signals: ['modelChanged(changeSet)', 'finished'],
    handlers: {
        modelChanged(obj, changeSet){
            let owner = obj.owner
            if(owner && owner.enableNotifications && owner.modelChanged){
                if(owner._internal.isTransaction){
                    owner._internal.changeList = owner._internal.changeList.concat(changeSet)
                    owner._internal.countChanges++
                } else {
                    owner.modelChanged(changeSet)
                }
            }
        },
    },
})
BaseClass.cachedPoperties = new Set(['__typename'])

// Every type derived from BaseClass: its m_ properties are the serialized ones (cachedPoperties),
// read lazily from JSON, and a change reports internalModelChanged(name, sender)
BaseClass.$defined = function(Class, type, ownProps){
    let names = ownProps.filter(name => name.slice(0, 2) === 'm_' || name === '__typename')
    let parentSet = Object.getPrototypeOf(Class).cachedPoperties || BaseClass.cachedPoperties
    Class.cachedPoperties = new Set([...parentSet, ...names])
    for(let name of names){
        if(name === '__typename') continue
        let prop = type.propByName.get(name)
        let id = prop.id, signal = prop.signal
        Object.defineProperty(Class.prototype, name, {
            get(){
                engine.track(this, signal)
                let value = this.$v[id]
                if(value !== null && typeof value === 'object' && !(value instanceof QtObject)) value = this.$materialize(id, name, value)
                return value
            },
            set(value){ this.$assign(id, value) },
            configurable: true,
        })
        let inherited = type.handlers[signal]
        type.handlers[signal] = (obj, a, b) => {
            if(inherited) inherited(obj, a, b)
            if(obj.enableNotifications) obj._internal.internalModelChanged(name, obj)
        }
    }
}

module.exports = { BaseClass }
