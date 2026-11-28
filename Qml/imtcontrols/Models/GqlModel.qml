import QtQuick 2.0

TreeItemModel {
    id: container;

    property var responseContext: null

    // Must stay in sync with imtbase::s_authenticationTokenHeaderId
    readonly property string authenticationTokenHeaderId: "x-authentication-token"

    function SetGlobalAccessToken(token){
        XMLHttpRequest.QMLAuthToken = token
    }

    function SetRefreshToken(token){
        XMLHttpRequest.QMLAuthRefreshToken = token
    }

    function SetProductId(productId){
        XMLHttpRequest.QMLProductId = productId
    }

    function GetGlobalAccessToken(){
        return XMLHttpRequest.QMLAuthToken || ""
    }

    function GetProductId(){
        return XMLHttpRequest.QMLProductId || ""
    }

    function setGqlQuery(gqlData, headers, context){
        this.state = "Loading"
        var xhr = new XMLHttpRequest;
        xhr.open("POST", "../../graphql");
        xhr.send(gqlData);

        xhr.onreadystatechange = function(){
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.responseContext = context
                this.json = xhr.responseText;
                this.state = "Processing"
                this.updateJSONModel()
                this.updateTreeItemJSONModel()
                this.state = "Ready"
            }
        }.bind(this)
    }
}
