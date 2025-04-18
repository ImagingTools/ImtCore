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
				if(typeof item[key] === 'object'){
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
								graphQL += "\"" + item[key][k] + "\""
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
					if (value === undefined){
						value = null
					}
					graphQL += item.getJSONKeyForProperty(key) + ':' + (typeof item[key] === 'string' ? '"' + item[key] + '"' : value)
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

	function addElement(element){
		element.owner = this.owner
		element.connectProperties()
		this.append({item: element})
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
}


