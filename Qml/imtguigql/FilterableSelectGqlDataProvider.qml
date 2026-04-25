import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

FilterableSelectDataProvider {
	id: root

	// HTTP headers
	function getHeaders(){
		return {}
	}

	// Fields to get from server
	property var fields: ["id", "name"]

	property GqlRequestSender itemsRequest: GqlRequestSender {
		requestType: 0
		gqlCommandId: "GetSelectableItems"

		function createQueryParams(query){
			var viewParams = Gql.GqlObject("viewParams")
			viewParams.InsertField("offset", root.__offset)
			viewParams.InsertField("count", root.__count)
			viewParams.InsertField("filterModel", root.__filter)

			var inputParams = Gql.GqlObject("input")
			inputParams.InsertField("collectionId", root.collectionId)
			inputParams.InsertFieldObject(viewParams)

			query.AddParam(inputParams)

			var queryFields = Gql.GqlObject("items")
			for (let key of root.fields){
				queryFields.InsertField(key)
			}
			query.AddField(queryFields)
		}

		function getHeaders(){
			return root.getHeaders()
		}

		function onResult(data){
			root.listObjectsReceived(data)
		}

		function onError(message, type){
			root.listObjectsReceiveFailed(message)
		}
	}

	// Internal state for last request params
	property int __offset: 0
	property int __count: 20
	property var __filter: null

	function getSelectableItems(count, offset, filter){
		root.__offset = offset || 0
		root.__count = count || 20
		root.__filter = filter || null
		itemsRequest.send()
	}
}
