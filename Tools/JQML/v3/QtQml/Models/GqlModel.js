const TreeItemModel = require("./TreeItemModel")

class GqlModel extends TreeItemModel {
	GetGlobalAccessToken(){
		return XMLHttpRequest.QMLAuthToken || ""
	}

    SetGlobalAccessToken(token){
		XMLHttpRequest.QMLAuthToken = token
	}
	
	SetRefreshToken(token){
		XMLHttpRequest.QMLAuthRefreshToken = token
	}
	
	SetProductId(productId){
		XMLHttpRequest.QMLProductId = productId
	}
	
	GetGlobalAccessToken(){
		return XMLHttpRequest.QMLAuthToken || ""
	}
	
	GetProductId(){
		return XMLHttpRequest.QMLProductId || ""
	}
    
    setGqlQuery(gqlData, headers){
        this.state = "Loading"
        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")
        xhr.gqlRequest = this

        if(headers && typeof headers === 'object'){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        xhr.onreadystatechange = ()=>{
            if (!this.__destroyed && xhr.readyState === XMLHttpRequest.DONE){
				if (xhr.status === 401){
					this.state = "Unauthorized"
				}
				
				if (xhr.status === 403){
					this.state = "Forbidden"
				}
				
				if (xhr.status === 200){
					this.json = xhr.responseText
					this.state = "Processing"
					this.updateJSONModel()
					this.updateTreeItemJSONModel()
					this.state = "Ready"
				}
			}
        }

        xhr.send(gqlData)
    }
}



module.exports = GqlModel