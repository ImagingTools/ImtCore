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
		xhr.gqlRequest = this;
		xhr.open("POST", "../../graphql")
		
		if(headers){
			for(let name in headers){
				xhr.setRequestHeader(name, headers[name])
			}
		}
		
		xhr.send(gqlData)
		
		xhr.onreadystatechange = function(){
			if (this.UID && xhr.readyState === XMLHttpRequest.DONE){
				if (xhr.status === 401){
					this.getProperty('state').reset("Unauthorized")
				}
				
				if (xhr.status === 403){
					this.getProperty('state').reset("Forbidden")
				}
				
				if (xhr.status === 200){
					this.getProperty('json').reset(xhr.responseText)
					this.getProperty('state').reset("Processing")
					this.updateJSONModel()
					this.updateTreeItemJSONModel()
					this.getProperty('state').reset("Ready")
				}
			}
		}.bind(this)
	}
	
}

module.exports.GqlModel = GqlModel
