import QtQuick 2.0
import "jsonpath.js" as JSONPath

Item {
	property string source: ""
	property string json: ""
	property string query: ""
	property string target: ""

	property ListModel model : ListModel { id: jsonModel }
	property alias count: jsonModel.count

	onSourceChanged: {
		state = "Loading"
        if(source == "")
            return
		var xhr = new XMLHttpRequest;
		xhr.open("GET", source);
		xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE){
				json = xhr.responseText;
                state = "Ready"
            }
		}
		xhr.send();
	}

	onJsonChanged: updateJSONModel()
	onQueryChanged: updateJSONModel()

	function makeJson(){
		json = JSON.stringify(jsonModel.$items)
	}
	function sendJson(method){
		var xhr = new XMLHttpRequest;
		xhr.open(method, target);
		xhr.send(json);
	}

    function updateJSONModel() {
	jsonModel.clear();

	if ( json === "" )
	    return;
	var d1 = new Date()
	var objectArray = parseJSONString(json, query);
	var dict = {};
	for ( var key in objectArray ) {
	    var jo = objectArray[key];
	    dict[key] = jo
	}
        jsonModel.append(dict);
	var d2 = new Date()
	console.log(d2.getMilliseconds() - d1.getMilliseconds())
    }
    
	function parseJSONString(jsonString, jsonPathQuery) {
		var objectArray = JSON.parse(jsonString);
		if ( jsonPathQuery !== "" )
			objectArray = JSONPath.jsonPath(objectArray, jsonPathQuery);

		return objectArray;
	}

	Component.onCompleted: updateJSONModel();
}
