import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

/*!
	\qmltype FilterableSelectDataProvider
	\inqmlmodule imtcolgui
	\brief Abstract data provider for FilterableSelectPopup — SINGLE SOURCE OF TRUTH for data and selection.

	Manages request lifecycle, pagination, data normalization, race condition protection
	and the complete selection state. UI receives only normalized JS objects
	and reads selection via the provider API — no backend coupling.

	Selection model:
	- \c selectedIds defines WHAT is selected (source of truth)
	- \c selectedItems defines WHAT DATA is available for selected IDs (resolved cache, may be partial)

	Subclasses must override \c executeRequest() and \c executeFetchByIds()
	to implement concrete data fetching.

	\sa FilterableSelectPopup, FilterableSelectGqlDataProvider
*/
QtObject {
	id: root

	// --- Configuration ---
	property string collectionId
	property int pageSize: 20
	property bool multiSelect: false

	// --- Public read-only state (data) ---
	readonly property var items: __internal.items
	readonly property bool isInitialLoading: __internal.isInitialLoading
	readonly property bool isPageLoading: __internal.isPageLoading
	readonly property bool hasMore: __internal.hasMore
	readonly property var error: __internal.error

	// --- Signals ---
	signal dataChanged()
	signal errorOccurred(var error)
	signal selectionChanged()

	// =============================================
	// DATA FETCHING API
	// =============================================

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

	// =============================================
	// SELECTION API — SINGLE SOURCE OF TRUTH
	// =============================================

	/*!
		Returns whether the given item ID is currently selected.
		\param id Item ID.
	*/
	function isItemSelected(id){
		var sid = String(id || "")
		return sid !== "" && __internal.selectedIds[sid] === true
	}

	/*!
		Returns an array of all selected item IDs.
	*/
	function getSelectedIds(){
		var result = []
		for (var k in __internal.selectedIds){
			if (__internal.selectedIds[k]){
				result.push(k)
			}
		}
		return result
	}

	/*!
		Returns an array of resolved item objects for currently selected IDs.
		May be partial — not all selected IDs are guaranteed to have resolved data.
		Only returns items whose ID is in \c selectedIds (source of truth).
	*/
	function getSelectedItems(){
		var result = []
		for (var k in __internal.selectedIds){
			if (__internal.selectedIds[k] && __internal.selectedItems[k]){
				result.push(__internal.selectedItems[k])
			}
		}
		return result
	}

	/*!
		Returns the number of currently selected IDs.
	*/
	function getSelectedCount(){
		var count = 0
		for (var k in __internal.selectedIds){
			if (__internal.selectedIds[k]){
				count++
			}
		}
		return count
	}

	/*!
		Returns true if any selected ID does not yet have resolved item data
		in the selectedItems cache. Useful for UI to show placeholders.
	*/
	function hasUnresolvedItems(){
		for (var k in __internal.selectedIds){
			if (__internal.selectedIds[k] && !__internal.selectedItems[k]){
				return true
			}
		}
		return false
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
		Returns the type identifier for a selected item by ID, if known.
		\param id Item ID.
		\return Type ID string or empty string if not resolved.
	*/
	function getSelectedItemTypeId(id){
		var sid = String(id || "")
		if (sid === ""){
			return ""
		}
		var item = __internal.selectedItems[sid]
		return item ? String(item.typeId || "") : ""
	}

	/*!
		Toggles the selection state of an item.
		In multiSelect mode: adds if not selected, removes if selected.
		In single-select mode: replaces the selection.
		\param id Item ID.
		\param item Full normalized item object { id, title, ... } for the data cache.
	*/
	function toggleItem(id, item){
		var sid = String(id || "")
		if (sid === ""){
			return
		}

		if (root.multiSelect){
			if (__internal.selectedIds[sid]){
				__removeFromSelection(sid)
			}
			else {
				__addToSelection(sid, item)
			}
		}
		else {
			root.setSingleSelection(sid, item)
		}
	}

	/*!
		Sets a single selection, replacing any previous selection.
		\param id Item ID.
		\param item Full normalized item object.
	*/
	function setSingleSelection(id, item){
		var sid = String(id || "")
		if (sid === ""){
			return
		}

		var newIds = ({})
		newIds[sid] = true
		__internal.selectedIds = newIds

		var newItems = ({})
		if (item){
			newItems[sid] = item
		}
		__internal.selectedItems = newItems
		root.selectionChanged()
	}

	/*!
		Clears the entire selection (both IDs and data cache).
	*/
	function clearSelection(){
		__internal.selectedIds = ({})
		__internal.selectedItems = ({})
		root.selectionChanged()
	}

	/*!
		Sets preselected IDs and resolves their data.
		Populates selectedIds immediately. Cleans stale entries from selectedItems,
		keeping only items that match the new selection. For missing items,
		calls \c fetchByIds() to resolve from server.
		\param ids Array of item IDs to preselect.
	*/
	function setPreselectedIds(ids){
		var selected = ({})
		var list = ids || []
		for (var i = 0; i < list.length; i++){
			var sid = String(list[i])
			if (sid !== ""){
				selected[sid] = true
			}
		}
		__internal.selectedIds = selected

		// Clean stale entries from selectedItems — keep only items in new selection
		var cleanedItems = ({})
		for (var j in __internal.selectedItems){
			if (selected[j] && __internal.selectedItems[j]){
				cleanedItems[j] = __internal.selectedItems[j]
			}
		}
		__internal.selectedItems = cleanedItems

		// Find IDs that are not yet resolved in selectedItems
		var missingIds = []
		for (var k in selected){
			if (!cleanedItems[k]){
				missingIds.push(k)
			}
		}

		if (missingIds.length > 0){
			__internal.fetchByIdsRequestId++
			root.executeFetchByIds(__internal.fetchByIdsRequestId, missingIds)
		}
		else {
			root.selectionChanged()
		}
	}

	/*!
		Pre-populates the data cache for a known item without changing selection state.
		Useful when caller already has item data (e.g. from a previous session).
		\param id Item ID.
		\param item Full normalized item object.
	*/
	function addKnownItem(id, item){
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

	// =============================================
	// PROTECTED — subclass must override
	// =============================================

	/*!
		Override in subclass to perform actual data fetching.
		When results arrive, call \c onRequestSuccess(requestId, items).
		On error, call \c onRequestError(requestId, message, code).
	*/
	function executeRequest(requestId, count, offset, filter){
		console.warn("executeRequest() should be implemented in a subclass")
		root.onRequestSuccess(requestId, [])
	}

	/*!
		Override in subclass to fetch specific items by IDs.
		When results arrive, call \c onFetchByIdsSuccess(requestId, items).
		On error, call \c onFetchByIdsError(requestId, message).
	*/
	function executeFetchByIds(requestId, ids){
		console.warn("executeFetchByIds() should be implemented in a subclass")
		root.onFetchByIdsSuccess(requestId, [])
	}

	// =============================================
	// PROTECTED — called by subclass on response
	// =============================================

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
		Merges resolved items into selectedItems data cache.
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
		root.selectionChanged()
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
		// Emit selectionChanged even on failure so UI updates with partial data
		root.selectionChanged()
	}

	// =============================================
	// PRIVATE
	// =============================================

	function __addToSelection(sid, item){
		var updatedIds = ({})
		for (var k in __internal.selectedIds){
			updatedIds[k] = __internal.selectedIds[k]
		}
		updatedIds[sid] = true
		__internal.selectedIds = updatedIds

		if (item){
			var updatedItems = ({})
			for (var j in __internal.selectedItems){
				updatedItems[j] = __internal.selectedItems[j]
			}
			updatedItems[sid] = item
			__internal.selectedItems = updatedItems
		}
		root.selectionChanged()
	}

	function __removeFromSelection(sid){
		var updatedIds = ({})
		for (var k in __internal.selectedIds){
			if (k !== sid){
				updatedIds[k] = __internal.selectedIds[k]
			}
		}
		__internal.selectedIds = updatedIds

		var updatedItems = ({})
		for (var j in __internal.selectedItems){
			if (j !== sid){
				updatedItems[j] = __internal.selectedItems[j]
			}
		}
		__internal.selectedItems = updatedItems
		root.selectionChanged()
	}

	// --- Private state ---
	property QtObject __internal: QtObject {
		// Data state
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
		// Selection state
		property var selectedIds: ({})
		property var selectedItems: ({})
	}
}
