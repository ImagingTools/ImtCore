import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtbaseFilterableSelectSdl 1.0

/*!
	\qmltype FilterableSelectGqlDataProvider
	\inqmlmodule imtguigql
	\brief GQL-based implementation of FilterableSelectDataProvider.

	Uses GqlSdlRequestSender with SDL-generated types to fetch selectable items.
	Normalizes SDL response objects to plain JS { id, title, description, icon, color }
	before passing to the base provider.

	Passes currently selected IDs as \c excludeIds so the server excludes them
	from the result set via \c CDocumentIdFilter with \c CT_NOT_IN.

	Preselected items are resolved via \c knownItems passed on popup open.

	\sa FilterableSelectDataProvider
*/
FilterableSelectDataProvider {
	id: root

	property string tenantId: ""

	function getHeaders(){
		return {}
	}

	property QtObject __gql: QtObject {
		property int pendingRequestId: 0
	}

	// --- Shared normalization helper ---
	function __normalizePayloadItems(m_items){
		var normalized = []
		if (m_items){
			var count = m_items.getItemsCount()
			for (var i = 0; i < count; i++){
				var id = m_items.getData("m_id", i)
				var typeId = m_items.getData("m_typeId", i)
				var name = m_items.getData("m_name", i)
				var desc = m_items.getData("m_description", i)
				var icon = m_items.getData("m_icon", i)
				var color = m_items.getData("m_color", i)
				normalized.push({
					id: id || "",
					typeId: typeId || "",
					title: name || "",
					description: desc || "",
					icon: icon || "",
					color: color || ""
				})
			}
		}
		return normalized
	}

	// --- Main list request ---
	property GetSelectableItemsInput getSelectableItemsInput: GetSelectableItemsInput {}
	property CollectionFilter __filter: CollectionFilter {
		Component.onCompleted: {
			setFilteringInfoIds(["Name", "Description"])
		}
	}

	property GqlSdlRequestSender getSelectableItemsRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseFilterableSelectSdlCommandIds.s_getSelectableItems
		sdlObjectComp: Component {
			GetSelectableItemsPayload {
				onFinished: {
					var rid = root.__gql.pendingRequestId
					root.onRequestSuccess(rid, root.__normalizePayloadItems(m_items))
				}
			}
		}

		onFinished: {
			if (status === -1){
				root.onRequestError(root.__gql.pendingRequestId, qsTr("Failed to load items"), "GQL_ERROR")
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	function executeRequest(requestId, count, offset, filter){
		// Guard against empty collectionId — server cannot handle it
		if (!root.collectionId) {
			root.onRequestError(requestId, qsTr("No collection selected"), "EMPTY_COLLECTION")
			return
		}

		__gql.pendingRequestId = requestId

		getSelectableItemsInput.m_collectionId = root.collectionId

		// Pass tenantId for tenant-scoped filtering
		if (root.tenantId && root.tenantId !== "")
			getSelectableItemsInput.m_tenantId = root.tenantId
		else
			getSelectableItemsInput.m_tenantId = null

		// Pass selected IDs as excludeIds for server-side filtering
		var selectedIds = root.getSelectedIds()
		if (selectedIds.length > 0){
			getSelectableItemsInput.m_excludeIds = selectedIds
		}
		else {
			getSelectableItemsInput.m_excludeIds = null
		}

		if (!getSelectableItemsInput.hasViewParams()){
			getSelectableItemsInput.emplaceViewParams()
		}

		var viewParams = getSelectableItemsInput.m_viewParams
		viewParams.m_offset = offset || 0
		viewParams.m_count = count || 20

		__filter.clearAllFilters(true)
		if (filter && filter !== ""){
			__filter.setTextFilter(filter)
			viewParams.m_filterModel = __filter
		}
		else {
			viewParams.m_filterModel = null
		}

		getSelectableItemsRequest.send(getSelectableItemsInput)
	}

	function executeFetchByIds(requestId, ids){
		// Items are resolved via knownItems passed on popup open.
		// If items are not available, UI gracefully degrades to showing the ID.
		root.onFetchByIdsSuccess(requestId, [])
	}
}
