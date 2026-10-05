import QtQuick 2.0

ListModel {
	property var owner: null
	signal finished
	dynamicRoles: true;

	onOwnerChanged: {
		for (let i = 0; i < count; ++i){
			let item = get(i).item
			if (item !== null && item !== undefined){
				item.owner = owner
			}
		}
	}

	function getProperties(item){
		let list = []
		if (item === null || item === undefined){
			return list
		}
		if(Qt.platform.os === 'web'){
			for(let key in item.$properties){
				if(
						key.indexOf('m_') >= 0
						&& typeof item[key] !== "function"
						&& item[key] !== undefined
						&& item[key] !== null || key == '__typename'){
					list.push(key)
				}
			}
		} else {
			for(let key in item){
				if(
						key.indexOf('m_') >= 0
						&& typeof item[key] !== "function"
						&& item[key] !== undefined
						&& item[key] !== null || key == '__typename'){
					list.push(key)
				}
			}
		}

		return list
	}

	function hasNullElements(){
		for (let i = 0; i < count; ++i){
			let item = get(i).item
			if (item === null || item === undefined){
				return true
			}
		}

		return false
	}

	// The full list of an SDL item still contains null properties, they must be validated before skipping
	function getItemPropertyIds(item){
		return item.getProperties ? item.getProperties() : getProperties(item)
	}

	function createGraphQlString(value){
		if (!/[\\"\r\n\t\b\f]/.test(value)){
			return '"' + value + '"'
		}

		return '"' + value.replace(/\\/g, "\\\\").replace(/\"/g, "\\\"").replace(/\r/g, "\\r").replace(/\n/g, "\\n").replace(/\t/g, "\\t").replace(/[\b]/g, "\\b").replace(/\f/g, "\\f") + '"'
	}

	// Returns '' if a required array of an item is null or has null elements; scalars are not validated
	function toJson(){
		let json = '['
		for (let i = 0; i < count; i++){
			if (i > 0){
				json += ','
			}

			let item = get(i).item
			if (item === null || item === undefined){
				json += 'null'
				continue
			}

			let canValidate = typeof item.isArrayValueValid === "function"
			let separator = '{'
			for (let key of getItemPropertyIds(item)){
				let value = item[key]
				if (value === null || value === undefined){
					if (canValidate && !item.isArrayValueValid(key, value)){
						return ''
					}

					continue
				}

				let serializedValue
				if (typeof value === 'object'){
					if (canValidate && !item.isArrayValueValid(key, value)){
						return ''
					}

					if (Array.isArray(value)){
						serializedValue = '['
						for (let k = 0; k < value.length; k++){
							if (k != 0){
								serializedValue += ", "
							}

							serializedValue += (typeof value[k] === "string") ? JSON.stringify(value[k]) : value[k]
						}
						serializedValue += ']'
					}
					else if (typeof value.toJson === "function"){
						serializedValue = value.toJson()
						if (serializedValue === ''){
							return ''
						}
					}
					else{
						continue
					}
				}
				else{
					serializedValue = (typeof value === 'string') ? JSON.stringify(value) : value
				}

				json += separator + '"' + item.getJSONKeyForProperty(key) + '":' + serializedValue
				separator = ','
			}
			json += (separator === '{') ? '{}' : '}'
		}
		json += ']'

		return json
	}

	// Returns '' if a required array of an item is null or has null elements; scalars are not validated
	function toGraphQL(){
		let graphQL = '['
		for (let i = 0; i < count; i++){
			if (i > 0){
				graphQL += ','
			}

			let item = get(i).item
			if (item === null || item === undefined){
				graphQL += 'null'
				continue
			}

			let canValidate = typeof item.isArrayValueValid === "function"
			let separator = '{'
			for (let key of getItemPropertyIds(item)){
				let value = item[key]
				if (value === null || value === undefined){
					if (canValidate && !item.isArrayValueValid(key, value)){
						return ''
					}

					continue
				}

				let serializedValue
				if (typeof value === 'object'){
					if (canValidate && !item.isArrayValueValid(key, value)){
						return ''
					}

					if (Array.isArray(value)){
						serializedValue = '['
						for (let k = 0; k < value.length; k++){
							if (k != 0){
								serializedValue += ", "
							}

							serializedValue += (typeof value[k] === "string") ? createGraphQlString(value[k]) : value[k]
						}
						serializedValue += ']'
					}
					else{
						serializedValue = value.toGraphQL()
						if (serializedValue === ''){
							return ''
						}
					}
				}
				else{
					serializedValue = (typeof value === 'string') ? createGraphQlString(value) : value
				}

				graphQL += separator + item.getJSONKeyForProperty(key) + ':' + serializedValue
				separator = ','
			}
			graphQL += (separator === '{') ? '{}' : '}'
		}
		graphQL += ']'

		return graphQL
	}

	function isEqualWithModel(model){
		if (typeof this != typeof model){
			return false;
		}

		if (count !== model.count){
			return false;
		}

		for(let i = 0; i < count; i++){
			let item1 = get(i).item
			let item2 = model.get(i).item
			let item1IsNull = item1 === null || item1 === undefined
			let item2IsNull = item2 === null || item2 === undefined
			if (item1IsNull !== item2IsNull){
				return false
			}
			if (item1IsNull){
				continue
			}

			let list1 = getProperties(item1)
			let list2 = model.getProperties(item2)

			for(let j = 0; j < list1.length; j++){
				let key = list1[j]

				if (!list2.includes(key)){
					return false;
				}

				if(typeof item1[key] !== typeof item2[key]){
					return false;
				}

				if(typeof item1[key] === 'object'){
					if (item1[key] && item1[key].isEqualWithModel){
						let ok = item1[key].isEqualWithModel(item2[key])
						if (!ok){
							return false
						}
					}
					else if (item1[key] !== item2[key]){
						return false
					}
				} else {
					if (item1[key] !== item2[key]){
						return false
					}
				}
			}
		}

		return true;
	}
	
	function copyMe(){
		let retVal = Qt.createComponent('BaseModel.qml').createObject()
		if (!retVal){
			console.debug("Unable to create copy for BaseModel. Error: Creating component is failed")
			return null
		}
		
		for(let i = 0; i < count; i++){
			let item = get(i).item
			retVal.addElement(item === null || item === undefined ? null : item.copyMe())
		}
		
		return retVal
	}

	function createFromJson(json){
		return fromJSON(json);
	}

	function fromJSON(json){
		this.clear()

		let arr = JSON.parse(json)
		for(let i = 0; i < arr.length; i++){
			if (arr[i] === null){
				this.addElement(null)
				continue
			}
			let sourceTypename
			if (arr[i]['__typename']){
				sourceTypename = arr[i]['__typename']
			}
			else {
				continue
			}
			let obj = Qt.createComponent(sourceTypename + ".qml").createObject(this)
			obj.fromObject(arr[i])
			this.addElement(obj)
		}

		finished()
	}

	/// \deprecated! OBSOLETE function ONLY for support legacy code DO NOT USE IT! Use \c appendElement() instead. Will be removed next releases.
	function addElement(element){
		insertElement(this.count, element)
	}
	
	function appendElement(element){
		insertElement(this.count, element)
	}

	function insertElement(index, element){
		if (element !== null && element !== undefined){
			element.owner = this.owner
			element.connectProperties()
		}
		this.insert(index, {item: element})
		if (owner){
			owner.modelChanged([])
		}
	}

	function removeElement(index){
		if(index === undefined){
			index = 0
		}

		this.remove(index)
		if (owner){
			owner.modelChanged([])
		}
	}

	function getItemsCount(){
		return this.count;
	}

	function containsKey(key, index){
		if(index === undefined){
			index = 0
		}

		let item = this.get(index).item
		return item !== null && item !== undefined && item[key] !== undefined;
	}

	function getData(key, index){
		if(index === undefined){
			index = 0
		}

		if (this.get(index) === undefined){
			return undefined
		}

		let item = this.get(index).item
		return item === null || item === undefined ? item : item[key];
	}
	
	function setProperty(index, propName, value){
		let item = get(index).item
		if (item === null || item === undefined){
			return
		}
		if (item[propName] !== value){
			item[propName] = value
		}
	}

	function swapItems(index1, index2){
		if (index1 < 0 || index1 >= this.count || index2 < 0 || index2 >= this.count){
			return false
		}
		
		let sourceItem1 = this.get(index1).item
		let sourceItem2 = this.get(index2).item
		let item1 = sourceItem1 === null || sourceItem1 === undefined ? null : sourceItem1.copyMe()
		let item2 = sourceItem2 === null || sourceItem2 === undefined ? null : sourceItem2.copyMe()
		if (item1 !== null){
			item1.owner = this.owner
		}
		if (item2 !== null){
			item2.owner = this.owner
		}

		this.get(index1).item = item2
		this.get(index2).item = item1

		return true
	}
}


