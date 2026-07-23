const QtObject = require("./QtObject")
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
		isTransaction: { type: Bool, value: false },
		countChanges: { type: Int, value: 0 },
		changeList: { type: Var, value: null },
		removed: { type: Var, value: null },
		__typename: { type: String, value: '' },

		enableNotificationsChanged: {type:Signal, args:[]},
		propertiesIsConnectedChanged: {type:Signal, args:[]},
		ownerChanged: {type:Signal, args:[]},
		_internalChanged: {type:Signal, args:[]},
		isTransactionChanged: {type:Signal, args:[]},
		countChangesChanged: {type:Signal, args:[]},
		changeListChanged: {type:Signal, args:[]},
		removedChanged: {type:Signal, args:[]},
		__typenameChanged: {type:Signal, args:[]},

		internalModelChanged: { type:Signal, args: ['name', 'sender'] },
		modelChanged: { type:Signal, args: ['changeSet'] },
		finished: { type:Signal, args: [] },
    })
	
	static cachedPoperties = new Set(['__typename'])


	SLOT_internalModelChanged(name, sender){
		if (this.isTransaction){
			let changeObj = {"name":name,"sender":sender}
				this.changeList.push(changeObj)
				this.countChanges++
				return
			}
		this.modelChanged([{"name":name,"sender":sender}])
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
			this.modelChanged(this.changeList)

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

	get _internal(){
		return this.__proxy
	}

	SLOT_modelChanged(changeSet){
		if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
			if (this.owner.isTransaction){
				this.owner.changeList.concat(changeSet)
				this.owner.countChanges++
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
		this.startTransaction()
	}

	endChanges() {
		this.stopTransaction()
	}

	removeKey(key){
		let selfKeys = this.getProperties()

		if (selfKeys.has(key)) {
			if (this[key] && this[key].destroy){
				this[key].destroy()
			}
			this[key] = null
		}

		if(!this.removed) this.removed = []
		this.removed.push(key)
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
		for (let key of this.getProperties()) {
			if(key === '__typename' && this[key] === '') continue

			if (this[key] == null && this._internal.containceInRemoved(key)){
				continue
			}
			if (!isFirst) json += ','
			isFirst = false
			if (typeof this[key] === 'object') {
				if (Array.isArray(this[key])) {

					json += '"' + this.getJSONKeyForProperty(key) + '":'

					json += "["

					for (let j = 0; j < this[key].length; j++) {
						let value = this[key][j]
						if (j != 0) {
							json += ", "
						}

						if (typeof value === "string") {
							value = JSON.stringify(value)
						}
						
						json += value
					}

					json += "]"
				}
				else if (this[key] !== null) {
					json += '"' + this.getJSONKeyForProperty(key) + '":' + this[key].toJson()
				}
				else{
					json += '"' + this.getJSONKeyForProperty(key) + '": null'
				}
			} else {
				let value = this[key]
				if (value === undefined) {
					value = null
				}
				let safeValue = this[key]
				if (typeof safeValue === 'string') {
					safeValue = this.escapeSpecialChars(safeValue)
				}

				json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof this[key] === 'string' ? '"' + safeValue + '"' : value)
			}
		}
		json += '}'
		return json
	}

	toGraphQL() {
		let graphQL = '{'
		let isFirst = true
		for (let key of this.getProperties()) {
			if (this[key] == null && this._internal.containceInRemoved(key)){
				continue
			}
			if (!isFirst) graphQL += ','
			isFirst = false
			if (typeof this[key] === 'object') {
				if (Array.isArray(this[key])) {
					graphQL += this.getJSONKeyForProperty(key) + ':'

					graphQL += "["

					for (let j = 0; j < this[key].length; j++) {
						if (j != 0) {
							graphQL += ", "
						}

						if (typeof this[key][j] === "string") {
							graphQL += "\"" + this.escapeSpecialChars(this[key][j]) + "\""
						}
						else {
							graphQL += this[key][j]
						}
					}

					graphQL += "]"
				}
				else {
					graphQL += this.getJSONKeyForProperty(key) + ':' + ((this[key] !== null) ? this[key].toGraphQL() : "null")
				}
			} else {
				let value = this[key]
				if (value === undefined) {
					value = null
				}

				graphQL += this.getJSONKeyForProperty(key) + ':';
				if (typeof this[key] === 'string') {
					let data = this[key];
					
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


		// for(let key in obj){
		// 	this[key] = obj[key]
		// }
		// return true
		
		this.fromObject(obj)
	}

	escapeSpecialChars(jsonString) {
		return jsonString.replace(/\\/g, "\\\\")
		.replace(/\"/g, "\\\"")
		.replace(/\n/g, "\\n")
		.replace(/\r/g, "\\r")
		.replace(/\t/g, "\\t")
		.replace(/\f/g, "\\f")
	}

	reviver(key,value){
		if(!key) return value

		if(value){
			if(typeof value === 'object'){
				if(Array.isArray(value)) {
					let model = BaseModel.create()
					for(let item of value){
						model.append({item: item})
					}
					
					return model
				} else {
					let obj = BaseClass.create()
					for(let key in value){
						obj[key] = value[key]
					}
					return obj
				}
			}

			return value
		}

		return value
	}

	fromObject(sourceObject) {
		let self = this.__self 
		for(let _key of this.getProperties()){
			let key = _key.replace('m_', '')
			if (!(key in sourceObject)){
				if(self[_key] && typeof self[_key] === "object"){
					if (self[_key].clear){
						self[_key].clear()
					}
					if (self[_key].destroy){
						self[_key].destroy()
					}
					self[_key] = null
				}
			}

			this.removeAt(_key)

			if (sourceObject[key] === null){
				self[_key] = null
			}
			else if (typeof sourceObject[key] === "object") {
				if (Array.isArray(sourceObject[key])) {
					let component = this.createComponent(_key)

					if (component) {
						if(!self[_key]){
							let obj = BaseModel.create(this)
							obj.owner = this
							self[_key] = obj
						}

						self[_key].owner = this
						for (let sourceObjectInner of sourceObject[key]) {
							let sourceTypename
							if (sourceObjectInner['__typename']){
								sourceTypename = sourceObjectInner['__typename']
							}
							let obj = this.createElement(_key, sourceTypename).createObject(this)
							obj.fromObject(sourceObjectInner)
							self[_key].append({ item: obj })
							obj.owner = this
						}
					}
					else {
						self[_key] = sourceObject[key]
					}
				} else {
					let obj
					if (!self[_key]) {
						let sourceData = sourceObject[key]
						let sourceTypename
						if (sourceData['__typename']){
							sourceTypename = sourceData['__typename']
						}	
						obj = this.createComponent(_key, sourceTypename).createObject(this)
					}
					else {
						obj = self[_key]
					}

					obj.fromObject(sourceObject[key])
					self[_key] = obj

					obj.owner = this
				}
			} else {
				self[_key] = sourceObject[key]
			}
		}

		this.finished()
		
		return true
	}
}



module.exports = BaseClass



// module.exports.BaseClass = BaseClass
