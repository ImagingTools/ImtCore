import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0

/*!
	\qmltype FilterableSelectDataProvider
	\inqmlmodule imtcolgui
	\brief CollectionDataProvider for FilterableSelectPopup with debounced text filtering.

	Wraps CollectionDataProvider to provide server-side text filtering with debounce,
	offset-based pagination, and model management for the filterable select popup.

	Usage:
	\code
	FilterableSelectDataProvider {
		commandId: "FilterableSelectList"
		textFilteringInfoIds: ["name", "description"]
	}
	\endcode
*/
CollectionDataProvider {
	id: root

	property int debounceInterval: 300
	property bool endListReached: false

	signal filterableModelUpdated()
	signal filterableModelFailed(string message)

	onModelUpdated: {
		root.filterableModelUpdated()
	}

	onFailed: {
		root.filterableModelFailed(message)
	}

	function applyTextFilter(text){
		root.filter.setTextFilter(text)
	}

	function resetAndFetch(){
		root.endListReached = false
		root.offset = 0
		root.updateModel(0)
	}

	function fetchNextPage(){
		if (root.endListReached || root.state.toLowerCase() !== "ready"){
			return
		}

		let currentCount = root.collectionModel ? root.collectionModel.getItemsCount() : 0
		if (currentCount > 0 && currentCount > root.offset){
			root.updateModel(root.offset + root.count)
		}
	}
}
