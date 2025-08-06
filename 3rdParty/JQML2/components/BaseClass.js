const { QtObject } = require('./QtObject')
const { QBool, QVar, QInt } = require('../utils/properties')
const { Qt } = require('../utils/Qt')

class Internal extends QtObject {
	static defaultProperties = {
		isTransaction: { type: QBool, value: false },
		countChanges: { type: QInt, value: 0 },
		changeList: { type: QVar, value: null },
		removed: { type: QVar, value: [] },
	}

	static defaultSignals = {
		internalModelChanged: { params: ['name', 'sender'] },
	}

	$complete() {
		if (!this.$completed) {
			this.useTypename = false
			this.changeList = []

			this.internalModelChanged.connect((name, sender) => {
												  if (this.isTransaction){
													  let changeObj = {"name":name,"sender":sender}

													  this.changeList.push(changeObj)

													  this.countChanges++;

													  return;
												  }

												  this.parent.modelChanged([{"name":name,"sender":sender}]);
											  })

		}
		super.$complete()
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

			return;
		}

		if (this.countChanges > 0){
			this.parent.modelChanged(this.changeList)

			this.countChanges = 0
		}

		this.isTransaction = false
	}

	removeAt(key){
	// get index if value found otherwise -1
		let index = this.removed.indexOf(key)
		if (index > -1) { //if found
			this.removed.splice(index, 1)
		}
	}

	containsInRemoved(key){
		let index = this.removed.indexOf(key)
		if (index > -1) {
			return true
		}

		return false
	}
}

for(let propName in Internal.defaultProperties){
	Object.defineProperty(Internal.prototype, propName, {
							  get: function(){
								  let property = this.getProperty(propName)
								  let caller = global.queueLink[global.queueLink.length-1]
								  if(caller) caller.subscribe(property)
								  return property.get()
							  },
							  set: function(newVal){
								  this.getProperty(propName).reset(newVal)
							  }
						  })

	Object.defineProperty(Internal.prototype, propName+'Changed', {
							  get: function(){
								  return this.getProperty(propName).getNotify()
							  },
						  })
}

for(let sigName in Internal.defaultSignals){
	Object.defineProperty(Internal.prototype, sigName, {
							  get: function(){
								  return this.getSignal(sigName)
							  },
						  })
}

class BaseClass extends QtObject {
	static defaultProperties = {
		enableNotifications: { type: QBool, value: true },
		propertiesIsConnected: { type: QBool, value: false },
		owner: { type: QVar, value: null },
	}

	static defaultSignals = {
		modelChanged: { params: ['changeSet'] },
		finished: { params: [] },
	}

	$propertiesMember = []

	createProperty(name, type, value){
		if((name[0] === 'm' && name[1] === '_' && this.$propertiesMember.indexOf(name) < 0) || (name == '__typename')) this.$propertiesMember.push(name)
        super.createProperty(name, type, value)
    }

    createVariantProperty(name, type, value){
		if((name[0] === 'm' && name[1] === '_' && this.$propertiesMember.indexOf(name) < 0) || (name == '__typename')) this.$propertiesMember.push(name)
        super.createVariantProperty(name, type, value)
    }

