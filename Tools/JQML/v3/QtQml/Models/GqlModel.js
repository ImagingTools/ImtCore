const TreeItemModel = require("./TreeItemModel")

class GqlModel extends TreeItemModel {
	static requestTimeout = 120000

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

        // A request that never answers ends as a transport error instead of loading forever.
        xhr.timeout = GqlModel.requestTimeout

        xhr.onreadystatechange = ()=>{
            if (!this.__destroyed && xhr.readyState === XMLHttpRequest.DONE){
				if (xhr.status === 401){
					this.state = "Unauthorized"
				}
				else if (xhr.status === 403){
					this.state = "Forbidden"
				}
				else if (xhr.status === 200){
					this.json = xhr.responseText
					this.state = "Processing"
					this.updateJSONModel()
					this.updateTreeItemJSONModel()
					this.state = "Ready"
				}
				else{
					// 5xx, a dropped connection (status 0) or a timeout: without this the model stayed in
					// "Loading" and every view waiting for it kept its loading indicator.
					this.state = "Error"
				}
			}
        }

        xhr.send(gqlData)
    }
}



module.exports = GqlModel