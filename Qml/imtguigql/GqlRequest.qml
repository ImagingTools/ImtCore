import QtQuick 2.12
import imtcontrols 1.0

QtObject {
    id: root;

    property string json;
    property string state;

    function setGqlQuery(gqlData, headers){
        root.state = "Loading"

        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")

        if(headers){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        xhr.send(gqlData)

        xhr.onreadystatechange = function(){
            if (xhr.readyState === XMLHttpRequest.DONE){
				if (xhr.status === 401 || xhr.status === 403){
					root.state = "Error";
					return
				}

                root.json = xhr.responseText;
				if (root.state === "Ready"){
					root.state = "Loading"
				}

                root.state = "Ready";
            }
        }
    }
}