	$onModelChanged(changeSet){
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

	$complete() {
		if (!this.$completed) {
	// 		this._internal = new Internal(this)
	// 		this.modelChanged.connect(this, this.$onModelChanged)
			this.connectProperties()
		}
		super.$complete()
	}

	constructor(parent,exCtx,exModel){
		super(parent,exCtx,exModel)

		this._internal = new Internal(this)
		this.modelChanged.connect(this, this.$onModelChanged)
		// this.connectProperties()
	}

	beginChanges() {
		this._internal.startTransaction();
	}

	endChanges() {
		this._internal.stopTransaction();
	}

	removeKey(key){
		let selfKeys = this.getProperties()

		if (selfKeys.includes(key)) {
			if (this[key] && this[key].destroy){
				this[key].destroy()
			}
			this[key] = null
		}
		this._internal.removed.push(key)
	}

	connectProperties() {
		if (this.propertiesIsConnected) {
			return
		}

		let list = this.getProperties()

		for (let name of list) {
			this[name + 'Changed'].connect(()=>{
											   if (this.enableNotifications)
											   this._internal.internalModelChanged(name, self)
										   })
		}

		this.propertiesIsConnected = true
	}

	createMe() {
		return new BaseClass()
	}

	isEqualWithModel(model) {
		if (typeof this != typeof model) {
			return false
		}

		let selfKeys = this.getProperties()
		let sourceKeys = model.getProperties()

		if (selfKeys.length !== sourceKeys.length) {
			return false;
		}

		for (let i = 0; i < selfKeys.length; i++) {
			let key = selfKeys[i]

			if (!sourceKeys.includes(key)) {
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
					let ok = (this[key] === model[key]);
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
		obj.fromJSON(this.toJson())
		obj.connectProperties()

		return obj
	}

	copyFrom(item) {
		this.fromJSON(item.toJson())
		return true
	}

	copyTo(item) {
		item.fromJSON(this.toJson())
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
		return this.$propertiesMember
	}

	createFromJson(json) {
		return this.fromJSON(json)
	}

	toJson() {
		let list = this.getProperties()

		let json = '{'
		let isFirst = true
		for (let i = 0; i < list.length; i++) {
			let key = list[i]
			if (this[key] == null && this._internal.containsInRemoved(key)){
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
					safeValue = safeValue.replace(/\\/g, '\u005C\u005C')
					safeValue = safeValue.replace(/\"/g, '\u005C"')
				}

				json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof this[key] === 'string' ? '"' + safeValue + '"' : value)
			}
		}
		json += '}'
		return json
	}

	toGraphQL() {
		let list = this.getProperties()

		let graphQL = '{'
		let isFirst = true
		for (let i = 0; i < list.length; i++) {
			let key = list[i]
			if (this[key] == null && this._internal.containsInRemoved(key)){
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
							graphQL += "\"" + this[key][j] + "\""
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

					data = data.replace(/\\/g, "\\\\")
					data = data.replace(/\"/g, "\\\"")
					data = data.replace(/\r/g, "\\\\r")
					data = data.replace(/\n/g, "\\\\n")
					data = data.replace(/\t/g, "\\\\t")

					graphQL += '"'
					graphQL += data
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
			obj = JSON.parse(this.escapeSpecialChars(json));
		} catch (e) {
			console.error(e);
			return false;
		}

		return this.fromObject(obj)
	}

	escapeSpecialChars(jsonString) {
		return jsonString.replace(/\n/g, "\\n")
		.replace(/\r/g, "\\r")
		.replace(/\t/g, "\\t")
		.replace(/\f/g, "\\f");
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
			if (sourceObject[key] === null){
				this[_key] = null
			}
			else if (typeof sourceObject[key] === "object") {
				if (Array.isArray(sourceObject[key])) {
					let component = this.createComponent(_key)

					if (this[_key]) {
						if (this[_key].clear) {
							this[_key].clear()
						}
					} else {
						if (component) {
							let obj = Qt.createComponent('', 'BaseModel.qml', this).createObject(this)
							this[_key] = obj
						}
					}

					if (component) {
						for (let sourceObjectInner of sourceObject[key]) {
							let obj = this.createElement(_key).createObject(this)
							obj.fromObject(sourceObjectInner)
							this[_key].append({ item: obj })
							obj.owner = this
							obj.connectProperties()
						}
					}
					else {
						this[_key] = sourceObject[key]
					}
				} else {
					let obj
					if (!this[_key]) {
						let sourceData = sourceObject[key]
						let sourceTypename
						if (sourceData['__typename']){
							sourceTypename = sourceData['__typename']
						}	
						obj = this.createComponent(_key, sourceTypename).createObject(this)
					}
					else {
						obj = this[_key]
					}

					obj.fromObject(sourceObject[key])
					this[_key] = obj

					obj.owner = this
					obj.connectProperties()
				}
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

module.exports.BaseClass = BaseClass
