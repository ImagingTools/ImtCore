import QtQuick 2.0

JSONListModel {
    id: container;

    property var infoPath: [];
    property string baseUrl;
    property var queryParams: [];
    property bool isArray: false;

//    function get(row){
//        return container.get(row)
//    }


    function GetItemsCount(){
        return this.count
    }


    function GetData(key, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        var modelObject = this.get(row)
        var retVal = modelObject[key]
        if (retVal === null)
            return null
        if(typeof retVal === 'object' && retVal._qmlName !== 'TreeItemModel.qml'){
            var retModel
            retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
            retModel.append(retVal);
            //            var component = Qt.createComponent("TreeItemModel.qml");
//            if (component.status === Component.Ready) {
//                retModel = component.createObject(container);
//                retModel.append(retVal);
//            }
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
        console.log("setData",key,value,row)
        var modelObject = this.get(row)

        if (modelObject === null)
            console.log("modelObject is null")

        modelObject[key] = value

//        var xhr = new XMLHttpRequest;
//        var json = {}
//        json[key] = value
//        for (var keyInfo in this.infoPath){
//            json[keyInfo] = this.infoPath[keyInfo]
//        }
//        var txt_data = JSON.stringify(json)
//        console.log("setData", json, txt_data, json.toString())
//        xhr.open("POST", target);
//        xhr.send(txt_data);
    }


    function SetExternTreeModel(key, value, row){
        this.SetData(key, value, row)
    }

    function GetKeys(){
        var keys = []
        if(this.count > 0){
            var modelObject = this.get(0)
            keys = Object.keys(modelObject)
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


    function InsertNewItem(){
        this.append({});
        return this.count - 1;
    }


    function RemoveItem(index){
        this.remove(index)
    }


    function AddTreeModel(key, row){
        var retModel;
        retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
        this.SetData(key, retModel, row);
        return retModel;
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

            var j = 0;
            for (var property in modelObject) {
                if (j > 0)
                    retVal += ","
                j++;
                retVal += "\"" + property + "\":"
                var modelVal = modelObject[property]
                if (modelVal === null)
                    modelVal += "null"
                else if(typeof modelVal === 'object' && modelVal._qmlName == 'TreeItemModel.qml'){
                    retVal += modelVal.toJSON()
                }
                else if(typeof modelVal === 'string' || modelVal instanceof String){
                    retVal += "\"" + modelVal + "\""
                }
                else
                    retVal += modelVal
            }
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
        for(var row = 0; row < this.GetItemsCount(); row++){
            var modelObject = this.get(row)
            var keys = Object.keys(modelObject)
            for ( var index in keys ) {
                var retVal = modelObject[keys[index]]
                if(retVal !== null && typeof retVal === 'object' && retVal._qmlName !== 'TreeItemModel.qml'){
                    var retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
                    retModel.append(retVal);
                    retVal = retModel
                    retVal.updateTreeItemJSONModel()
                }
                this.SetData(keys[index],retVal,row)
            }
        }
    }


}
