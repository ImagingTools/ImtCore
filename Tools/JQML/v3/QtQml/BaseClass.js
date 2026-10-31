const QtObject = require("./QtObject")
const Property = require("./Property")
const Bool = require("./Bool")
const Int = require("./Int")
const Var = require("./Var")
const String = require("./String")
const Signal = require("./Signal")
const BaseModel = require("./BaseModel")

const specialCharsPattern = /[\\"\n\r\t\f]/

// The list elements found last by BaseClass.findChild, one per owner: edits of many elements of a list go in order,
// so the next one is found next to it, on every level of a tree
const lastElements = []
const lastElementsLimit = 8

// The owners are held weakly: a closed document must not stay in memory because it was edited last
function findLastElement(owner){
	for (let element of lastElements){
		if (element.ownerRef.deref() === owner){
			return element
		}
	}

	return null
}

function rememberElement(owner, name, index){
	let element = findLastElement(owner)
	if (element === null){
		if (lastElements.length >= lastElementsLimit){
			lastElements.shift()
		}
		element = {"ownerRef": new WeakRef(owner), "name": name, "index": index}
		lastElements.push(element)
	}

	element.name = name
	element.index = index
}

// Element components of the lists: createElement() creates a new one each time.
// By the class of the object (a name may be mangled in a minified bundle), then by list and element type.
const elementComponents = new Map()

function getElementComponent(object, propertyId, typename){
	let typeComponents = elementComponents.get(object.__self.constructor)
	if (typeComponents === undefined){
		typeComponents = new Map()
		elementComponents.set(object.__self.constructor, typeComponents)
	}

	let key = propertyId + "/" + (typename ? typename : "")
	let component = typeComponents.get(key)
	if (component === undefined){
		component = object.createElement(propertyId, typename)
		typeComponents.set(key, component)
	}

	return component
}

class Internal extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
		// isTransaction, countChanges, changeList and removed are plain fields: nothing binds to them,
		// and a property notifies each change, which made every fromObject() transaction slow on the web
		__typename: { type: String, value: '' },

		__typenameChanged: {type:Signal, args:[]},

		internalModelChanged: { type:Signal, args: ['name', 'sender'] },
    })


	SLOT_internalModelChanged(name, sender){
		this.__base.reportChanges([this.__base.createChange([this.__base.getPathKey(name)], "set", name, sender)])
	}

	startTransaction(){
		if (this.isTransaction){
			console.error("Unable to start transaction. Error: transaction already started.")

			return
		}

		this.changeList = []
		this.isTransaction = true
	}

	stopTransaction(){
		if (!this.isTransaction){
			console.error("Unable to stop transaction. Error: there is no active transaction.")

			return
		}

		if (this.countChanges > 0){
			this.__base.modelChanged(this.changeList)

			this.countChanges = 0
		}

		this.isTransaction = false
		delete this.changeList
	}

	removeAt(key){
	// get index if value found otherwise -1
		let index = this.removed ? this.removed.indexOf(key) : -1
		if (index > -1) { //if found
			this.removed.splice(index, 1)
			if(this.removed.length === 0) delete this.removed
		}
	}

	containceInRemoved(key){
		let index = this.removed ? this.removed.indexOf(key) : -1
		if (index > -1) {
			return true
		}

		return false
	}
}

