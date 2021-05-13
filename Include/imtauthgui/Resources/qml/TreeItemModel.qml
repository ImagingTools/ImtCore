import QtQuick 2.0

JSONListModel {
    id: container

    property var infoPath: []
    
    function get(row){
        return container.model.get(row)
    }
    
    function GetData(key, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        var modelObject = get(row)
        var retVal = modelObject[key]
        return  retVal
    }
    function SetData(key, value, row){
        if(row === undefined)
            row = 0
        if(row === null)
            row = 0
        console.log("setData",key,value,row)
        var modelObject = get(row)
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
}
