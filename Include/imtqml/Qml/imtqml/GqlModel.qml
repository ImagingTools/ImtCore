import QtQuick 2.0

TreeItemModel {
    id: container;
    
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
                this.updateJSONModel()
                this.updateTreeItemJSONModel()
                this.state = "Ready"
            }
        }.bind(this)
    }
}
