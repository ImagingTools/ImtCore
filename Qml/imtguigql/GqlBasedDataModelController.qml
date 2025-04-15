import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0

DataModelController {
	id: root
	
	property string setCommandId
	property string getCommandId
	
	property GqlBasedDataModelProvider dataModelProvider: GqlBasedDataModelProvider {
		getCommandId: root.getCommandId
		dataModel: root.dataModel
		
		onDataModelReady: {
			root.dataModelReady(params)
		}
		
		onDataModelLoadFailed: {
			root.dataModelLoadFailed(errorMessage)
		}
		
		function getHeaders(){
			return root.getHeaders()
		}
	}

	function loadDataModel(paramsObj){
		dataModelProvider.loadDataModel(paramsObj)
	}
	
	function updateDataModel(paramsObj){
		var query = Gql.GqlRequest("mutation", setCommandId)
		
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
					root.dataModelUpdateFailed("Json convertation failed")
					return
				}
				
				if ("data" in responseObj){
					root.dataModelUpdated(root.dataModel)
					return
				}
				
				let message = ""
				if ("errors" in responseObj){
					let errorsObject = responseObj["errors"]
					if (root.setCommandId in errorsObject){
						errorsObject = errorsObject[root.setCommandId]
					}

					if ("message" in errorsObject){
						message = errorsObject["message"];
					}
				}

				root.dataModelUpdateFailed(message)
			}
			else if (state === "Error"){
				root.dataModelUpdateFailed(qsTr("Network error"))
			}
		}
	}
}

