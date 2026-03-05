const { ListModel } = require('./ListModel')
const { QBool, QVar } = require('../utils/properties')

class BaseModel extends ListModel {
	static defaultProperties = {
		dynamicRoles: { type: QBool, value: true },
		owner: { type: QVar, value: null },
	}

	static defaultSignals = {
		internalModelChanged: { params: ['name', 'sender'] },
	}

	getProperties(item){
		let list = []

		for (let key in item.$properties) {
			if (key[0] === 'm' && key[1] === '_' || key == '__typename') {
				list.push(key)
			}
		}

		return list
	}

	toJson(){
		let json = '['
		for(let i = 0; i < this.count; i++){
			let item = this.get(i).item
			let list = this.getProperties(item)

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

			if(i < this.count - 1) json += ','
		}
		json +=']'
		return json
	}

	toGraphQL(){
		let graphQL = '['
		for(let i = 0; i < this.count; i++){
			let item = this.get(i).item
			let list = this.getProperties(item)

			graphQL += '{'
			for(let j = 0; j < list.length; j++){
				let key = list[j]
				if (item[key] == null){
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

			if(i < this.count - 1) graphQL += ','
		}
		graphQL +=']'
		return graphQL
	}

	isEqualWithModel(model){
		if (typeof this != typeof model){
			return false;
		}

		if (this.count !== model.count){
			return false;
		}

		for(let i = 0; i < this.count; i++){
			let item1 = this.get(i).item
			let item2 = model.get(i).item

			let list1 = this.getProperties(item1)
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
	
	copyMe(){
		let retVal = new BaseModel()
		if (!retVal){
			return null
		}
		
		for(let i = 0; i < this.count; i++){
			let item = this.get(i).item
			retVal.addElement(item.copyMe())
		}
		
		return retVal
	}

	addElement(element){
		this.insertElement(this.count, element)
	}

	removeElement(index){
		this.remove(index)
		if (this.owner){
			this.owner.modelChanged([])
		}
	}

	getItemsCount(){
		return this.count;
	}

	containsKey(key, index){
		return this.get(index).item[key] != undefined;
	}

	getData(key, index){
		return this.get(index).item[key];
	}
	
	setProperty(index, propName, value){
		let item = this.get(index).item
		if (item[propName] !== value){
			item[propName] = value
		}
	}
	
	swapItems(index1, index2){
		if (index1 < 0 || index1 >= this.count || index2 < 0 || index2 >= this.count ){
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

	insertElement(index, element){
		element.owner = this.owner
		element.connectProperties()
		this.insert(index, {item: element})
		if (this.owner){
			this.owner.modelChanged([])
		}
	}
}

module.exports.BaseModel = BaseModel
