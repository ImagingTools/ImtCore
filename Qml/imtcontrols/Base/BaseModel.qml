import QtQuick 2.0

ListModel {
	property var owner: null
	dynamicRoles: true;

	function getProperties(item){
		let list = []
		if(Qt.platform.os === 'web'){
			for(let key in item.$properties){
				if(
						key.indexOf('m_') >= 0
						&& typeof item[key] !== "function"
						&& item[key] !== undefined
						&& item[key] !== null){
					list.push(key)
				}
			}
		} else {
			for(let key in item){
				if(
						key.indexOf('m_') >= 0
						&& typeof item[key] !== "function"
						&& item[key] !== undefined
						&& item[key] !== null){
					list.push(key)
				}
			}
		}

		return list
	}

	function toJson(){
		let json = '['
		for(let i = 0; i < count; i++){
			let item = get(i).item

			let list = getProperties(item)
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
						json += '"' + item.getJSONKeyForProperty(key) + '":' + item[key].toJson()
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

			if(i < count - 1) json += ','
		}
		json +=']'
		return json
	}

	function toGraphQL(){
		let graphQL = '['
		for(let i = 0; i < count; i++){
			let item = get(i).item
			let list = getProperties(item)

			graphQL += '{'
			for(let j = 0; j < list.length; j++){
				let key = list[j]
				if(typeof item[key] === 'object'){
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
								data = data.replace(/\r/g, "\\\\r")
								data = data.replace(/\n/g, "\\\\n")
								data = data.replace(/\t/g, "\\\\t")

								graphQL += "\"" + data + "\""
							}
							else{
								graphQL += item[key][k]
							}
						}

						graphQL += "]"
					}
					else{
						graphQL += item.getJSONKeyForProperty(key) + ':' + item[key].toGraphQL()
					}
				} else {
					let value = item[key]

					if(typeof value === 'string'){
						value = value.replace(/\\/g, "\\\\")
						value = value.replace(/\"/g, "\\\"")
						value = value.replace(/\r/g, "\\\\r")
						value = value.replace(/\n/g, "\\\\n")
						value = value.replace(/\t/g, "\\\\t")
					}

					if (value === undefined){
						value = null
					}
					graphQL += item.getJSONKeyForProperty(key) + ':' + (typeof item[key] === 'string' ? '"' + value + '"' : value)
				}
				if(j < list.length - 1) graphQL += ','
			}
			graphQL +='}'

			if(i < count - 1) graphQL += ','
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
					let ok = item1[key].isEqualWithModel(item2[key])
					if (!ok){
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
			retVal.addElement(item.copyMe())
		}
		
		return retVal
	}

	function addElement(element){
		insertElement(this.count, element)
	}

	function insertElement(index, element){
		element.owner = this.owner
		element.connectProperties()
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

		return this.get(index).item[key] !== undefined;
	}

	function getData(key, index){
		if(index === undefined){
			index = 0
		}

		if (this.get(index) === undefined){
			return undefined
		}

		return this.get(index).item[key];
	}
	
	function setProperty(index, propName, value){
		let item = get(index).item
		if (item[propName] !== value){
			item[propName] = value
		}
	}

	function swapItems(index1, index2){
		if (index1 < 0 || index1 >= this.count || index2 < 0 || index2 >= this.count){
			return false
		}
		
		let item1 = this.get(index1).item.copyMe()
		let item2 = this.get(index2).item.copyMe()
		item1.owner = this.owner
		item2.owner = this.owner

		this.get(index1).item = item2
		this.get(index2).item = item1

		return true
	}
}


