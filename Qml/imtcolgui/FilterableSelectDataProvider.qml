import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

/*!
	\qmltype FilterableSelectDataProvider
	\inqmlmodule imtcolgui
	\brief Abstract data provider for FilterableSelectPopup.

	Manages request lifecycle, pagination, data normalization and race condition protection.
	UI receives only normalized JS objects and state signals — no backend coupling.
	Subclasses must override \c executeRequest() to implement concrete data fetching.

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

	// --- Signals for UI ---
	signal dataChanged()
	signal errorOccurred(var error)

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

		let newItems = rawItems || []
		let merged = __internal.items.slice()
		let ids = ({})
		for (let k in __internal.knownIds){
			ids[k] = true
		}

		for (let i = 0; i < newItems.length; i++){
			let item = newItems[i]
			if (item && item.id !== undefined && item.id !== null){
				let sid = String(item.id)
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

	// --- Private state ---
	property QtObject __internal: QtObject {
		property var items: []
		property var knownIds: ({})
		property int requestId: 0
		property int currentOffset: 0
		property string currentFilter: ""
		property bool isInitialLoading: false
		property bool isPageLoading: false
		property bool hasMore: true
		property var error: null
	}
}
