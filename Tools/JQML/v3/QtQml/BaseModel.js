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

	hasNullElements(){
		for(let i = 0; i < this.count; i++){
			let item = this.get(i).item
			if (item === null || item === undefined){
				return true
			}
		}

		return false
	}

	// Returns '' if a required array of an item is null or has null elements; scalars are not validated
	toJson(){
		let json = '['
		for(let i = 0; i < this.count; i++){
			if (i > 0) json += ','

			let item = this.get(i).item
			if (item === null || item === undefined){
				json += 'null'
				continue
			}

			let separator = '{'
			for(let key of this.getProperties(item)){
				let value = item[key]
				let serializedValue
				if (value == null){
					if (!item.isArrayValueValid(key, value)){
						return ''
					}

					serializedValue = ' null'
				}
				else if(typeof value === 'object'){
					if (!item.isArrayValueValid(key, value)){
						return ''
					}

					if (Array.isArray(value)){
						serializedValue = "["
						for (let k = 0; k < value.length; k++){
							if (k != 0){
								serializedValue += ", "
							}

							serializedValue += (typeof value[k] === "string") ? "\"" + this.escapeSpecialChars(value[k]) + "\"" : value[k]
						}
						serializedValue += "]"
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
					serializedValue = (typeof value === 'string') ? '"' + this.escapeSpecialChars(value) + '"' : value
				}

				json += separator + '"' + item.getJSONKeyForProperty(key) + '":' + serializedValue
				separator = ','
			}
			json += (separator === '{') ? '{}' : '}'
		}
		json +=']'
		return json
	}

	// Returns '' if a required array of an item is null or has null elements; scalars are not validated
	toGraphQL(){
		let graphQL = '['
		for(let i = 0; i < this.count; i++){
			if (i > 0) graphQL += ','

			let item = this.get(i).item
			if (item === null || item === undefined){
				graphQL += 'null'
				continue
			}

			let separator = '{'
			for(let key of this.getProperties(item)){
				let value = item[key]
				let serializedValue
				if (value == null){
					if (!item.isArrayValueValid(key, value)){
						return ''
					}

					serializedValue = 'null'
				}
				else if(typeof value === 'object'){
					if (!item.isArrayValueValid(key, value)){
						return ''
					}

					if (Array.isArray(value)){
						serializedValue = "["
						for (let k = 0; k < value.length; k++){
							if (k != 0){
								serializedValue += ", "
							}

							serializedValue += (typeof value[k] === "string") ? "\"" + this.escapeSpecialChars(value[k]) + "\"" : value[k]
						}
						serializedValue += "]"
					}
					else{
						serializedValue = value.toGraphQL()
						if (serializedValue === ''){
							return ''
						}
					}
				}
				else{
					serializedValue = (typeof value === 'string') ? '"' + this.escapeSpecialChars(value) + '"' : value
				}

				graphQL += separator + item.getJSONKeyForProperty(key) + ':' + serializedValue
				separator = ','
			}
			graphQL += (separator === '{') ? '{}' : '}'
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

	copyFrom(sourceObject) {
		for(let i = 0; i < sourceObject.count; i++){
			let item = sourceObject.get(i).item
			this.addElement(item.copyMe())
		}

		return true
	}

	createFromJson(json){
		return this.fromJSON(json);
	}

	fromJSON(json){
		let arr = JSON.parse(json)
		return this.fromObject(arr)
	}

	fromObject(sourceObject){
		this.clear()

		for(let i = 0; i < sourceObject.length; i++){
			let sourceTypename
			if (sourceObject[i]['__typename']){
				sourceTypename = sourceObject[i]['__typename']
			}
			else {
				continue
			}
			let obj = QtFunctions.createComponent(sourceTypename + ".qml").createObject(this)
			obj.fromObject(sourceObject[i])
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
		if (index < 0 || index >= this.count){
			return false
		}
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
