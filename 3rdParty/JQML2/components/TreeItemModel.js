const { JSONListModel } = require('./JSONListModel')
const { QString, QBool, QVar, QModelData, QProperty, QLinkedBool } = require('../utils/properties')

class TreeItemModel extends JSONListModel {
    static defaultProperties = {
        infoPath: { type: QVar, value: undefined },
        queryParams: { type: QVar, value: undefined },
        baseUrl: { type: QString, value: '' },
        isArray: { type: QBool, value: false },
        isUpdateEnabled: { type: QLinkedBool, value: true },
    }

    static defaultSignals = {
        modelChanged: { params: [] },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }

    $emitDataChanged(topLeft, bottomRight, roles){
        if(this.getPropertyValue('isUpdateEnabled')) super.$emitDataChanged(topLeft, bottomRight, roles)
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

    addResource(resource){
        super.addResource(resource)

        if(resource instanceof TreeItemModel){
            resource.getProperty('isUpdateEnabled').setOriginCompute(()=>{resource.getProperty('isUpdateEnabled').subscribe(this.getProperty('isUpdateEnabled')); return this.getProperty('isUpdateEnabled').get()})
            resource.getProperty('isUpdateEnabled').update()
        }
    }

    SetUpdateEnabled(flag){
        this.getProperty('isUpdateEnabled').reset(flag)
    }

    GetItemsCount(){
        return this.count
    }

    GetData(key, row){
        if(row === undefined || row === null) row = 0

        let modelObject = this.get(row)
        let retVal = modelObject ? modelObject[key] : undefined

        if (retVal === undefined)
            return undefined

        if(typeof retVal === 'object' && !(retVal instanceof QtObject)){
            let retModel = new TreeItemModel(this)
            if(typeof modelObject === 'object' && modelObject instanceof QModelData){
                modelObject.$lock = true
                modelObject[key] = retModel
                modelObject.$lock = false
            } else {
                modelObject[key] = retModel
            }

            if(Object.keys(retVal).length) {
                if (Array.isArray(retVal)){
                    retModel.SetIsArray(true)
                }

                retModel.append(retVal)
            }

            return  retModel
        }

        return retVal
    }


    SetData(key, value, row){
        if(row === undefined || row === null) row = 0

        if(this.count === 0 && row === 0){
            this.append({})
        }

        let modelObject = this.get(row)

        if (modelObject[key] === undefined && value === ""){
            modelObject[key] = value

            return true
        }

        if(modelObject[key] !== value){
            if(typeof modelObject === 'object' && modelObject instanceof QModelData){
                modelObject.$lock = true
                modelObject[key] = value
                modelObject.$lock = false
            } else {
                modelObject[key] = value
            }

            if (this.getPropertyValue('isUpdateEnabled')){
                let signal = this.getProperty('data').getNotify()
                if(signal) signal(row, row+1)
            }
        }

        return true
    }


    RemoveData(key, row){
        if(row === undefined || row === null) row = 0

        let modelObject = this.get(row)

        if(key in modelObject){
            if(modelObject[key] && modelObject[key] instanceof QProperty) modelObject[key].unsubscribe()
            delete modelObject[key]
        }

        if (this.getPropertyValue('isUpdateEnabled')){
            let signal = this.getSignal('modelChanged')
            if(signal) signal()
        }
    }

    Clear(){
        this.clear()
    }

    IsValidData(key, row){
        let data = this.GetData(key, row)
        let retVal = (data === undefined || data === null) ? false : true
        return retVal
    }

    SetExternTreeModel(key, value, row){
        this.SetData(key, value, row)
    }

    CopyItemDataFromModel(index, externTreeModel, externIndex){
        this.RemoveItem(index)
        this.InsertNewItem(index)

        let retVal = true
        let keys = externTreeModel.GetKeys(externIndex)

        for(let key of keys){
            let value = externTreeModel.GetData(key, externIndex)

            if (typeof value === 'object' && value instanceof TreeItemModel){
                let childModel = this.AddTreeModel(key, index)

                retVal = retVal && childModel.CopyFrom(value)
            } else {
                retVal = retVal && this.SetData(key, value, index)
            }

            if (!retVal){
                break
            }
        }

        if (this.getPropertyValue('isUpdateEnabled')){
            let signal = this.getProperty('data').getNotify()
            if(signal) signal()
        }

        return retVal
    }

    CopyFrom(externTreeModel){
        if(externTreeModel){
            this.Clear()

            let retVal = true

            for(let index = 0; index < externTreeModel.GetItemsCount(); index++){
                this.InsertNewItem(index)

                let keys = externTreeModel.GetKeys(index)

                for(let key of keys){
                    let value = externTreeModel.GetData(key, index)

                    if (typeof value === 'object' && value instanceof TreeItemModel){
                        let childModel = this.AddTreeModel(key, index)

                        retVal = retVal && childModel.CopyFrom(value)
                    } else {
                        retVal = retVal && this.SetData(key, value, index)
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

    CopyItemDataToModel(index, model, externIndex){
        if(externIndex === undefined) externIndex = 0
        
        let keys = this.GetKeys(index)

        for(let key in keys){
            let value = this.GetData(keys[key], index)
            model.SetData(keys[key], value, externIndex)
        }
    }

    GetModelFromItem(index){
        if(this.count <= 0 || index < 0 || index >= this.count) return

        let item = this.get(index)
        let retModel = new TreeItemModel()
        this.CopyItemDataToModel(index, retModel, 0)
        return retModel

    }

    IsEqualWithModel(externModel){
        if(!externModel) return false
        
        return this.IsEqual(externModel)
    }

    IsEqual(sourceModel){
        if(sourceModel) {
            if(this.GetItemsCount() !== sourceModel.GetItemsCount()){
                return false
            }

            for(let i = 0; i < sourceModel.GetItemsCount(); ++i){
                let item = this.GetModelFromItem(i)
                let sourceItem = sourceModel.GetModelFromItem(i)

                let itemKeys = item.GetKeys()
                let sourceItemKeys = sourceItem.GetKeys()

                if(itemKeys.length !== sourceItemKeys.length){
                    return false
                }

                for(let key in sourceItemKeys){
                    if(!itemKeys.includes(sourceItemKeys[key])){
                        return false
                    }

                    let itemValue = item.GetData(sourceItemKeys[key])
                    let sourceValue = sourceItem.GetData(sourceItemKeys[key])

                    if(typeof sourceValue === 'object'){
                        if(typeof itemValue !== 'object'){
                            return false
                        }

                        let result = sourceValue.IsEqual(itemValue)

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

    Copy(obj){
        if(!obj) return false

        this.clear()
        for(let i = 0; i < obj.count; i++){
            this.CopyItemDataFromModel(this.InsertNewItem(), obj, i)
        }
        return true
    }

    CopyMe(){
        let retModel = new TreeItemModel()

        if (!retModel.Copy(this)){
            return null
        }

        return retModel
    }

    GetKeys(index){
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


    ContainsKey(key, row){
        if(row === undefined || row === null) row = 0
        if(this.count > row) return this.get(row).hasOwnProperty(key)
        return false
    }


    SetQueryParam(key, value){
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


    Refresh(){

    }


    SetIsArray(value){
        this.isArray = value
    }


    InsertNewItem(index){
        if(index !== undefined && index !== null && typeof index === 'number'){
            this.insert(index, {})
            return index
        } else {
            this.append({})
            return this.count - 1
        }
    }

    InsertNewItemWithParameters(index, parameters){
        this.append(parameters)
        return this.count - 1
    }

    RemoveItem(index){
        this.remove(index)

        if (this.getPropertyValue('isUpdateEnabled')){
            this.modelChanged()
        }
    } 

    IsTreeModel(key, index){
        return this.GetTreeItemModel(key, index) != null
    }

    GetTreeItemModel(key, index){
        let data = this.GetData(key, index)
        if(data instanceof TreeItemModel) return data

        return null
    }

    AddTreeModel(key, row){
        let retModel = new TreeItemModel(this)
        this.SetUpdateEnabled(false)
        this.SetData(key, retModel, row)
        this.SetUpdateEnabled(true)
        return retModel
    }

    CreateFromJson(jsonString){
        this.Clear()
        this.json = jsonString

        this.updateJSONModel()
        this.updateTreeItemJSONModel()
    }

    ToJson(){
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
                    if(modelData instanceof QModelData){
                        let j = 0
                        for (let property in modelObject) {
                            if (j > 0) retVal += ","
                            j++;
                            if(property !== '') retVal += "\"" + property + "\":"

                            recurciveJSON(modelData[property])
                        }
                    } else if(modelData instanceof TreeItemModel){
                        retVal += modelData.ToJson()
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
        for(let row = 0; row < this.GetItemsCount(); row++){
            let modelObject = this.get(row)
            let keys = Object.keys(modelObject)
            for ( let index in keys ) {
                let retVal = modelObject[keys[index]]
                if(retVal !== null && typeof retVal === 'object' && !(retVal instanceof QtObject)){
                    let retModel = new TreeItemModel(this)
                    if(typeof modelObject === 'object' && modelObject instanceof QModelData){
                        modelObject.$lock = true
                        modelObject[keys[index]] = retModel
                        modelObject.$lock = false
                    } else {
                        modelObject[keys[index]] = retModel
                    }

                    if(Object.keys(retVal).length) {
                        if (Array.isArray(retVal)){
                            retModel.SetIsArray(true)
                        }

                        retModel.append(retVal)
                    }
                    retVal = retModel
                    retVal.updateTreeItemJSONModel()
                }
                this.SetData(keys[index],retVal,row)
            }
        }
    }
}

module.exports.TreeItemModel = TreeItemModel
