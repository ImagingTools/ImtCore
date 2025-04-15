import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0

// Provider for SDL object
DataModelProvider {
	id: root
	
	property string getCommandId
	
	function loadDataModel(paramsObj){
		var query = Gql.GqlRequest("query", getCommandId)
		
		let inputObject = Gql.GqlObject("input")
		if (paramsObj.toGraphQL !== undefined){
			inputObject.fromObject(paramsObj)
		}
		else if (typeof paramsObj == "object"){
			let keys = Object.keys(paramsObj)
			for (let key in paramsObj){
				inputObject.InsertField(key, paramsObj[key])
			}
		}
		
		query.AddParam(inputObject)
		
		gqlRequest.setGqlQuery(query.GetQuery(), root.getHeaders())
	}
	
	function getHeaders(){
		return {}
	}
	
	property GqlRequest gqlRequest: GqlRequest {
		onStateChanged: {
			if (state === "Ready"){
				let responseObj = null
				try{
					responseObj = JSON.parse(json)
				}
				catch(e){
					root.dataModelLoadFailed("Json convertation failed")
					return
				}
				
				if ("data" in responseObj){
					let dataObject = responseObj["data"]
					if (root.getCommandId in dataObject){
						dataObject = dataObject[root.getCommandId]
					}

					if (!root.dataModel){
						root.dataModelLoadFailed("Unable to create data model from json. Error: Data model is invalid")
						return
					}
					
					root.dataModel.fromObject(dataObject)

					root.dataModelReady(root.dataModel)
					
					return
				}
				
				let message = ""
				if ("errors" in responseObj){
					let errorsObject = responseObj["errors"]
					if (root.getCommandId in errorsObject){
						errorsObject = errorsObject[root.getCommandId]
					}

					if ("message" in errorsObject){
						message = errorsObject["message"];
					}
				}

				root.dataModelLoadFailed(message)
			}
			else if (state === "Error"){
				root.dataModelLoadFailed(qsTr("Network error"))
			}
		}
	}
}

