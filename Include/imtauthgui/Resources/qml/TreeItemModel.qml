import QtQuick 2.0

JSONListModel {
    id: container

    property var infoPath: []
    
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
