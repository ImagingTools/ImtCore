const QtObject = require("./QtObject")
const Bool = require("./Bool")
const Int = require("./Int")
const Var = require("./Var")
const Signal = require("./Signal")

class Internal extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        isTransaction: { type: Bool, value: false },
		countChanges: { type: Int, value: 0 },
		changeList: { type: Var, value: null },

		internalModelChanged: { type:Signal, args: ['name', 'sender'] },
    })

	__complete() {
		if (!this.__completed) {
			this.changeList = []

			Signal.get(this, 'internalModelChanged').connect((name, sender) => {
				if (this.isTransaction){
					let changeObj = {"name":name,"sender":sender}
						this.changeList.push(changeObj)
						this.countChanges++
						return
					}
				this.parent.modelChanged([{"name":name,"sender":sender}])
			})

		}
		super.__complete()
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
			this.parent.modelChanged(this.changeList)

			this.countChanges = 0
		}

		this.isTransaction = false
	}
}

class BaseClass extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        enableNotifications: { type: Bool, value: true },
		propertiesIsConnected: { type: Bool, value: false },
		owner: { type: Var, value: null },

		modelChanged: { type:Signal, args: ['changeSet'] },
		finished: { type:Signal, args: [] },
    })

	__complete() {
		if (!this.__completed) {
			this.__internal = Internal.create(this)
			Signal.get(this, 'modelChanged').connect(this, (changeSet)=>{
				if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
					this.owner.modelChanged(changeSet)
				}
			})
			this.connectProperties()
		}
		super.__complete()
	}

	// SLOT_modelChanged(changeSet){
	// 	if (this.owner && this.owner.enableNotifications && this.owner.modelChanged) {
	// 		this.owner.modelChanged(changeSet)
	// 	}
	// }

	beginChanges() {
		this.__internal.startTransaction()
	}

	endChanges() {
		this.__internal.stopTransaction()
	}

	connectProperties() {
		if (this.propertiesIsConnected) {
			return
		}

		let list = this.getProperties()

		for (let name of list) {
			Signal.get(this, name + 'Changed').connect(()=>{
				if (this.enableNotifications) this.__internal.internalModelChanged(name, self)
			})
		}

		this.propertiesIsConnected = true
	}

	createMe() {
		return BaseClass.create()
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
		let meta = this.__self.constructor.meta
		let list = []

		for (let key in meta) {
			if (meta[key].type !== Signal && key[0] === 'm' && key[1] === '_') {
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
			obj = JSON.parse(this.escapeSpecialChars(json))
		} catch (e) {
			console.error(e)
			return false
		}

		return this.fromObject(obj)
	}

	escapeSpecialChars(jsonString) {
		return jsonString.replace(/\n/g, "\\n")
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
		if(this.__internal) this.__internal.destroy()
		super.destroy()
	}
}



module.exports = BaseClass



// module.exports.BaseClass = BaseClass
