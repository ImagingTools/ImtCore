import QtQuick 2.0

JSONListModel {
    id: container

    property var infoPath: []
    property string baseUrl
    property var queryParams: []

//    function get(row){
//        return container.get(row)
//    }

    
    function GetData(key, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        var modelObject = get(row)
        var retVal = modelObject[key]
        if (retVal === null)
            return null
        if(typeof retVal === 'object'){
            var retModel
            var component = Qt.createComponent("TreeItemModel.qml");
            if (component.status === Component.Ready) {
                retModel = component.createObject(container);
                retModel.append(retVal);
            }
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
        var modelObject = get(row)

        if (modelObject === undefined && row === 0){
            append({})
            modelObject = get(row)
        }
        if (modelObject === null)
            console.log("modelObject is null")

        modelObject[key] = value

        var xhr = new XMLHttpRequest;
        var json = {}
        json[key] = value
        for (var keyInfo in infoPath){
            json[keyInfo] = infoPath[keyInfo]
        }
 //       alert( JSON.stringify(json) )
        var txt_data = JSON.stringify(json)
        console.log("setData", json, txt_data, json.toString())
        xhr.open("POST", target);
        xhr.send(txt_data);
    }

    function ContainsKey(key,row){
        //        return true
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        if(count > row)
            return get(row).hasOwnProperty(key)
        return false
    }

    function SetQueryParam(key,value){
        queryParams[key] = value
    }

    function needsReload(){
        var newSource = baseUrl
        var first = true;
        for (var queryKey in queryParams){
            if (first)
                newSource += "?"
            else
                newSource += "&"
            first = false
            newSource += queryKey + "=";
            newSource += queryParams[queryKey]
  //          newSource += "\""
        }
        console.log("newSource",newSource)
        source = newSource
    }
}
