import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0

/*!
	\qmltype FilterableSelectCollectionDataProvider
	\inqmlmodule imtcolgui
	\brief FilterableSelectPopup data provider backed by a collection list request.

	Talks to the collection controller (CObjectCollectionControllerCompBase) instead of
	GetSelectableItems, so it inherits everything that controller already does for the
	collection views: the client field ids are mapped onto the stored ones
	(ReplaceComplexFilterFields), the headers decide what is filterable, and the text
	filter is turned into the collection query. Only paging is added on top.

	A row is exposed as \c { id, title, values }, where \c values holds every requested
	field by its id - a host delegate can therefore show any field of the collection
	without the server publishing extra parameters for it.

	\sa FilterableSelectDataProvider, FilterableSelectPopup, CollectionDataProvider
*/
FilterableSelectDataProvider {
	id: root

	//! GQL command-ID of the collection list request.
	property string commandId

	//! Collection fields to request; each one lands in \c item.values under its id.
	property var fields: []

	//! Field carrying the row id.
	property string idField: "id"

	//! Field shown as the row title.
	property string titleField: "name"

	//! Fields the typed text is matched against, as collection field ids.
	property var textFilterFieldIds: []

	//! Host-supplied GroupFilter objects, re-applied to every request.
	property var groupFilters: []

	property string sortByField
	property string orderType: "ASC"

	function getHeaders(){
		return {}
	}

	property QtObject __state: QtObject {
		property int pendingRequestId: 0
		property bool requestPending: false
		property var appliedGroupFilters: []
	}

	property CollectionDataProvider __collection: CollectionDataProvider {
		commandId: root.commandId
		fields: root.fields
		sortByField: root.sortByField
		orderType: root.orderType

		function getHeaders(){
			return root.getHeaders()
		}

		onModelUpdated: {
			root.__onCollectionUpdated()
		}

		onFailed: {
			if (!root.__state.requestPending){
				return
			}
			root.__state.requestPending = false
			root.onRequestError(root.__state.pendingRequestId, message, "GQL_ERROR")
		}
	}

	function executeRequest(requestId, count, offset, filter){
		if (!root.commandId){
			root.onRequestError(requestId, qsTr("No collection selected"), "EMPTY_COLLECTION")
			return
		}

		root.__state.pendingRequestId = requestId
		root.__state.requestPending = true

		var collectionFilter = root.__collection.filter
		root.__applyGroupFilters(collectionFilter)

		if (root.textFilterFieldIds && root.textFilterFieldIds.length > 0){
			collectionFilter.setFilteringInfoIds(root.textFilterFieldIds)
		}

		collectionFilter.setTextFilter(filter || "")

		root.__collection.offset = offset || 0
		root.__collection.count = count || root.pageSize
		root.__collection.updateModel()
	}

	function executeFetchByIds(requestId, ids){
		// Preselected rows are resolved via knownItems passed on popup open.
		root.onFetchByIdsSuccess(requestId, [])
	}

	// Re-attached per request: the host rebuilds its groups as its own state changes.
	function __applyGroupFilters(collectionFilter){
		var previous = root.__state.appliedGroupFilters
		for (var i = 0; i < previous.length; i++){
			collectionFilter.removeGroupFilter(previous[i])
		}

		var applied = []
		var groups = root.groupFilters || []
		for (var j = 0; j < groups.length; j++){
			if (groups[j]){
				collectionFilter.addGroupFilter(groups[j])
				applied.push(groups[j])
			}
		}

		root.__state.appliedGroupFilters = applied
	}

	function __onCollectionUpdated(){
		if (!root.__state.requestPending){
			return
		}

		root.__state.requestPending = false
		root.onRequestSuccess(root.__state.pendingRequestId, root.__normalizeItems())
	}

	function __normalizeItems(){
		var items = []
		var model = root.__collection.collectionModel
		if (!model){
			return items
		}

		var count = model.getItemsCount()
		for (var i = 0; i < count; i++){
			var values = ({})
			for (var j = 0; j < root.fields.length; j++){
				var fieldId = root.fields[j]
				values[fieldId] = model.containsKey(fieldId, i) ? String(model.getData(fieldId, i)) : ""
			}

			var id = String(values[root.idField] || "")
			if (id === ""){
				continue
			}

			items.push({
				id: id,
				title: String(values[root.titleField] || id),
				values: values
			})
		}

		return items
	}
}