class BaseClass extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        enableNotifications: { type: Bool, value: true },
		propertiesIsConnected: { type: Bool, value: false },
		owner: { type: Var, value: null },
		_internal: { type: Var, value: null },

		enableNotificationsChanged: {type:Signal, args:[]},
		propertiesIsConnectedChanged: {type:Signal, args:[]},
		ownerChanged: {type:Signal, args:[]},
		_internalChanged: {type:Signal, args:[]},

		modelChanged: { type:Signal, args: ['changeSet'] },
		finished: { type:Signal, args: [] },
    })

	static handle = {
        get(target, key){
			if(key !== '__typename' && target.constructor.cachedPoperties.has(key)){
				let node = target.constructor.meta[key]
				if(target.__destroying || target.__destroyed) return node.type.get(target, key, node)
				if(typeof target[key] === "object" && !(target[key] instanceof QtObject)){
					let pureData = target[key]

					if(pureData === null){
						target[key] = null
					} else if(typeof pureData === "object") {
						if(Array.isArray(pureData)){
							let component = target.__proxy.createComponent(key)
			
							if (component) {
								// Owner is attached only after filling, so a lazy read does not report a model change
								let model = node.typeTarget.create(target.__proxy)

								for (let _pureData of pureData) {
									let sourceTypename
									if (_pureData['__typename']){
										sourceTypename = _pureData['__typename']
									}
									let obj = getElementComponent(target.__proxy, key, sourceTypename).createObject(target.__proxy)

									obj.enableNotifications = false
									obj.fromObject(_pureData)
									obj.enableNotifications = true
									model.append({ item: obj })
									obj.owner = target.__proxy
								}

								target[key] = model
								model.owner = target.__proxy
								model.finished()
							}
							else {
								target[key] = pureData
							}
						} else {
							let sourceTypename
							if (target[key]['__typename']){
								sourceTypename = target[key]['__typename']
							}	
							let obj = target.__proxy.createComponent(key, sourceTypename).createObject(target.__proxy)

							// Owner is attached only after filling, so a lazy read does not report a model change
							obj.fromObject(pureData)

							target[key] = obj
							obj.owner = target.__proxy
						}
					} else {
						target[key] = pureData
					}

					
				}
                return node.type.get(target, key, node)
			} else if(target.constructor.meta.hasOwnProperty(key)){
                let node = target.constructor.meta[key]
                return node.type.get(target, key, node)
            } else {
                return target[key]
            }
        },

        set(target, key, value){
            if(target.constructor.meta.hasOwnProperty(key)){
                let node = target.constructor.meta[key]

                if(node.type.isAssignableFrom(Property)){
                    return node.type.reset(target, key, value, node)
                } else {
                    return node.type.set(target, key, value, node)
                }
            } else {
                target[key] = value

                return true
            }
        },
    }

	static create(parent = null, properties = {}){
		let proxy = super.create(parent, properties)

		proxy._internal = Internal.create()
		proxy._internal.isTransaction = false
		proxy._internal.countChanges = 0
		proxy._internal.changeList = null
		proxy._internal.__base = proxy

		return proxy
	}

	static cachedPoperties = new Set(['__typename'])

	SLOT_modelChanged(changeSet){
		if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
			let ownerChangeSet = this.owner.prependChildPath !== undefined ? this.owner.prependChildPath(this, changeSet) : changeSet
			if (this.owner._internal.isTransaction){
				this.owner._internal.changeList = this.owner._internal.changeList.concat(ownerChangeSet)
				this.owner._internal.countChanges++
			}
			else{
				this.owner.modelChanged(ownerChangeSet)
			}
		}
	}

	// A change record of modelChanged(changeSet), see Docs/Development/ModelChangeSet.md
	createChange(path, operation, name, sender){
		return {"path": path, "operation": operation, "name": name, "sender": sender}
	}

	// Path step of a property: its JSON key, as in the SDL schema.
	getPathKey(propertyId){
		let key = this.getJSONKeyForProperty(propertyId)
		if (key === undefined || key === null || key === ""){
			return propertyId
		}

		return key
	}

	// Reports changes of this object at once, or adds them to the open transaction.
	reportChanges(changes){
		if (this._internal.isTransaction){
			this._internal.changeList = this._internal.changeList.concat(changes)
			this._internal.countChanges++

			return
		}

		this.modelChanged(changes)
	}

	// Property holding child directly or as a list element: {"name": "m_items", "index": 2}, index -1 for a direct child.
	// Reads the stored values: a lazily loaded object that is not created yet cannot be the child.
	findChild(child){
		// Edits of many elements of a list go in order, so the next element is found next to the last one
		let lastElement = findLastElement(this)
		if (lastElement !== null){
			let list = this.__self[lastElement.name]
			if (list instanceof BaseModel){
				for (let delta = 1; delta >= -1; delta--){
					let index = lastElement.index + delta
					if (index >= 0 && index < list.count && list.get(index).item === child){
						lastElement.index = index
						return {"name": lastElement.name, "index": index}
					}
				}
			}
		}

		// A list is a property value itself, never an element: the lists need no scan
		let isList = child instanceof BaseModel
		for (let key of this.getProperties()) {
			let value = this.__self[key]
			if (value === null || value === undefined || typeof value !== "object"){
				continue
			}

			if (value === child){
				return {"name": key, "index": -1}
			}

			if (!isList && value instanceof BaseModel){
				for (let k = 0; k < value.count; k++){
					if (value.get(k).item === child){
						rememberElement(this, key, k)
						return {"name": key, "index": k}
					}
				}
			}
		}

		return null
	}

	// The changes of child with the path of child in this object in front; the path is null when child is not found.
	prependChildPath(child, changeSet){
		let location = this.findChild(child)
		let step = null
		if (location !== null){
			step = location.index < 0 ? [this.getPathKey(location.name)] : [this.getPathKey(location.name), location.index]
		}

		// The child does not tell what changed in it: all of it may have. An empty changeSet would add nothing to a transaction.
		if (!changeSet || changeSet.length === 0){
			return [this.createChange(step, "set", location !== null ? location.name : "", child)]
		}

		let result = []
		for (let change of changeSet){
			let path = step !== null && change.path ? step.concat(change.path) : null
			result.push(this.createChange(path, change.operation ? change.operation : "set", change.name, change.sender))
		}

		return result
	}

	// Change of a list of this object: operation "insert", "remove" or "set" at the element index.
	createListChange(list, operation, index){
		let location = this.findChild(list)
		if (location === null || location.index >= 0){
			return this.createChange(null, operation, "", list)
		}

		return this.createChange([this.getPathKey(location.name), index], operation, location.name, list)
	}

	// SLOT_modelChanged(changeSet){
	// 	if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
	// 		this.owner.modelChanged(changeSet)
	// 	}
	// }

	beginChanges() {
		this._internal.startTransaction()
	}

	endChanges() {
		this._internal.stopTransaction()
	}

	removeKey(key){
		let selfKeys = this.getProperties()

		if (selfKeys.has(key)) {
			if (this[key] && this[key].destroy){
				this[key].destroy()
			}
			this[key] = null
		}

		if(!this._internal.removed) this._internal.removed = []
		if(this._internal.removed.indexOf(key) === -1) this._internal.removed.push(key)
	}

	connectProperties() {}

	createMe() {
		return this.constructor.create()
	}

	isEqualWithModel(model) {
		if (typeof this != typeof model) {
			return false
		}

		let selfKeys = this.getProperties()
		let sourceKeys = model.getProperties()

		if (selfKeys.size !== sourceKeys.size) {
			return false;
		}

		for (let key of selfKeys) {
			if (!sourceKeys.has(key)) {
				return false
			}

			if (typeof this[key] !== typeof model[key]) {
				return false
			}

			if (typeof this[key] === 'object') {
				if (this[key] && this[key].isEqualWithModel) {
					let ok = this[key].isEqualWithModel(model[key])
					if (!ok) {
						return false
					}
				}
				else {
					let ok = (this[key] === model[key])
					if (!ok) {
						return false
					}
				}
			}
			else {
				if (this[key] !== model[key]) {
					return false
				}
			}
		}

		return true
	}

	refresh() {
	}

	copy(item) {
		return copyFrom(item)
	}

	copyMe() {
		let obj = this.createMe()
		obj.copyFrom(this)

		return obj
	}

	copyFrom(item) {
		this.fromJSON(item.toJson())
		return true
	}

	copyFrom2(item) {
		let sourceObject = item
		for(let objKey of this.getProperties()){
			if (!(this.getJSONKeyForProperty(objKey) in sourceObject)){
				if(this[objKey] && typeof this[objKey] === "object"){
					if (this[objKey].clear){
						this[objKey].clear()
					}
					if (this[objKey].destroy){
						this[objKey].destroy()
					}
					this[objKey] = null
				}
			}
		}

		for (let key of item.getProperties()) {
			if (key === '__typename' || (sourceObject[key] == null && sourceObject._internal.containceInRemoved(key))){
				continue
			}

			this._internal.removeAt(key)

			if (sourceObject[key] === null){
				this[key] = null
			}
			else if (typeof sourceObject[key] === "object") {
				if (Array.isArray(sourceObject[key])) {
					let component = this.createComponent(key)

					if (this[key]) {
						if (this[key].clear) {
							this[key].clear()
						}
					} else {
						if (component) {
							let obj = BaseModel.create(this)
							obj.owner = this
							this[key] = obj
						}
					}

					if (component) {
						this[key].owner = this
						for (let sourceObjectInner of sourceObject[key]) {
							let sourceTypename
							if (sourceObjectInner['__typename']){
								sourceTypename = sourceObjectInner['__typename']
							}
							let obj = getElementComponent(this, key, sourceTypename).createObject(this)
							obj.copyFrom(sourceObjectInner)
							this[key].append({ item: obj })
							obj.owner = this
						}
					}
					else {
						this[key] = sourceObject[key]
					}
				} else {
					let obj
					if (!this[key]) {
						let sourceData = sourceObject[key]
						let sourceTypename
						if (sourceData['__typename']){
							sourceTypename = sourceData['__typename']
						}	
						obj = this.createComponent(key, sourceTypename).createObject(this)
					}
					else {
						obj = this[key]
					}

					obj.copyFrom(sourceObject[key])
					this[key] = obj

					obj.owner = this
				}
			} else {
				this[key] = sourceObject[key]
			}
		}

		this.finished()
		
		return true
	}

	copyTo(item) {
		item.copyFrom(this)
		return true
	}

	createComponent(propertyId, typename){
	}


	createElement(propertyId, typename){
	}

	getJSONKeyForProperty(propertyId) {
		return propertyId
	}

	getProperties() {
		return this.__self.constructor.cachedPoperties
	}

	isArrayRequired(propertyId) {
		return false
	}

	areArrayElementsRequired(propertyId) {
		return false
	}

	isArrayValueValid(propertyId, value) {
		if (value === null || value === undefined) {
			return !this.isArrayRequired(propertyId)
		}
		if (!this.areArrayElementsRequired(propertyId)) {
			return true
		}
		if (Array.isArray(value)) {
			for (let element of value) {
				if (element === null || element === undefined) {
					return false
				}
			}
		}
		else if (value.hasNullElements && value.hasNullElements()) {
			return false
		}

		return true
	}

	createFromJson(json) {
		return this.fromJSON(json)
	}

	// Returns '' if a required array is null or has null elements; scalars are not validated
	toJson() {
		let json = '{'
		let isFirst = true
		for (let key of this.getProperties()) {
			let value = this[key]
			if(key === '__typename' && value === '') continue

			if (value == null) {
				if (!this.isArrayValueValid(key, value)) {
					return ''
				}
				if (this._internal.containceInRemoved(key)) {
					continue
				}
			}
			if (!isFirst) json += ','
			isFirst = false
			if (typeof value === 'object') {
				if (Array.isArray(value)) {
					if (!this.isArrayValueValid(key, value)) {
						return ''
					}

					json += '"' + this.getJSONKeyForProperty(key) + '":'

					json += "["

					for (let j = 0; j < value.length; j++) {
						let element = value[j]
						if (j != 0) {
							json += ", "
						}

						if (typeof element === "string") {
							element = JSON.stringify(element)
						}
						
						json += element
					}

					json += "]"
				}
				else if (value !== null) {
					if (!this.isArrayValueValid(key, value)) {
						return ''
					}
					let serializedValue = value.toJson()
					if (serializedValue === '') {
						return ''
					}
					json += '"' + this.getJSONKeyForProperty(key) + '":' + serializedValue
				}
				else{
					json += '"' + this.getJSONKeyForProperty(key) + '": null'
				}
			} else {
				if (value === undefined) {
					json += '"' + this.getJSONKeyForProperty(key) + '":null'
				}
				else if (typeof value === 'string') {
					json += '"' + this.getJSONKeyForProperty(key) + '":"' + this.escapeSpecialChars(value) + '"'
				}
				else {
					json += '"' + this.getJSONKeyForProperty(key) + '":' + value
				}
			}
		}
		json += '}'
		return json
	}

	// Returns '' if a required array is null or has null elements; scalars are not validated
	toGraphQL() {
		let graphQL = '{'
		let isFirst = true
		for (let key of this.getProperties()) {
			let value = this[key]
			if (value == null) {
				if (!this.isArrayValueValid(key, value)) {
					return ''
				}
				if (this._internal.containceInRemoved(key)) {
					continue
				}
			}
			if (!isFirst) graphQL += ','
			isFirst = false
			if (typeof value === 'object') {
				if (Array.isArray(value)) {
					if (!this.isArrayValueValid(key, value)) {
						return ''
					}

					graphQL += this.getJSONKeyForProperty(key) + ':'

					graphQL += "["

					for (let j = 0; j < value.length; j++) {
						let element = value[j]
						if (j != 0) {
							graphQL += ", "
						}

						if (typeof element === "string") {
							graphQL += "\"" + this.escapeSpecialChars(element) + "\""
						}
						else {
							graphQL += element
						}
					}

					graphQL += "]"
				}
				else if (value !== null) {
					if (!this.isArrayValueValid(key, value)) {
						return ''
					}
					let serializedValue = value.toGraphQL()
					if (serializedValue === '') {
						return ''
					}
					graphQL += this.getJSONKeyForProperty(key) + ':' + serializedValue
				}
				else {
					graphQL += this.getJSONKeyForProperty(key) + ':null'
				}
			} else {
				graphQL += this.getJSONKeyForProperty(key) + ':';
				if (value === undefined) {
					graphQL += 'null'
				}
				else if (typeof value === 'string') {
					graphQL += '"'
					graphQL += this.escapeSpecialChars(value)
					graphQL += '"'
				}
				else {
					graphQL += value
				}
			}
		}
		graphQL += '}'
		return graphQL
	}

	fromJSON(json) {
		let obj;
		try {
			obj = JSON.parse(json.replace(/\n/g, "\\n").replace(/\r/g, "\\r").replace(/\t/g, "\\t").replace(/\f/g, "\\f"))
		} catch (e) {
			console.error(e)
			return false
		}

		return this.fromObject(obj)
	}

	escapeSpecialChars(jsonString) {
		// Most strings have nothing to escape: one search instead of six replacements
		if (!specialCharsPattern.test(jsonString)){
			return jsonString
		}

		return jsonString.replace(/\\/g, "\\\\")
		.replace(/\"/g, "\\\"")
		.replace(/\n/g, "\\n")
		.replace(/\r/g, "\\r")
		.replace(/\t/g, "\\t")
		.replace(/\f/g, "\\f")
	}

	fromObject(sourceObject) {
		// One modelChanged for the whole object, as in Qt
		this.beginChanges()

		for(let objKey of this.getProperties()){
			if (!(this.getJSONKeyForProperty(objKey) in sourceObject)){
				if(this[objKey] && typeof this[objKey] === "object"){
					if (this[objKey].clear){
						this[objKey].clear()
					}
					if (this[objKey].destroy){
						this[objKey].destroy()
					}
					this[objKey] = null
				}
			}
		}

		let refilledLists = []
		for (let key in sourceObject) {
			let _key = "m_" + key[0].toLowerCase() + key.slice(1, key.length)

			this._internal.removeAt(_key)

			// Only a list of SDL objects (it has an element component) is filled again; an array of values is a new value, as in Qt.
			// getPropertyType() does not tell them apart: it says "BaseModel" for an array of enum values too.
			let current = this.__self[_key]
			if (Array.isArray(sourceObject[key]) && current !== null && current !== undefined && this.createComponent(_key)){
				refilledLists.push(_key)
			}

			if (sourceObject[key] === null){
				this[_key] = null
			} else {
				this[_key] = sourceObject[key]
			}
		}

		// A list filled again is not reported, as in Qt: filling from data is a load, not an edit
		if (refilledLists.length > 0){
			this._internal.changeList = this._internal.changeList.filter(change => refilledLists.indexOf(change.name) < 0)
			this._internal.countChanges = this._internal.changeList.length
		}

		this.endChanges()
		this.finished()
		
		return true
	}

	destroy(){
		if(this._internal) this._internal.destroy()
		super.destroy()
	}
}



module.exports = BaseClass



// module.exports.BaseClass = BaseClass
