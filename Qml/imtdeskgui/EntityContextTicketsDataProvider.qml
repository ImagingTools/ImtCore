// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtdeskImtDeskSdl 1.0

/*!
	\qmltype EntityContextTicketsDataProvider
	\inqmlmodule imtdeskgui
	\brief Paged, server-filtered provider of the tickets attached to one entity.

	Backs \c EntityContextTicketsPanel (and anything else showing an entity's
	support tickets) with the contract \c SimpleCollectionTable expects:
	\c fetch / \c fetchMore / \c retry plus \c items, \c isInitialLoading,
	\c isPageLoading and \c error.

	Text filtering and paging are done by the server
	(\c CEntityContextTicketsControllerComp), so the panel never holds the whole
	ticket list. \c totalCount reports how many tickets match the filter in
	total, not how many are currently loaded.

	\sa FilterableSelectDataProvider, SimpleCollectionTable
*/
FilterableSelectDataProvider {
	id: root

	property string context: ""
	property string entityType: ""
	property string entityId: ""
	readonly property bool hasEntityContext: root.entityType !== "" && root.entityId !== ""

	//! Tickets matching the current filter server-side, ignoring paging.
	readonly property int totalCount: __state.totalCount

	pageSize: 25

	function getHeaders(){
		return {}
	}

	function refresh(){
		if (root.hasEntityContext){
			root.refetch()
		}
	}

	property QtObject __state: QtObject {
		property int pendingRequestId: 0
		property int totalCount: 0
	}

	// Only `textFilter` is read by the server controller, so no field ids are
	// configured here - a fieldsFilter would be built and then ignored.
	property CollectionFilter __filter: CollectionFilter {}

	property EntityContextTicketsInput __input: EntityContextTicketsInput {}

	property GqlSdlRequestSender __request: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtdeskImtDeskSdlCommandIds.s_entityContextTickets

		sdlObjectComp: Component {
			EntityContextTicketsPayload {
				onFinished: {
					root.__state.totalCount = m_totalCount ? m_totalCount : 0
					root.onRequestSuccess(root.__state.pendingRequestId, root.__normalizeTickets(m_items))
				}
			}
		}

		onFinished: {
			if (status < 0){
				root.onRequestError(root.__state.pendingRequestId, qsTr("Failed to load tickets"), "GQL_ERROR")
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	function executeRequest(requestId, count, offset, filter){
		if (!root.hasEntityContext){
			root.onRequestError(requestId, qsTr("No entity selected"), "EMPTY_ENTITY")
			return
		}

		root.__state.pendingRequestId = requestId

		root.__input.m_entityType = root.entityType
		root.__input.m_entityId = root.entityId

		if (!root.__input.hasViewParams()){
			root.__input.emplaceViewParams()
		}

		var viewParams = root.__input.m_viewParams
		viewParams.m_offset = offset || 0
		viewParams.m_count = count || root.pageSize

		root.__filter.clearAllFilters(true)
		if (filter && filter !== ""){
			root.__filter.setTextFilter(filter)
			viewParams.m_filterModel = root.__filter
		}
		else {
			viewParams.m_filterModel = null
		}

		root.__request.send(root.__input)
	}

	function executeFetchByIds(requestId, ids){
		// This provider only ever feeds a list view; there is no preselection to resolve.
		root.onFetchByIdsSuccess(requestId, [])
	}

	// `title` plus the structured fields below are ready for either
	// SimpleCollectionTable's own default row (bold title only, here) or a
	// custom `delegateComponent` (EntityContextTicketsPanel uses its own
	// ticket row, showing status/priority/assignee/createdAt individually).
	function __normalizeTickets(itemsModel){
		var normalized = []
		if (!itemsModel){
			return normalized
		}

		var count = itemsModel.getItemsCount()
		for (var i = 0; i < count; i++){
			var id = itemsModel.getData("m_id", i)
			if (!id){
				continue
			}

			var number = itemsModel.getData("m_number", i)
			var ticketTitle = itemsModel.getData("m_title", i)

			normalized.push({
				id: String(id),
				title: ticketTitle ? String(ticketTitle) : "",
				number: (number !== undefined && number !== null) ? Number(number) : 0,
				status: String(itemsModel.getData("m_status", i) || ""),
				priority: String(itemsModel.getData("m_priority", i) || ""),
				assignee: String(itemsModel.getData("m_assignee", i) || ""),
				createdAt: String(itemsModel.getData("m_createdAt", i) || "")
			})
		}


		return normalized
	}
}
