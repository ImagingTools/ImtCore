import QtQuick 2.0
import "jsonpath.js"

ListModel {
	property string source: "";
	property string json: "";
	property string query: "";
	property string target: "";

//	property ListModel model : ListModel { id: jsonModel }
//	property alias count: jsonModel.count
    property string state;

	onSourceChanged: {
		this.state = "Loading"
        if(this.source == "")
            return
		var xhr = new XMLHttpRequest;
		xhr.open("GET", source);
        xhr.onreadystatechange = () => {
            if (xhr.readyState === XMLHttpRequest.DONE){
		this.json = xhr.responseText;
                this.updateJSONModel()
//                console.log("Model",jsonModel.$items)
                this.state = "Ready"
            }
		}
		xhr.send();
	}

    //onJsonChanged: updateJSONModel()
	onQueryChanged: {
		this.updateJSONModel()
	}

	function makeJson(){
		this.json = JSON.stringify(jsonModel.$items)
	}
	function sendJson(method){
		var xhr = new XMLHttpRequest;
		xhr.open(method, target);
		xhr.send(json);
	}

    function updateJSONModel() {
        if ( this.json === "" )
            return;

        this.clear();
		var d1 = new Date()
        var objectArray = this.parseJSONString(this.json, this.query);
		var dict = {};
		for ( var key in objectArray ) {
			var jo = objectArray[key];
			dict[key] = jo
		}
			this.append(dict);
		var d2 = new Date()
		console.log(d2.getMilliseconds() - d1.getMilliseconds())
    }
    
	function parseJSONString(jsonString, jsonPathQuery) {
		var objectArray = JSON.parse(jsonString);
		if ( jsonPathQuery !== "" )
			objectArray = jsonPath(objectArray, jsonPathQuery);

		return objectArray;
	}

	Component.onCompleted: {
		this.updateJSONModel();
	}
}
