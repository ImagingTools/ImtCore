import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtbaseFilterableSelectSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

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

	property string context: ""
	function getHeaders(){
		return {}
	}

	/**
	 * Ids that must never be offered, on top of the already selected ones.
	 */
	property var extraExcludeIds: []

	/**
	 * Host-supplied GroupFilter objects sent in viewParams.filterModel with every
	 * request, so a picker can be scoped the same way as the collection behind it.
	 */
	property var groupFilters: []

	property QtObject __gql: QtObject {
		property int pendingRequestId: 0
		property var appliedGroupFilters: []
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
				var params = m_items.getData("m_params", i)
				var normalizedParams = root.__normalizeParameters(params)
				normalized.push({
					id: id || "",
					typeId: typeId || "",
					title: name || "",
					description: desc || "",
					icon: icon || "",
					color: color || "",
					parameters: normalizedParams,
					totalCount: count
				})
			}
		}
		return normalized
	}

	// --- Normalize parameters from SDL ParamsSet to plain JS array ---
	// Parameter.data carries the parameter serialized as a JSON archive, so the
	// readable value has to be decoded with the matching SDL type.
	property TextParam __textParamDecoder: TextParam {}

	function __normalizeParameters(paramsSet){
		var result = []
		if (!paramsSet)
			return result
		var paramsList = paramsSet.m_parameters ? paramsSet.m_parameters : paramsSet
		var count = paramsList.getItemsCount ? paramsList.getItemsCount() : 0
		for (var i = 0; i < count; i++){
			var paramId = paramsList.getData("m_id", i)
			var paramName = paramsList.getData("m_name", i)
			var paramDesc = paramsList.getData("m_description", i)
			var paramData = paramsList.getData("m_data", i)
			result.push({
				id: paramId || "",
				name: paramName || "",
				description: paramDesc || "",
				data: paramData || "",
				value: root.__decodeParameterValue(paramsList.getData("m_typeId", i), paramData)
			})
		}
		return result
	}

	function __decodeParameterValue(typeId, data){
		if (!data || String(typeId) !== "TextParam"){
			return ""
		}
		root.__textParamDecoder.m_text = ""
		root.__textParamDecoder.createFromJson(String(data))
		return String(root.__textParamDecoder.m_text || "")
	}

	// --- Main list request ---
	property GetSelectableItemsInput getSelectableItemsInput: GetSelectableItemsInput {}
	// The server matches the text only against the field ids it is told about, so
	// without them the filter silently matches nothing.
	property CollectionFilter __filter: CollectionFilter {
		Component.onCompleted: {
			setFilteringInfoIds(["Name", "Description"])
		}
	}

	property GqlSdlRequestSender getSelectableItemsRequest: GqlSdlRequestSender {
		context: root.context
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

		// Pass selected IDs as excludeIds for server-side filtering, plus anything the host
		// declared unselectable (a role must not be offered as its own parent).
		var excludeIds = root.getSelectedIds().slice()
		for (var i = 0; i < root.extraExcludeIds.length; i++){
			var extraId = root.extraExcludeIds[i]
			if (extraId && excludeIds.indexOf(extraId) < 0){
				excludeIds.push(extraId)
			}
		}

		if (excludeIds.length > 0){
			getSelectableItemsInput.m_excludeIds = excludeIds
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

		__filter.setTextFilter(filter || "")

		var groupCount = root.__applyGroupFilters()
		viewParams.m_filterModel = ((filter && filter !== "") || groupCount > 0) ? __filter : null

		getSelectableItemsRequest.send(getSelectableItemsInput)
	}

	// Re-attached per request: the host rebuilds its groups as its own state changes.
	function __applyGroupFilters(){
		var previous = root.__gql.appliedGroupFilters
		for (var i = 0; i < previous.length; i++){
			root.__filter.removeGroupFilter(previous[i])
		}

		var groups = root.groupFilters || []
		var applied = []
		for (var j = 0; j < groups.length; j++){
			if (groups[j]){
				root.__filter.addGroupFilter(groups[j])
				applied.push(groups[j])
			}
		}

		root.__gql.appliedGroupFilters = applied
		return applied.length
	}

	function executeFetchByIds(requestId, ids){
		// Items are resolved via knownItems passed on popup open.
		// If items are not available, UI gracefully degrades to showing the ID.
		root.onFetchByIdsSuccess(requestId, [])
	}
}
