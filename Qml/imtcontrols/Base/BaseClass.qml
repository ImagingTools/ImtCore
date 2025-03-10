import QtQuick 2.0

QtObject {
	id: root;

	signal modelChanged(var changeSet)
	signal finished
	property bool enableNotifications: true
	property var owner

	property QtObject _internal: QtObject{
		property bool isTransaction: false; // if true - transaction started else stopped
		property int countChanges: 0; // the number of changes during the transaction
		property var changeList: [];
		signal internalModelChanged(string name, var sender); // internal modelChanged

		onInternalModelChanged: {
			if (isTransaction){
				let changeObj = {};
				changeObj["name"] = name;
				changeObj["sender"] = sender;

				changeList.push(changeObj)

				countChanges++;

				return;
			}

			root.modelChanged([{"name":name,"sender":sender}]);
		}

		function startTransaction(){
			if (isTransaction){
				console.error("Unable to start transaction. Error: transaction already started.");

				return;
			}

			changeList = []
			isTransaction = true;
		}

		function stopTransaction(){
			if (!isTransaction){
				console.error("Unable to stop transaction. Error: there is no active transaction.");

				return;
			}

			if (countChanges > 0){
				root.modelChanged(changeList);

				countChanges = 0;
			}

			isTransaction = false;
		}
	}

	Component.onCompleted: {
		connectProperties();
	}

	onModelChanged: {
		if(owner && owner.enableNotifications && owner.modelChanged){
			owner.modelChanged(changeSet)
		}
	}

	function beginChanges(){
		this._internal.startTransaction();
	}

	function endChanges(){
		this._internal.stopTransaction();
	}

	property bool propertiesIsConnected: false

	function connectProperties(){
		if (propertiesIsConnected){
			return;
		}

		let self = this
		let list = getProperties()

		for(let name of list){
			this[name+'Changed'].connect(function(){
				if(self.enableNotifications)
					self._internal.internalModelChanged(name, self)
			}
			)
		}

		propertiesIsConnected = true;
	}

	function createMe(){
		return Qt.createComponent('BaseClass.qml').createObject()
	}

	function isEqualWithModel(model){
		if (typeof this != typeof model){
			return false;
		}

		let selfKeys = this.getProperties()
		let sourceKeys = model.getProperties()

		if (selfKeys.length !== sourceKeys.length){
			return false;
		}

		for(let i = 0; i < selfKeys.length; i++){
			let key = selfKeys[i];

			if (!sourceKeys.includes(key)){
				return false;
			}

			if(typeof this[key] !== typeof model[key]){
				return false;
			}

			if(typeof this[key] === 'object'){
				if (this[key].isEqualWithModel){
					let ok = this[key].isEqualWithModel(model[key]);
					if (!ok){
						return false;
					}
				}
				else{
					let ok = (this[key] === model[key]);
					if (!ok){
						return false;
					}
				}
			}
			else{
				if (this[key] !== model[key]){
					return false;
				}
			}
		}

		return true;
	}

	function refresh(){
	}

	function copy(item){
		return copyFrom(item)
	}

	function copyMe(){
		let obj = createMe()
		obj.fromJSON(toJson())
		obj.connectProperties();

		return obj
	}

	function copyFrom(item){
		fromJSON(item.toJson())
		return true
	}

	function copyTo(item){
		item.fromJSON(toJson())
		return true
	}

	function createComponent(name){

	}

	function getJSONKeyForProperty(propertyId){
		return propertyId
	}

	function getProperties(){
		let list = []
		if(Qt.platform.os === 'web'){
			for(let key in this.$properties){
				if(key.indexOf('m_') >= 0 && key !== 'owner' && key !== 's_keys' && key !== 'objectName' && key !== 'enableNotifications' && typeof this[key] !== "function"){
					list.push(key)
				}
			}
		} else {
			for(let key in this){
				if(key.indexOf('m_') >= 0 && key !== 'objectName' && key !== 'owner' && key !== 's_keys' && key !== 'enableNotifications' && typeof this[key] !== "function"){
					list.push(key)
				}
			}
		}

		return list
	}

	function createFromJson(json){
		return fromJSON(json);
	}

	function toJson(){
		let list = getProperties()

		let json = '{'
		for(let i = 0; i < list.length; i++){
			let key = list[i]
			if(typeof this[key] === 'object'){
				if (Array.isArray(this[key])){
					if(i > 0){
						json += ','
					}

					json += '"' + this.getJSONKeyForProperty(key) + '":'

					json += "["

					for (let j = 0; j < this[key].length; j++){
						if (j != 0){
							json += ", "
						}

						if (typeof this[key][j] === "string"){
							json += "\"" + this[key][j] + "\""
						}
						else{
							json += this[key][j]
						}
					}

					json += "]"
				}
				else if (this[key]){
					if(i > 0){
						json += ','
					}

					json += '"' + this.getJSONKeyForProperty(key) + '":' + this[key].toJson()
				}
			} else {
				let value = this[key]
				if (value === undefined){
					value = null
				}
				let safeValue = this[key]
				if (typeof safeValue === 'string'){
					safeValue = safeValue.replace(/\\/g, '\u005C\u005C')
					safeValue = safeValue.replace(/\"/g,'\u005C"')
				}

				if(i > 0){
					json += ','
				}

				json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof this[key] === 'string' ? '"' + safeValue + '"' : value)
			}
		}
		json +='}'
		return json
	}

	function toGraphQL(){
		let list = getProperties()

		let graphQL = '{'
		for(let i = 0; i < list.length; i++){
			let key = list[i]
			if(typeof this[key] === 'object'){
				if (Array.isArray(this[key])){
					graphQL +=  this.getJSONKeyForProperty(key) + ':'

					graphQL += "["

					for (let j = 0; j < this[key].length; j++){
						if (j != 0){
							graphQL += ", "
						}

						if (typeof this[key][j] === "string"){
							graphQL += "\"" + this[key][j] + "\""
						}
						else{
							graphQL += this[key][j]
						}
					}

					graphQL += "]"
				}
				else{
					graphQL += this.getJSONKeyForProperty(key) + ':' + ((this[key] !== null) ? this[key].toGraphQL() : "null")
				}
			} else {
				let value = this[key]
				if (value === undefined){
					value = null
				}

				graphQL += this.getJSONKeyForProperty(key) + ':';
				if (typeof this[key] === 'string'){
					let data = this[key];

					data = data.replace(/\\/g,"\\\\")
					data = data.replace(/\"/g,"\\\"")
					data = data.replace(/\r/g,"\\\\r")
					data = data.replace(/\n/g,"\\\\n")
					data = data.replace(/\t/g,"\\\\t")

					graphQL += '"'
					graphQL += data
					graphQL += '"'
				}
				else{
					graphQL += value
				}
			}
			if(i < list.length - 1) graphQL += ','
		}
		graphQL +='}'
		return graphQL
	}

	function fromJSON(json){
		let obj;
		try {
			obj = JSON.parse(escapeSpecialChars(json));
		} catch (e) {
			console.error(e);
			return null;
		}

		return fromObject(obj)
	}

	function escapeSpecialChars(jsonString){
		return jsonString.replace(/\n/g, "\\n")
		.replace(/\r/g, "\\r")
		.replace(/\t/g, "\\t")
		.replace(/\f/g, "\\f");
	}

	function fromObject(sourceObject){
		for(let objKey of this.getProperties()){
			if (!(getJSONKeyForProperty(objKey) in sourceObject)){
				if(typeof this[objKey] === "object"){
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

		for(let key in sourceObject){
			let _key = "m_" + key[0].toLowerCase() + key.slice(1,key.length)
			if(typeof sourceObject[key] === "object"){
				if(Array.isArray(sourceObject[key])){
					let component = createComponent(_key)

					if(this[_key]){
						if (this[_key].clear){
							this[_key].clear()
						}
					} else {
						if (component){
							let obj = Qt.createComponent('BaseModel.qml').createObject(this)
							this[_key] = obj
						}
					}

					if (component){
						for(let sourceObjectInner of sourceObject[key]){
							let obj = createComponent(_key).createObject(this)
							obj.fromObject(sourceObjectInner)
							this[_key].append({item: obj})
							obj.owner = this
							obj.connectProperties()
						}
					}
					else{
						this[_key] = sourceObject[key]
					}
				} else {
					let obj
					if (!this[_key]){
						obj = createComponent(_key).createObject(this)
					}
					else{
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

		finished()
	}
}
