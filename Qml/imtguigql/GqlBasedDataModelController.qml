import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

DataModelController {
	id: root

	property string gqlCommandId
	property var responseModel
	property var inputModel
	property string context

	function saveDataModel(params, documentId, documentName, documentDescription, documentTypeId){
		root.requestStarted(params)

		var query = Gql.GqlRequest("mutation", gqlCommandId)

		let inputObject = Gql.GqlObject("input")

		let inputParams = params
		if (inputModel){
			inputParams = prepareInputModel(params, documentId, documentName, documentDescription, documentTypeId)
		}

		if (inputParams && inputParams.toGraphQL !== undefined){
			inputObject.fromObject(inputParams)
		}
		else if (typeof inputParams == "object"){
			let keys = Object.keys(params)
			for (let key in params){
				inputObject.InsertField(key, params[key])
			}
		}

		query.AddParam(inputObject)

		let headers = root.getHeaders()
		if (headers && root.context && root.context != "")
			headers["context"] = root.context

		gqlRequest.setGqlQuery(query.GetQuery(), headers)
	}

	function prepareInputModel(params, documentId, documentName, documentDescription){
		return params
	}

	function getHeaders(){
		return {}
	}

	function prepareResultModel(){

	}

	property GqlRequest gqlRequest: GqlRequest {
		onStateChanged: {
			if (state === "Ready"){
				let responseObj = null
				try{
					responseObj = JSON.parse(json)
				}
				catch(e){
					root.dataModelSaveFailed("Json convertation failed")
					return
				}

				if ("data" in responseObj){
					let dataObject = responseObj["data"]
					if (root.gqlCommandId in dataObject){
						dataObject = dataObject[root.gqlCommandId]
					}

					let responseModel = root.resultModel
					if (root.responseModel){
						responseModel = root.responseModel
					}

					if (!responseModel){
						root.dataModelSaveFailed("Unable to create data model from json. Error: Result model is invalid")
						return
					}

					responseModel.fromObject(dataObject)

					root.prepareResultModel()

					root.resultModelReady(root.resultModel)

					return
				}

				let message = ""
				if ("errors" in responseObj){
					let errorsObject = responseObj["errors"]
					if (root.gqlCommandId in errorsObject){
						errorsObject = errorsObject[root.gqlCommandId]
					}

					if ("message" in errorsObject){
						message = errorsObject["message"];
					}
				}

				root.dataModelSaveFailed(message)
			}
			else if (state === "Error"){
				root.dataModelSaveFailed(qsTr("Network error"))
			}
		}
	}
}

