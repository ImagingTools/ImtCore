import QtQuick 2.0

JSONListModel {
    id: treeItemModelContainer;

    property var infoPath: [];
    property string baseUrl;
    property var queryParams: [];
    property bool isArray: false;
    property bool isUpdateEnabled: true;

    function SetUpdateEnabled(flag){
        isUpdateEnabled = flag
    }

    function GetItemsCount(){
        return this.count
    }

    function GetTreeItemModel(key, row){
        let model_ = GetData(key, row);
        return model_ && typeof model_ === 'object' && model_.constructor.name.indexOf('TreeItemModel') >= 0 ? model_ : null;
    }

    function GetData(key, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        var modelObject = this.get(row)
        var retVal = modelObject ? modelObject[key] : undefined

        if (retVal === undefined)
            return undefined

        if(typeof retVal === 'object' && !(retVal instanceof QtObject)){
            var retModel
            retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
            if(typeof modelObject === 'object' && modelObject.constructor.name === 'QModelData'){
                modelObject.$lock = true
                modelObject[key] = retModel
                modelObject.$lock = false
            } else {
                modelObject[key] = retModel
            }
            retModel.isUpdateEnabled = this.isUpdateEnabled
            retModel.getProperty('isUpdateEnabled').setCompute(function(){return this.isUpdateEnabled}.bind(this))
            retModel.getProperty('isUpdateEnabled').update()
            if(Object.keys(retVal).length) retModel.append(retVal);

            return  retModel
        }

        return retVal
    }


    function SetData(key, value, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        if(this.count === 0 && row === 0){
            this.append({})
        }

        var modelObject = this.get(row)

        if (modelObject === null)
            console.log("modelObject is null")

        if (modelObject[key] === undefined && value === ""){
            modelObject[key] = value

            return true
        }

        if(modelObject[key] !== value){
            if(typeof modelObject === 'object' && modelObject.constructor.name === 'QModelData'){
                modelObject.$lock = true
                modelObject[key] = value
                modelObject.$lock = false
            } else {
                modelObject[key] = value
            }

            if (isUpdateEnabled){
                this.dataChanged(row, row+1)
            }
        }

        return true
    }


    function RemoveData(key, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0

        var modelObject = this.get(row)

        if (modelObject === null)
            console.log("modelObject is null")

        if(key in modelObject){
            if(modelObject[key] && modelObject[key].unsubscribe) modelObject[key].unsubscribe()
            delete modelObject[key]
        }



        if (isUpdateEnabled){
            this.modelChanged()
        }
    }

    function Clear(){
        this.clear();
    }

    function IsValidData(key, row){
        var data = this.GetData(key, row);
        var retVal = (data === undefined || data === null) ? false : true;
        return retVal
    }

    function SetExternTreeModel(key, value, row){
        this.SetData(key, value, row)
    }

    function CopyItemDataFromModel(index, externTreeModel, externIndex){
        RemoveItem(index)
        InsertNewItem(index)

        let retVal = true
        let keys = externTreeModel.GetKeys(externIndex)

        for(let key of keys){
            let value = externTreeModel.GetData(key, externIndex)

            if (typeof value === 'object' && value.constructor.name.indexOf('TreeItemModel') >= 0){
                let childModel = AddTreeModel(key, index)

                retVal = retVal && childModel.CopyFrom(value)
            } else {
                retVal = retVal && SetData(key, value, index)
            }

            if (!retVal){
                break
            }
        }

        if (isUpdateEnabled){
            this.dataChanged()
        }

        return retVal
    }

    function CopyFrom(externTreeModel){
        if(externTreeModel){
            Clear()

            let retVal = true

            for(let index = 0; index < externTreeModel.GetItemsCount(); index++){
                InsertNewItem(index)

                let keys = externTreeModel.GetKeys(index)

                for(let key of keys){
                    let value = externTreeModel.GetData(key, index)

                    if (typeof value === 'object' && value.constructor.name.indexOf('TreeItemModel') >= 0){
                        let childModel = AddTreeModel(key, index)

                        retVal = retVal && childModel.CopyFrom(value)
                    } else {
                        retVal = retVal && SetData(key, value, index)
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

    function CopyItemDataToModel(index, model, externIndex){
        if(externIndex === undefined){
            externIndex = 0;
        }
        var keys = this.GetKeys(index);

        for(var key in keys){
            let value = this.GetData(keys[key], index);
            model.SetData(keys[key], value, externIndex);
        }
    }

    function GetModelFromItem(index){
        if(count <= 0 || index < 0 || index >= count) return;

        let item = this.get(index);
        let retModel = this.createComponent("imtqml/TreeItemModel.qml", null);
        CopyItemDataToModel(index, retModel, 0)
        return retModel;

    }



    function IsEqualWithModel(externModel){
        if(!externModel) {
            return false
        }
        return IsEqual(externModel);
    }

    function IsEqual(sourceModel){
        if(sourceModel) {
            if(this.GetItemsCount() !== sourceModel.GetItemsCount()){
                return false;
            }

            for(let i = 0; i < sourceModel.GetItemsCount(); ++i){
                var item = this.GetModelFromItem(i);
                var sourceItem = sourceModel.GetModelFromItem(i);

                var itemKeys = item.GetKeys();
                var sourceItemKeys = sourceItem.GetKeys();

                if(itemKeys.length !== sourceItemKeys.length){
                    return false;
                }

                for(let key in sourceItemKeys){
                    if(!itemKeys.includes(sourceItemKeys[key])){
                        return false;
                    }

                    var itemValue = item.GetData(sourceItemKeys[key]);
                    var sourceValue = sourceItem.GetData(sourceItemKeys[key]);

                    if(typeof sourceValue === 'object'){
                        if(typeof itemValue !== 'object'){
                            return false;
                        }

                        let result = sourceValue.IsEqual(itemValue);

                        if(!result){
                            return false;
                        }
                    }
                    else if(sourceValue !== itemValue){
                        return false;
                    }
                }
            }

            return true;
        }

        return false;
    }

    function Copy(obj){
        if(!obj) return false

        this.clear()
        for(let i = 0; i < obj.count; i++){
            this.CopyItemDataFromModel(this.InsertNewItem(), obj, i)
        }
        return true
    }

    function CopyMe(){
        let retModel = this.createComponent("imtqml/TreeItemModel.qml", null);
        if (!retModel){
            return null
        }

        if (!retModel.Copy(this)){
            return null
        }

        return retModel;
    }

    function GetKeys(index){
        if (index === undefined) index = 0

        var keys = []
        if(this.count > 0){
            var modelObject = this.get(index)
            for(let key in modelObject){
                if(key === 'index' || key === 'model' || key === 'context') continue
                keys.push(key)
            }

            keys = keys.filter(function(arrFiltered) { return arrFiltered !== 'index' })

        }
        return keys;
    }


    function ContainsKey(key,row){
        //        return true
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        if(this.count > row)
            return this.get(row).hasOwnProperty(key)
        return false
    }


    function SetQueryParam(key,value){
        this.queryParams[key] = value
    }


    function needsReload(){
        var newSource = this.baseUrl
        var first = true;
        for (var queryKey in this.queryParams){
            if (first)
                newSource += "?"
            else
                newSource += "&"
            first = false
            newSource += queryKey + "=";
            newSource += this.queryParams[queryKey]
  //          newSource += "\""
        }
        console.log("newSource",newSource)
        this.source = newSource
    }


    function Refresh(){

    }


    function SetIsArray(value){
        this.isArray = value;
    }


    function InsertNewItem(index){
        if(index !== undefined && index !== null && typeof index === 'number'){
            this.insert(index, {});
            return index;
        } else {
            this.append({});
            return this.count - 1;
        }
    }

    function InsertNewItemWithParameters(index, parameters){
        this.append(parameters);
        return this.count - 1;
    }

    function RemoveItem(index){
        this.remove(index)

        if (isUpdateEnabled){
            this.dataChanged()
        }
    }


    function AddTreeModel(key, row){
        var retModel;
        retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
        this.SetUpdateEnabled(false)
        this.SetData(key, retModel, row);
        this.SetUpdateEnabled(true)
        return retModel;
    }

    //Parse from the extern json to the intern model
    function CreateFromJson(jsonString){
        treeItemModelContainer.clear();
        this.json = jsonString;

        this.updateJSONModel()
        this.updateTreeItemJSONModel()
    }

    function toJSON(){
        var retVal = ""
        if (this.isArray || this.count > 1)
            retVal += "["
        else
            retVal += "{"
        for (var i = 0; i < this.count; i++){
            var modelObject = this.get(i)
            if (i > 0){
                retVal += ","
            }
            if (this.isArray || this.count > 1)
                retVal += "{"

            let recurciveJSON = function(modelData){
                if (modelData === null || modelData === undefined) {
                    retVal += "null"
                } else if(typeof modelData === 'object'){
                    if(modelData instanceof QModelData){
                        let j = 0
                        for (var property in modelObject) {
                            if (j > 0) retVal += ","
                            j++;
                            retVal += "\"" + property + "\":"

                            recurciveJSON(modelData[property])
                        }
                    } else if(modelData.constructor.name.indexOf('TreeItemModel') >= 0){
                        retVal += modelData.toJSON()
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

            if (this.isArray || this.count > 1)
                retVal += "}"
        }

        if (this.isArray || this.count > 1)
            retVal += "]"
        else
            retVal += "}"
        return retVal
    }

    function updateTreeItemJSONModel(){
//        console.log("updateTreeItemJSONModel")
        for(var row = 0; row < this.GetItemsCount(); row++){
            var modelObject = this.get(row)
            var keys = Object.keys(modelObject)
            for ( var index in keys ) {
                var retVal = modelObject[keys[index]]
                if(retVal !== null && typeof retVal === 'object' && !(retVal instanceof QtObject)){
                    var retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
                    if(typeof modelObject === 'object' && modelObject.constructor.name === 'QModelData'){
                        modelObject.$lock = true
                        modelObject[keys[index]] = retModel
                        modelObject.$lock = false
                    } else {
                        modelObject[keys[index]] = retModel
                    }
                    retModel.isUpdateEnabled = this.isUpdateEnabled
                    retModel.getProperty('isUpdateEnabled').setCompute(function(){return this.isUpdateEnabled}.bind(this))
                    retModel.getProperty('isUpdateEnabled').update()
                    if(Object.keys(retVal).length) retModel.append(retVal);
                    retVal = retModel
                    retVal.updateTreeItemJSONModel()
                }
                this.SetData(keys[index],retVal,row)
            }
        }
    }
}
