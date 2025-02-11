const { TreeItemModel } = require('./TreeItemModel')

class GqlModel extends TreeItemModel {
    SetGlobalAccessToken(token){
        XMLHttpRequest.QMLAuthToken = token
    }

	SetRefreshToken(token){
		XMLHttpRequest.QMLAuthRefreshToken = token
	}
    
    setGqlQuery(gqlData, headers={}){
        this.getProperty('state').reset("Loading")
        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")

        if(headers){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        xhr.send(gqlData)

        xhr.onreadystatechange = function(){
            if (this.UID && xhr.readyState === XMLHttpRequest.DONE){
                this.getProperty('json').reset(xhr.responseText)
                this.getProperty('state').reset("Processing")
                this.updateJSONModel()
                this.updateTreeItemJSONModel()
                this.getProperty('state').reset("Ready")
            }
        }.bind(this)
    }

}

module.exports.GqlModel = GqlModel
