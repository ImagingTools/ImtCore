const ListModel = require('./Models/ListModel')
const Bool = require('./Bool')
const Var = require('./Var')
const Signal = require('./Signal')
const QtFunctions = require("../Qt/functions")

class BaseModel extends ListModel {
	static meta = Object.assign({}, ListModel.meta, {
        dynamicRoles: { type: Bool, value: true },
		owner: { type: Var, value: null },

		internalModelChanged: { type:Signal, args: ['name', 'sender'] },
		finished: { type:Signal, args: [] },
    })

	SLOT_ownerChanged(){
		for(let i = 0; i < this.count; i++){
			this.get(i).item.owner = this.owner
		}
	}

	escapeSpecialChars(jsonString) {
		return jsonString.replace(/\\/g, "\\\\")
		.replace(/\"/g, "\\\"")
		.replace(/\n/g, "\\n")
		.replace(/\r/g, "\\r")
		.replace(/\t/g, "\\t")
		.replace(/\f/g, "\\f")
	}

	getProperties(item){
		return item.getProperties()
	}

	toJson(){
		let json = '['
		for(let i = 0; i < this.count; i++){
			let item = this.get(i).item
			let list = this.getProperties(item)

			json += '{'
			let j = 0
			for(let key of list){
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
								json += "\"" + this.escapeSpecialChars(item[key][k]) + "\""
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
						safeValue = this.escapeSpecialChars(safeValue)
					}

					json += '"' + item.getJSONKeyForProperty(key) + '":' + (typeof item[key] === 'string' ? '"' + safeValue + '"' : value)
				}
				if(j < list.size - 1) json += ','
				j++
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
			let j = 0
			for(let key of list){
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
								graphQL += "\"" + this.escapeSpecialChars(item[key][k]) + "\""
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

					graphQL += item.getJSONKeyForProperty(key) + ':' + (typeof item[key] === 'string' ? '"' + this.escapeSpecialChars(item[key]) + '"' : value)
				}
				if(j < list.size - 1) graphQL += ','
				j++
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
		let retVal = BaseModel.create()
		if (!retVal){
			return null
		}
		
		for(let i = 0; i < this.count; i++){
			let item = this.get(i).item
			retVal.addElement(item.copyMe())
		}
		
		return retVal
	}

	createFromJson(json){
		return this.fromJSON(json);
	}

	fromJSON(json){
		this.clear()

		let arr = JSON.parse(json)
		for(let i = 0; i < arr.length; i++){
			let sourceTypename
			if (arr[i]['__typename']){
				sourceTypename = arr[i]['__typename']
			}
			else {
				continue
			}
			let obj = QtFunctions.createComponent(sourceTypename + ".qml").createObject(this)
			obj.fromObject(arr[i])
			this.addElement(obj)
		}

		this.finished()
	}

	addElement(element){
		element.owner = this.owner
		this.append({item: element})
		if (this.owner){
			if (this.owner._internal && this.owner._internal.isTransaction){
				this.owner._internal.countChanges++
			} else {
				this.owner.modelChanged([])
			}
		}
	}

	removeElement(index){
		this.remove(index)
		if (this.owner){
			if (this.owner._internal && this.owner._internal.isTransaction){
				this.owner._internal.countChanges++
			} else {
				this.owner.modelChanged([])
			}
		}
	}

	getItemsCount(){
		return this.count
	}

	containsKey(key, index){
		return this.get(index).item[key] != undefined
	}

	getData(key, index){
		return this.get(index).item[key]
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
		this.insert(index, {item: element})
		if (this.owner){
			if (this.owner._internal && this.owner._internal.isTransaction){
				this.owner._internal.countChanges++
			} else {
				this.owner.modelChanged([])
			}
		}
	}
}

module.exports = BaseModel
