import QtQuick 2.0

TreeItemModel {
    id: container;

    function SetGlobalAccessToken(token){
        console.log("SetGlobalAccessToken",token)
        XMLHttpRequest.QMLAuthToken = token
    }
    
    function SetGqlQuery(gqlData){
        console.log("SetGqlQuery",gqlData)
        this.state = "Loading"
        var xhr = new XMLHttpRequest;
        xhr.open("POST", "../../graphql");
        xhr.send(gqlData);

        xhr.onreadystatechange = function(){
//        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText;
                this.state = "Processing"
                this.updateJSONModel()
                this.updateTreeItemJSONModel()
                this.state = "Ready"
            }
        }.bind(this)
    }
}
