const AbstractItemModel = require("./AbstractItemModel")
const JSONListModel = require("./JSONListModel")
const QtObject = require("../QtObject")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")

class TreeItemModel extends JSONListModel {
    static meta = Object.assign({}, JSONListModel.meta, {
        infoPath: {type:Var, value:undefined, signalName:'infoPathChanged'},
        queryParams: {type:Var, value:undefined, signalName:'queryParamsChanged'},
        baseUrl: {type:String, value:'', signalName:'baseUrlChanged'},
        isArray: {type:Bool, value:false, signalName:'isArrayChanged'},
        isUpdateEnabled: {type:Bool, value:true, signalName:'isUpdateEnabledChanged'},

        infoPathChanged: {type:Signal, slotName:'onInfoPathChanged', args:[]},
        queryParamsChanged: {type:Signal, slotName:'onQueryParamsChanged', args:[]},
        baseUrlChanged: {type:Signal, slotName:'onBaseUrlChanged', args:[]},
        isArrayChanged: {type:Signal, slotName:'onIsArrayChanged', args:[]},
        isUpdateEnabledChanged: {type:Signal, slotName:'onIsUpdateEnabledChanged', args:[]},

        modelChanged: {type:Signal, slotName:'onModelChanged', args:['changeset']},
    })

    __m_countChanges = 0
    __m_countTransaction = 0
    __parentModel = null

    __propogate(){
        this.dataChanged()
        this.modelChanged()

        if(this.__parentModel){
            this.__parentModel.__propogate()
        }
    }

    __beginChanges(){
        this.__m_countTransaction++

        return true
    }

    __endChanges(){
        this.__m_countTransaction--

        if (this.__m_countChanges > 0 && this.__m_countTransaction === 0){
            this.modelChanged()

            this.__m_countChanges = 0

            return true
        }

        return false
    }

    append(dict){
        let copied = []

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            
            for(let i = 0; i < dict.length; i++){
                if(typeof dict[i] === 'object' && !Array.isArray(dict[i])){
                    copied.push(dict[i])
                } else {
                    copied.push({'': dict[i]})
                }
            }
		} else {
            copied = dict
        }

