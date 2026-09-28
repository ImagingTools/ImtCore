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

	function toJson(){
		let json = '['
		for(let i = 0; i < count; i++){
			let item = get(i).item
			if (i > 0){
				json += ','
			}
			if (item === null || item === undefined){
				json += 'null'
				continue
			}

			let list = getProperties(item)
			for (let propertyId of list){
				if (item.isArrayValueValid && !item.isArrayValueValid(propertyId, item[propertyId])){
					return ''
				}
			}
			json += '{'
			for(let j = 0; j < list.length; j++){
				let key = list[j]

				if (item[key] == null){
					json += '"' + item.getJSONKeyForProperty(key) + '": null'
				}
				else if(typeof item[key] === 'object'){
					if (Array.isArray(item[key])){
						json += '"' + item.getJSONKeyForProperty(key) + '":'

						json += "["

						for (let k = 0; k < item[key].length; k++){
							if (k != 0){
								json += ", "
							}

							if (typeof item[key][k] === "string"){
								json += "\"" + item[key][k] + "\""
							}
							else{
								json += item[key][k]
							}
						}

						json += "]"
					}
					else if (typeof item[key].toJson === "function"){
						let serializedValue = item[key].toJson()
						if (serializedValue === ''){
							return ''
						}
						json += '"' + item.getJSONKeyForProperty(key) + '":' + serializedValue
					}
				} else {
					let value = item[key]
					if (value === undefined){
						value = null
					}
					let safeValue = item[key]
					if (typeof safeValue === 'string'){
						safeValue = safeValue.replace(/\\/g, '\u005C\u005C')
						safeValue = safeValue.replace(/\"/g,'\u005C"')
					}

					json += '"' + item.getJSONKeyForProperty(key) + '":' + (typeof item[key] === 'string' ? '"' + safeValue + '"' : value)
				}
				if(j < list.length - 1) json += ','
			}
			json +='}'

		}
		json +=']'
		return json
	}

	function toGraphQL(){
		let graphQL = '['
		for(let i = 0; i < count; i++){
			let item = get(i).item
			if (i > 0){
				graphQL += ','
			}
			if (item === null || item === undefined){
				graphQL += 'null'
				continue
			}
			let list = getProperties(item)
			for (let propertyId of list){
				if (item.isArrayValueValid && !item.isArrayValueValid(propertyId, item[propertyId])){
					return ''
				}
			}

			graphQL += '{'
			for(let j = 0; j < list.length; j++){
				let key = list[j]
				if(item[key] === null){
					graphQL += item.getJSONKeyForProperty(key) + ':null'
				}
				else if(typeof item[key] === 'object'){
					if (Array.isArray(item[key])){
						graphQL +=  item.getJSONKeyForProperty(key) + ':'

						graphQL += "["

						for (let k = 0; k < item[key].length; k++){
							if (k != 0){
								graphQL += ", "
							}

							if (typeof item[key][k] === "string"){
								let data = item[key][k]

								data = data.replace(/\\/g, "\\\\")
								data = data.replace(/\"/g, "\\\"")
								data = data.replace(/\r/g, "\\r")
								data = data.replace(/\n/g, "\\n")
								data = data.replace(/\t/g, "\\t")

								graphQL += "\"" + data + "\""
							}
							else{
								graphQL += item[key][k]
							}
						}

						graphQL += "]"
					}
					else{
						let serializedValue = item[key].toGraphQL()
						if (serializedValue === ''){
							return ''
						}
						graphQL += item.getJSONKeyForProperty(key) + ':' + serializedValue
					}
				} else {
					let value = item[key]

					if(typeof value === 'string'){
						value = value.replace(/\\/g, "\\\\")
						value = value.replace(/\"/g, "\\\"")
						value = value.replace(/\r/g, "\\r")
						value = value.replace(/\n/g, "\\n")
						value = value.replace(/\t/g, "\\t")
					}

					if (value === undefined){
						value = null
					}
					graphQL += item.getJSONKeyForProperty(key) + ':' + (typeof item[key] === 'string' ? '"' + value + '"' : value)
				}
				if(j < list.length - 1) graphQL += ','
			}
			graphQL +='}'

		}
		graphQL +=']'
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


