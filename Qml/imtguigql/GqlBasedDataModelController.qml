import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

DataModelController {
	id: root

	property string gqlCommandId
	property var responseModel
	property var inputModel

	function saveDataModel(paramsObj){
		root.requestStarted(paramsObj)

		var query = Gql.GqlRequest("mutation", gqlCommandId)

		let inputObject = Gql.GqlObject("input")

		let inputParams = paramsObj
		if (inputModel){
			inputParams = prepareInputModel(paramsObj)
		}

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
					root.dataModelLoadFailed("Json convertation failed")
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
						root.dataModelLoadFailed("Unable to create data model from json. Error: Result model is invalid")
						return
					}

					responseModel.fromObject(dataObject)

					root.prepareResultModel()

					root.resultModelReady(root.dataModel)

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

