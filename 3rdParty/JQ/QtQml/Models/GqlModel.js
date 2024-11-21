const TreeItemModel = require("./TreeItemModel")

class GqlModel extends TreeItemModel {
    SetGlobalAccessToken(token){
        XMLHttpRequest.QMLAuthToken = token
    }
    
    setGqlQuery(gqlData, headers){
        this.state = "Loading"
        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")

        if(headers && typeof headers === 'object'){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        xhr.send(gqlData)

        xhr.onreadystatechange = ()=>{
            if(this.__destroyed) return
            
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

GqlModel.initialize()

module.exports = GqlModel