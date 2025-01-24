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
            if (key[0] === 'm' && key[1] === '_') {
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

    addElement(element){
        element.owner = this.owner
        element.connectProperties()
        this.append({item: element})
        if (this.owner){
            this.owner.modelChanged([])
        }
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
}

module.exports.BaseModel = BaseModel
