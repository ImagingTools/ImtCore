import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Provider for SDL object
DataModelProvider {
	id: root
	
	property string getCommandId
	property var responseModel
	property var inputModel
	
	function requestDataModel(paramsObj){
		console.log("requestDataModel")
		root.requestStarted(paramsObj)

		var query = Gql.GqlRequest("query", getCommandId)
		
		let inputObject = Gql.GqlObject("input")

		let inputParams = paramsObj
		if (!inputParams){
			inputParams = inputModel
		}
		inputParams = prepareInputModel(inputParams)

		if (inputParams && inputParams.toGraphQL !== undefined){
			inputObject.fromObject(inputParams)
		}
		else if (typeof inputParams == "object"){
			let keys = Object.keys(paramsObj)
			for (let key in paramsObj){
				inputObject.InsertField(key, paramsObj[key])
			}
		}
		
		query.AddParam(inputObject)
		
		gqlRequest.setGqlQuery(query.GetQuery(), root.getHeaders())
	}

	function prepareInputModel(paramsObj){
		return paramsObj
	}

	function prepareDataModel(){

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

					let responseModel = root.dataModel
					if (root.responseModel){
						responseModel = root.responseModel
					}

					if (!responseModel){
						root.dataModelLoadFailed("Unable to create data model from json. Error: Data model is invalid")
						return
					}

					responseModel.fromObject(dataObject)

					root.prepareDataModel()

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

