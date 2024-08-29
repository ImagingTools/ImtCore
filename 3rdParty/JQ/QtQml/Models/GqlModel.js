const TreeItemModel = require("./TreeItemModel")

class GqlModel extends TreeItemModel {
    SetGlobalAccessToken(token){
        XMLHttpRequest.QMLAuthToken = token
    }
    
    setGqlQuery(gqlData){
        this.state = "Loading"
        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")
        xhr.send(gqlData)

        xhr.onreadystatechange = ()=>{
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText
                this.state = "Processing"
                this.updateJSONModel()
                this.updateTreeItemJSONModel()
                this.state = "Ready"
            }
        }
    }
}

module.exports = GqlModel