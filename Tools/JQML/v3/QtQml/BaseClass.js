const QtObject = require("./QtObject")
const Property = require("./Property")
const Bool = require("./Bool")
const Int = require("./Int")
const Var = require("./Var")
const String = require("./String")
const Signal = require("./Signal")
const BaseModel = require("./BaseModel")

class Internal extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        isTransaction: { type: Bool, value: false },
		countChanges: { type: Int, value: 0 },
		changeList: { type: Var, value: null },
		removed: { type: Var, value: null },
		__typename: { type: String, value: '' },

		isTransactionChanged: {type:Signal, args:[]},
		countChangesChanged: {type:Signal, args:[]},
		changeListChanged: {type:Signal, args:[]},
		removedChanged: {type:Signal, args:[]},
		__typenameChanged: {type:Signal, args:[]},

		internalModelChanged: { type:Signal, args: ['name', 'sender'] },
    })

	// removed = []
	// changeList = []

	SLOT_internalModelChanged(name, sender){
		if (this.isTransaction){
			let changeObj = {"name":name,"sender":sender}
				this.changeList.push(changeObj)
				this.countChanges++
				return
			}
		this.__base.modelChanged([{"name":name,"sender":sender}])
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
								target[key] = node.typeTarget.create(target.__proxy)
								target[key].owner = target.__proxy

								for (let _pureData of pureData) {
									let sourceTypename
									if (_pureData['__typename']){
										sourceTypename = _pureData['__typename']
									}
									let obj = target.__proxy.createElement(key, sourceTypename).createObject(target.__proxy)
									
									target[key].append({ item: obj })
									obj.owner = target.__proxy

									obj.fromObject(_pureData)
								}

								target[key].finished()
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

							target[key] = obj
							obj.owner = target.__proxy

							obj.fromObject(pureData)
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
		proxy._internal.__base = proxy

		return proxy
	}

	static cachedPoperties = new Set(['__typename'])

	SLOT_modelChanged(changeSet){
		if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
			if (this.owner._internal.isTransaction){
				this.owner._internal.changeList.concat(changeSet)
				this.owner._internal.countChanges++
			}
			else{
				this.owner.modelChanged(changeSet)
			}
		}
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
		this._internal.removed.push(key)
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
							let obj = this.createElement(key, sourceTypename).createObject(this)
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

	createFromJson(json) {
		return this.fromJSON(json)
	}

	toJson() {
		let json = '{'
		let isFirst = true
		let self = this.__self

		for (let key of this.getProperties()) {
			let meta = self.constructor.meta[key]
			let thisValue = key in self ? self[key] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

			if(key === '__typename' && thisValue === '') continue

			if (thisValue == null && this._internal.containceInRemoved(key)){
				continue
			}
			if (!isFirst) json += ','
			isFirst = false
			if (typeof thisValue === 'object') {
				if(thisValue instanceof QtObject) {
					json += '"' + this.getJSONKeyForProperty(key) + '":' + thisValue.toJson()
				} else if(thisValue !== null){
					json += '"' + this.getJSONKeyForProperty(key) + '":' + JSON.stringify(thisValue)
				} else {
					json += '"' + this.getJSONKeyForProperty(key) + '": null'
				}
			} else {
				let value = thisValue
				if (value === undefined) {
					value = null
				}
				let safeValue = thisValue
				if (typeof safeValue === 'string') {
					safeValue = this.escapeSpecialChars(safeValue)
				}

				json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof thisValue === 'string' ? '"' + safeValue + '"' : value)
			}
		}
		json += '}'
		return json
	}

	toGraphQL() {
		let graphQL = '{'
		let isFirst = true
		let self = this.__self

		for (let key of this.getProperties()) {
			let meta = self.constructor.meta[key]
			let thisValue = key in self ? self[key] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

			if (thisValue == null && this._internal.containceInRemoved(key)){
				continue
			}
			if (!isFirst) graphQL += ','
			isFirst = false
			if (typeof thisValue === 'object') {
				if(thisValue instanceof QtObject) {
					graphQL += this.getJSONKeyForProperty(key) + ':' + thisValue.toGraphQL()
				} else if(thisValue !== null){
					graphQL += this.getJSONKeyForProperty(key) + ':' + JSON.stringify(thisValue)
				} else {
					graphQL += this.getJSONKeyForProperty(key) + ': null'
				}
			} else {
				let value = thisValue
				if (value === undefined) {
					value = null
				}

				graphQL += this.getJSONKeyForProperty(key) + ':';
				if (typeof thisValue === 'string') {
					let data = thisValue;
					
					graphQL += '"'
					graphQL += this.escapeSpecialChars(data)
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
		return jsonString.replace(/\\/g, "\\\\")
		.replace(/\"/g, "\\\"")
		.replace(/\n/g, "\\n")
		.replace(/\r/g, "\\r")
		.replace(/\t/g, "\\t")
		.replace(/\f/g, "\\f")
	}

	fromObject(sourceObject) {
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

		for (let key in sourceObject) {
			let _key = "m_" + key[0].toLowerCase() + key.slice(1, key.length)

			this._internal.removeAt(_key)

			if (sourceObject[key] === null){
				this[_key] = null
			} else {
				this[_key] = sourceObject[key]
			}
		}

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
