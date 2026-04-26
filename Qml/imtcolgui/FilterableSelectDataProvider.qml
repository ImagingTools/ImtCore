import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

/*!
	\qmltype FilterableSelectDataProvider
	\inqmlmodule imtcolgui
	\brief Abstract data provider for FilterableSelectPopup.

	Manages request lifecycle, pagination, data normalization and race condition protection.
	UI receives only normalized JS objects and state signals — no backend coupling.
	Subclasses must override \c executeRequest() and \c executeFetchByIds()
	to implement concrete data fetching.

	Maintains a persistent \c selectedItems map that stores full item objects for
	all selected IDs, independent from the current visible dataset (pagination/filtering).

	\sa FilterableSelectPopup, FilterableSelectGqlDataProvider
*/
QtObject {
	id: root

	// --- Configuration ---
	property string collectionId
	property int pageSize: 20

	// --- Public read-only state ---
	readonly property var items: __internal.items
	readonly property bool isInitialLoading: __internal.isInitialLoading
	readonly property bool isPageLoading: __internal.isPageLoading
	readonly property bool hasMore: __internal.hasMore
	readonly property var error: __internal.error
	readonly property var selectedItems: __internal.selectedItems

	// --- Signals for UI ---
	signal dataChanged()
	signal errorOccurred(var error)
	signal selectedItemsResolved()

	// --- Public API ---

	/*!
		Fetches the first page. Clears all existing data.
		\param filter Text filter string.
	*/
	function fetch(filter){
		__internal.items = []
		__internal.knownIds = ({})
		__internal.hasMore = true
		__internal.error = null
		__internal.currentFilter = filter || ""
		__internal.currentOffset = 0
		__internal.isInitialLoading = true
		__internal.isPageLoading = false

		__internal.requestId++
		root.dataChanged()
		root.executeRequest(__internal.requestId, root.pageSize, 0, __internal.currentFilter)
	}

	/*!
		Fetches the next page. Ignored if already loading or no more items.
	*/
	function fetchMore(){
		if (__internal.isInitialLoading || __internal.isPageLoading || !__internal.hasMore){
			return
		}

		__internal.isPageLoading = true
		__internal.error = null

		__internal.requestId++
		root.dataChanged()
		root.executeRequest(__internal.requestId, root.pageSize, __internal.currentOffset, __internal.currentFilter)
	}

	/*!
		Retries the last failed request.
	*/
	function retry(){
		if (__internal.items.length === 0){
			root.fetch(__internal.currentFilter)
		}
		else {
			root.fetchMore()
		}
	}

	/*!
		Fetches specific items by their IDs for resolving preselected items
		that are not in the current visible dataset.
		\param ids Array of item IDs to fetch.
	*/
	function fetchByIds(ids){
		if (!ids || ids.length === 0){
			return
		}

		__internal.fetchByIdsRequestId++
		root.executeFetchByIds(__internal.fetchByIdsRequestId, ids)
	}

	// --- Selection management ---

	/*!
		Stores a full item object in the selectedItems map.
		\param id Item ID.
		\param item Full normalized item object { id, title, ... }.
	*/
	function addSelectedItem(id, item){
		if (!id || id === "" || !item){
			return
		}
		var updated = ({})
		for (var k in __internal.selectedItems){
			updated[k] = __internal.selectedItems[k]
		}
		updated[String(id)] = item
		__internal.selectedItems = updated
	}

	/*!
		Removes an item from the selectedItems map.
		\param id Item ID to remove.
	*/
	function removeSelectedItem(id){
		if (!id || id === ""){
			return
		}
		var updated = ({})
		for (var k in __internal.selectedItems){
			if (k !== String(id)){
				updated[k] = __internal.selectedItems[k]
			}
		}
		__internal.selectedItems = updated
	}

	/*!
		Clears all selected items.
	*/
	function clearSelectedItems(){
		__internal.selectedItems = ({})
	}

	/*!
		Returns the display text for a selected item by ID.
		\param id Item ID.
		\return Title string or empty string if not resolved.
	*/
	function getSelectedItemText(id){
		var sid = String(id || "")
		if (sid === ""){
			return ""
		}
		var item = __internal.selectedItems[sid]
		return item ? String(item.title || "") : ""
	}

	/*!
		Returns an array of all selected item objects.
	*/
	function getSelectedItems(){
		var result = []
		for (var k in __internal.selectedItems){
			if (__internal.selectedItems[k]){
				result.push(__internal.selectedItems[k])
			}
		}
		return result
	}

	// --- Protected: subclass must override ---

	/*!
		Override in subclass to perform actual data fetching.
		When results arrive, call \c onRequestSuccess(requestId, items).
		On error, call \c onRequestError(requestId, message, code).
		\param requestId Unique request identifier — must be passed back to response handler.
		\param count Number of items to fetch.
		\param offset Offset for pagination.
		\param filter Text filter string.
	*/
	function executeRequest(requestId, count, offset, filter){
		console.warn("executeRequest() should be implemented in a subclass")
		root.onRequestSuccess(requestId, [])
	}

	/*!
		Override in subclass to fetch specific items by IDs.
		When results arrive, call \c onFetchByIdsSuccess(requestId, items).
		On error, call \c onFetchByIdsError(requestId, message).
		\param requestId Unique request identifier.
		\param ids Array of item IDs to fetch.
	*/
	function executeFetchByIds(requestId, ids){
		console.warn("executeFetchByIds() should be implemented in a subclass")
		root.onFetchByIdsSuccess(requestId, [])
	}

	// --- Protected: called by subclass on response ---

	/*!
		Called by subclass when request completes successfully.
		\param requestId Must match the requestId from executeRequest.
		\param rawItems Array of normalized JS objects: { id, title, description, icon, color }.
	*/
	function onRequestSuccess(requestId, rawItems){
		if (requestId !== __internal.requestId){
			return
		}

		var newItems = rawItems || []
		var merged = __internal.items.slice()
		var ids = ({})
		for (var k in __internal.knownIds){
			ids[k] = true
		}

		for (var i = 0; i < newItems.length; i++){
			var item = newItems[i]
			if (item && item.id !== undefined && item.id !== null){
				var sid = String(item.id)
				if (!ids[sid]){
					ids[sid] = true
					merged.push(item)
				}
			}
		}

		__internal.knownIds = ids
		__internal.items = merged
		__internal.currentOffset = merged.length
		__internal.hasMore = newItems.length >= root.pageSize
		__internal.isInitialLoading = false
		__internal.isPageLoading = false
		__internal.error = null
		root.dataChanged()
	}

	/*!
		Called by subclass when request fails.
		\param requestId Must match the requestId from executeRequest.
		\param message Human-readable error text.
		\param code Optional error code string.
	*/
	function onRequestError(requestId, message, code){
		if (requestId !== __internal.requestId){
			return
		}

		__internal.isInitialLoading = false
		__internal.isPageLoading = false
		__internal.error = {
			message: message || "Unknown error",
			code: code || "",
			requestId: requestId
		}
		root.dataChanged()
		root.errorOccurred(__internal.error)
	}

	/*!
		Called by subclass when fetchByIds completes successfully.
		Merges resolved items into selectedItems map.
		\param requestId Must match the requestId from executeFetchByIds.
		\param resolvedItems Array of normalized JS objects.
	*/
	function onFetchByIdsSuccess(requestId, resolvedItems){
		if (requestId !== __internal.fetchByIdsRequestId){
			return
		}

		var updated = ({})
		for (var k in __internal.selectedItems){
			updated[k] = __internal.selectedItems[k]
		}

		var items = resolvedItems || []
		for (var i = 0; i < items.length; i++){
			var item = items[i]
			if (item && item.id !== undefined && item.id !== null){
				updated[String(item.id)] = item
			}
		}

		__internal.selectedItems = updated
		root.selectedItemsResolved()
	}

	/*!
		Called by subclass when fetchByIds fails.
		\param requestId Must match the requestId from executeFetchByIds.
		\param message Human-readable error text.
	*/
	function onFetchByIdsError(requestId, message){
		if (requestId !== __internal.fetchByIdsRequestId){
			return
		}
		// Silent failure — selectedItems stays unchanged, IDs remain unresolved
	}

	// --- Private state ---
	property QtObject __internal: QtObject {
		property var items: []
		property var knownIds: ({})
		property int requestId: 0
		property int fetchByIdsRequestId: 0
		property int currentOffset: 0
		property string currentFilter: ""
		property bool isInitialLoading: false
		property bool isPageLoading: false
		property bool hasMore: true
		property var error: null
		property var selectedItems: ({})
	}
}
