import QtQuick 2.0
import "./"
import "../"

TreeItemModel {
    id: container
    
    function SetGqlQuery(gqlData){
        console.log("SetGqlQuery",gqlData)
        state = "Loading"
        var xhr = new XMLHttpRequest;
        xhr.open("POST", "../../graphql");
        xhr.send(gqlData);

        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE){
                json = xhr.responseText;
                updateJSONModel()
//                console.log("Model",jsonModel.$items)
                state = "Ready"
            }
        }
    }
}
