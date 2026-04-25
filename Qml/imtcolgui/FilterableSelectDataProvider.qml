import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype FilterableSelectDataProvider
	\inqmlmodule imtcolgui
	\brief Abstract base data provider for FilterableSelectPopup.

	Defines the interface for loading selectable items with text filtering
	and offset-based pagination. Subclasses must override \l fetchItems()
	to implement actual data retrieval (GQL, REST, local, etc).

	This component is GQL-agnostic — FilterableSelectPopup uses it via
	dependency injection.
*/
QtObject {
	id: root

	property TreeItemModel collectionModel: TreeItemModel {}

	property string collectionId

	property int offset: 0
	property int count: 20

	// Fields to get from server
	property var fields: ["id", "name"]
	// Fields to which text filter will be applied
	property var textFilteringInfoIds: ["name"]

	property string orderType: "ASC"
	property string sortByField: "name"

	property string state: "Ready"

	property CollectionFilter filter: CollectionFilter {}

	signal modelUpdated(var data)
	signal failed(string message)

	Component.onCompleted: {
		if (sortByField && orderType){
			filter.setSortingInfo(sortByField, orderType)
		}
	}

	Component.onDestruction: {
		if (collectionModel){
			collectionModel.destroy()
		}

		if (filter){
			filter.destroy()
		}
	}

	onTextFilteringInfoIdsChanged: {
		if (textFilteringInfoIds && typeof textFilteringInfoIds === 'object'){
			if (!filter.hasTextFilter() && textFilteringInfoIds.length === 0){
				return
			}

			filter.setFilteringInfoIds(textFilteringInfoIds)
		}
	}

	onOrderTypeChanged: {
		if (sortByField && orderType){
			filter.setSortingInfo(sortByField, orderType)
		}
	}

	onSortByFieldChanged: {
		if (sortByField && orderType){
			filter.setSortingInfo(sortByField, orderType)
		}
	}

	function updateModel(newOffset){
		if (newOffset !== undefined && newOffset !== null && newOffset >= 0){
			root.offset = newOffset
		}

		fetchItems()
	}

	function clearModel(){
		collectionModel.clear()
	}

	/*!
		\qmlmethod void FilterableSelectDataProvider::fetchItems()
		\brief Override in subclass to perform actual data fetch.

		Should use root.offset, root.count, root.filter, root.fields,
		root.collectionId to build the request.
		On success: set collectionModel, state = "Ready", emit modelUpdated.
		On failure: emit failed(message).
	*/
	function fetchItems(){
		console.warn("fetchItems() should be implemented in a subclass")
		modelUpdated(null)
	}
}