        super.append(copied)
    }

    insert(index, dict){
        let copied = []

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            
            for(let i = 0; i < dict.length; i++){
                if(typeof dict[i] === 'object' && !Array.isArray(dict[i])){
                    copied.push(dict[i])
                } else {
                    copied.push({'': dict[i]})
                }
            }
		} else {
            copied = dict
        }

        super.insert(index, copied)
    }

    setUpdateEnabled(flag){
        this.isUpdateEnabled = flag
    }

    getItemsCount(){
        return this.count
    }

    getData(key, row){
        if(row === undefined || row === null) row = 0

        let modelObject = this.get(row)
        let retVal = modelObject ? modelObject[key] : undefined

        if (retVal === undefined || retVal === null)
            return retVal

        if(typeof retVal === 'object' && !(retVal instanceof QtObject)){
            let retModel = TreeItemModel.create()
            retModel.__parentModel = this

            modelObject[key] = retModel

            if(Object.keys(retVal).length) {
                if (Array.isArray(retVal)){
                    retModel.setIsArray(true)
                }

                retModel.append(retVal)
            }

            return  retModel
        }

        return retVal
    }


    setData(key, value, row){
        JQApplication.updateLater(this)
        
        if(row === undefined || row === null) row = 0

        if(this.count === 0 && row === 0){
            this.append({})
        }

        let modelObject = this.get(row)

        if (modelObject[key] === undefined && value === ""){
            if (modelObject[key] != value){
                this.__m_countChanges++
            }

            modelObject[key] = value

            this.__endChanges()
            return true
        }

        if(modelObject[key] !== value){
            this.__m_countChanges++

            modelObject[key] = value
            

            if(this.isUpdateEnabled){
                this.dataChanged(row, row+1)
            }
        }

        return true
    }


    removeData(key, row){
        if(row === undefined || row === null) row = 0

        let modelObject = this.get(row)

        if(key in modelObject){
            delete modelObject[key]
        }

        this.modelChanged()
    }

    isValidData(key, row){
        let data = this.getData(key, row)
        let retVal = (data === undefined || data === null) ? false : true
        return retVal
    }

    setExternTreeModel(key, value, row){
        this.setData(key, value, row)
    }

    copyItemDataFromModel(index, externTreeModel, externIndex){
        this.removeItem(index)
        this.insertNewItem(index)

        let retVal = true
        let keys = externTreeModel.getKeys(externIndex)

        for(let key of keys){
            let value = externTreeModel.getData(key, externIndex)

            if (typeof value === 'object' && value instanceof TreeItemModel){
                let childModel = this.addTreeModel(key, index)

                retVal = retVal && childModel.copyFrom(value)
            } else {
                retVal = retVal && this.setData(key, value, index)
            }

            if (!retVal){
                break
            }
        }

        if (this.isUpdateEnabled){
            this.dataChanged()
        }

        return retVal
    }

    clear(){
        super.clear()
    }

    copyFrom(externTreeModel){
        if(externTreeModel){
            this.clear()

            let retVal = true

            for(let index = 0; index < externTreeModel.getItemsCount(); index++){
                this.insertNewItem(index)

                let keys = externTreeModel.getKeys(index)

                for(let key of keys){
                    let value = externTreeModel.getData(key, index)

                    if (typeof value === 'object' && value instanceof TreeItemModel){
                        let childModel = this.addTreeModel(key, index)

                        retVal = retVal && childModel.copyFrom(value)
                    } else {
                        retVal = retVal && this.setData(key, value, index)
                    }

                    if (!retVal){
                        break
                    }
                }
            }

            return retVal
        }

        return false
    }

    copyItemDataToModel(index, model, externIndex){
        if(externIndex === undefined) externIndex = 0
        
        let keys = this.getKeys(index)

        for(let key in keys){
            let value = this.getData(keys[key], index)
            model.setData(keys[key], value, externIndex)
        }
    }

    getModelFromItem(index){
        if(this.count <= 0 || index < 0 || index >= this.count) return

        let item = this.get(index)
        let retModel = TreeItemModel.create()
        retModel.__parentModel = this
        this.copyItemDataToModel(index, retModel, 0)
        return retModel

    }

    isEqualWithModel(externModel){
        if(!externModel) return false
        
        return this.isEqual(externModel)
    }

    isEqual(sourceModel){
        if(sourceModel) {
            if(this.getItemsCount() !== sourceModel.getItemsCount()){
                return false
            }

            for(let i = 0; i < sourceModel.getItemsCount(); ++i){
                let item = this.getModelFromItem(i)
                let sourceItem = sourceModel.getModelFromItem(i)

                let itemKeys = item.getKeys()
                let sourceItemKeys = sourceItem.getKeys()

                if(itemKeys.length !== sourceItemKeys.length){
                    return false
                }

                for(let key in sourceItemKeys){
                    if(!itemKeys.includes(sourceItemKeys[key])){
                        return false
                    }

                    let itemValue = item.getData(sourceItemKeys[key])
                    let sourceValue = sourceItem.getData(sourceItemKeys[key])

                    if(typeof sourceValue === 'object'){
                        if(typeof itemValue !== 'object'){
                            return false
                        }

                        let result = sourceValue.isEqual(itemValue)

                        if(!result){
                            return false
                        }
                    }
                    else if(sourceValue !== itemValue){
                        return false
                    }
                }
            }

            return true
        }

        return false
    }

    copy(obj){
        if(!obj) return false

        this.clear()
        for(let i = 0; i < obj.count; i++){
            this.copyItemDataFromModel(this.insertNewItem(), obj, i)
        }

        return true
    }

    copyMe(){
        let retModel = TreeItemModel.create()
        retModel.__parentModel = this

        if (!retModel.copy(this)){
            return null
        }

        return retModel
    }

    getKeys(index){
        if (index === undefined || index === null) index = 0

        let keys = []
        if(this.count > 0){
            let modelObject = this.get(index)
            for(let key in modelObject){
                keys.push(key)
            }
        }
        return keys
    }


    containsKey(key, row){
        if(row === undefined || row === null) row = 0
        if(this.count > row) return this.get(row).hasOwnProperty(key)
        return false
    }


    setQueryParam(key, value){
        this.queryParams[key] = value
    }


    needsReload(){
        let newSource = this.baseUrl
        let first = true

        for (let queryKey in this.queryParams){
            if (first)
                newSource += "?"
            else
                newSource += "&"
            first = false
            newSource += queryKey + "="
            newSource += this.queryParams[queryKey]
        }

        this.source = newSource
    }


    refresh(){

    }


    setIsArray(value){
        this.isArray = value
    }


    insertNewItem(index){
        if(index !== undefined && index !== null && typeof index === 'number'){
            this.insert(index, {})

            return index
        } else {
            this.append({})

            return this.count - 1
        }
    }

    insertNewItemWithParameters(index, parameters){
        this.append(parameters)

        return this.count - 1
    }

    removeItem(index){
        this.remove(index)
    } 

    isTreeModel(key, index){
        return this.getTreeItemModel(key, index) != null
    }

    getTreeItemModel(key, index){
        let data = this.getData(key, index)
        if(data instanceof TreeItemModel) return data

        return null
    }

    addTreeModel(key, row){
        let retModel = TreeItemModel.create()
        retModel.__parentModel = this

        this.setUpdateEnabled(false)
        this.setData(key, retModel, row)
        this.setUpdateEnabled(true)

        return retModel
    }

    createFromJson(jsonString){
        this.clear()
        this.json = jsonString

        this.updateJSONModel()
        this.updateTreeItemJSONModel()
    }

    toJson(){
        let retVal = ""
        if (this.isArray || this.count > 1)
            retVal += "["
        else
            retVal += "{"
        for (let i = 0; i < this.count; i++){
            let modelObject = this.get(i)
            if (i > 0){
                retVal += ","
            }
            if (this.isArray || this.count > 1)
                if(modelObject && Object.keys(modelObject).indexOf('') < 0) retVal += "{"

            let recurciveJSON = function(modelData){
                if (modelData === null || modelData === undefined) {
                    retVal += "null"
                } else if(typeof modelData === 'object'){
                    if(modelData instanceof AbstractItemModel){
                        let j = 0
                        for (let property in modelObject) {
                            if (j > 0) retVal += ","
                            j++;
                            if(property !== '') retVal += "\"" + property + "\":"

                            recurciveJSON(modelData[property])
                        }
                    } else if(modelData instanceof TreeItemModel){
                        retVal += modelData.toJson()
                    } else if(modelData instanceof QtObject){
                        retVal += "null"
                    } else {
                        retVal += JSON.stringify(modelData)
                    }
                } else if(typeof modelData === 'string'){
                    retVal += "\"" + modelData.replaceAll('\u005C', '\u005C\u005C') + "\""
                } else {
                    retVal += modelData
                }
            }

            recurciveJSON(modelObject)

            if (this.isArray || this.count > 1){
                if(modelObject && Object.keys(modelObject).indexOf('') < 0) retVal += "}"
            }
                
        }

        if (this.isArray || this.count > 1)
            retVal += "]"
        else
            retVal += "}"
        return retVal
    }

    updateTreeItemJSONModel(){
        for(let row = 0; row < this.getItemsCount(); row++){
            let modelObject = this.get(row)
            let keys = Object.keys(modelObject)
            for(let index in keys){
                let retVal = modelObject[keys[index]]
                if(retVal !== null && typeof retVal === 'object' && !(retVal instanceof QtObject)){
                    let retModel = TreeItemModel.create()
                    retModel.__parentModel = this

                    modelObject[keys[index]] = retModel

                    if(Object.keys(retVal).length) {
                        if (Array.isArray(retVal)){
                            retModel.setIsArray(true)
                        }

                        retModel.append(retVal)
                    }
                    retVal = retModel
                    retVal.updateTreeItemJSONModel()
                }
                this.setData(keys[index],retVal,row)
            }
        }
    }
}

module.exports = TreeItemModel