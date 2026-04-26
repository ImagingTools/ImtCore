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

	Also implements \c executeFetchByIds() for resolving preselected items
	that are not in the current visible dataset.

	\sa FilterableSelectDataProvider
*/
FilterableSelectDataProvider {
	id: root

	function getHeaders(){
		return {}
	}

	property QtObject __gql: QtObject {
		property int pendingRequestId: 0
		property int pendingFetchByIdsRequestId: 0
	}

	// --- Shared normalization helper ---
	function __normalizePayloadItems(m_items){
		var normalized = []
		if (m_items){
			var count = m_items.getItemsCount()
			for (var i = 0; i < count; i++){
				var id = m_items.getData("id", i)
				var name = m_items.getData("name", i)
				var desc = m_items.getData("description", i)
				var icon = m_items.getData("icon", i)
				var color = m_items.getData("color", i)
				normalized.push({
					id: id || "",
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
	property CollectionFilter __filter: CollectionFilter {}

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

	// --- FetchByIds request ---
	property GetSelectableItemsInput __fetchByIdsInput: GetSelectableItemsInput {}

	property GqlSdlRequestSender __fetchByIdsRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseFilterableSelectSdlCommandIds.s_getSelectableItems
		sdlObjectComp: Component {
			GetSelectableItemsPayload {
				onFinished: {
					var rid = root.__gql.pendingFetchByIdsRequestId
					root.onFetchByIdsSuccess(rid, root.__normalizePayloadItems(m_items))
				}
			}
		}

		onFinished: {
			if (status === -1){
				root.onFetchByIdsError(root.__gql.pendingFetchByIdsRequestId, qsTr("Failed to resolve items by IDs"))
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	function executeRequest(requestId, count, offset, filter){
		__gql.pendingRequestId = requestId

		getSelectableItemsInput.m_collectionId = root.collectionId

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
		__gql.pendingFetchByIdsRequestId = requestId

		__fetchByIdsInput.m_collectionId = root.collectionId
		__fetchByIdsInput.m_ids = ids

		__fetchByIdsRequest.send(__fetchByIdsInput)
	}
}
