const { QtObject } = require('./QtObject')
const { QBool, QVar, QInt } = require('../utils/properties')
const { Qt } = require('../utils/Qt')

class Internal extends QtObject {
	static defaultProperties = {
		isTransaction: { type: QBool, value: false },
		countChanges: { type: QInt, value: 0 },
		changeList: { type: QVar, value: null },
	}

	static defaultSignals = {
		internalModelChanged: { params: ['name', 'sender'] },
	}

	$complete() {
		if (!this.$completed) {
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

	$onModelChanged(changeSet){
		if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
			this.owner.modelChanged(changeSet)
		}
	}

	$complete() {
		if (!this.$completed) {
			this._internal = new Internal(this)
			this.modelChanged.connect(this, this.$onModelChanged)
			this.connectProperties()
		}
		super.$complete()
	}

	beginChanges() {
		this._internal.startTransaction();
	}

	endChanges() {
		this._internal.stopTransaction();
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
				if (this[key].isEqualWithModel) {
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

	createComponent(name) {

	}

	getJSONKeyForProperty(propertyId) {
		return propertyId
	}

	getProperties() {
		let list = []

		for (let key in this.$properties) {
			if (key[0] === 'm' && key[1] === '_') {
				list.push(key)
			}
		}

		return list
	}

	createFromJson(json) {
		return this.fromJSON(json)
	}

	toJson() {
		let list = this.getProperties()

		let json = '{'
		for (let i = 0; i < list.length; i++) {
			let key = list[i]
			if (typeof this[key] === 'object') {
				if (Array.isArray(this[key])) {
					if (i > 0) {
						json += ','
					}

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
				else if (this[key]) {
					if (i > 0) {
						json += ','
					}

					json += '"' + this.getJSONKeyForProperty(key) + '":' + this[key].toJson()
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

				if (i > 0) {
					json += ','
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
		for (let i = 0; i < list.length; i++) {
			let key = list[i]
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
			if (i < list.length - 1) graphQL += ','
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
							let obj = this.createComponent(_key).createObject(this)
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
						obj = this.createComponent(_key).createObject(this)
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
