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

    
    function GetData(key, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        var modelObject = this.get(row)
        var retVal = modelObject[key]
        if (retVal === null)
            return null
        if(typeof retVal === 'object'){
            var retModel
            retModel = this.createComponent("TreeItemModel.qml", this);
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
        console.log("setData",key,value,row)
        var modelObject = this.get(row)

        if (modelObject === undefined && row === 0){
            this.append({})
            modelObject = this.get(row)
        }
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
    }

    function AddTreeModel(key, row){
        var retModel;
        retModel = this.createComponent("TreeItemModel.qml", this);
        this.SetData(key, retModel, row);
        return retModel;
    }

}
