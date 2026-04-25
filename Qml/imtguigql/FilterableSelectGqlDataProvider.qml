import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

/*!
	\qmltype FilterableSelectGqlDataProvider
	\inqmlmodule imtguigql
	\brief GQL-based data provider for FilterableSelectPopup using SDL structures.

	Extends FilterableSelectDataProvider and implements fetchItems() using
	GqlRequestSender with the GetSelectableItems query, following the
	CollectionDataProvider pattern for constructing viewParams.

	Usage:
	\code
	FilterableSelectGqlDataProvider {
		collectionId: "UsersCollection"
		fields: ["id", "name", "description"]
		textFilteringInfoIds: ["name"]
	}
	\endcode
*/
FilterableSelectDataProvider {
	id: root

	// HTTP headers
	function getHeaders(){
		return {}
	}

	property GqlRequestSender itemsRequest: GqlRequestSender {
		requestType: 0
		gqlCommandId: "GetSelectableItems"

		function createQueryParams(query){
			var viewParams = Gql.GqlObject("viewParams")
			viewParams.InsertField("offset", root.offset)
			viewParams.InsertField("count", root.count)
			viewParams.InsertField("filterModel", root.filter)

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
			root.collectionModel = data.getData("items")
			root.state = "Ready"
			root.modelUpdated(data)
		}

		function onError(message, type){
			root.state = "Ready"
			root.failed(message)
		}
	}

	function fetchItems(){
		root.state = "Loading"
		itemsRequest.send()
	}
}
