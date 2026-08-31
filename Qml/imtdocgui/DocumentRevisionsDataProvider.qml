// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtbaseDocumentRevisionSdl 1.0

/*!
	\qmltype DocumentRevisionsDataProvider
	\inqmlmodule imtdocgui
	\brief Paged, server-filtered provider of one document's revision history.

	Backs \c DocumentHistoryView with the contract \c SimpleCollectionTable
	expects: \c fetch / \c fetchMore / \c retry plus \c items,
	\c isInitialLoading, \c isPageLoading and \c error.

	Text filtering and paging are done by the server
	(\c CDocumentRevisionControllerComp), so a document with a long history is
	never loaded in full. \c totalCount reports how many revisions match the
	filter in total; \c activeRevision is the revision currently in effect and
	is reported even when it falls outside the loaded page.

	\sa FilterableSelectDataProvider, SimpleCollectionTable
*/
FilterableSelectDataProvider {
	id: root

	property string context: ""
	property string documentId: ""
	property string collectionId: ""
	readonly property bool hasDocumentContext: root.documentId !== "" && root.collectionId !== ""

	//! Revisions matching the current filter server-side, ignoring paging.
	readonly property int totalCount: __state.totalCount
	//! Revision currently in effect for the document.
	readonly property int activeRevision: __state.activeRevision

	pageSize: 25

	function getHeaders(){
		return {}
	}

	function refresh(){
		if (root.hasDocumentContext){
			root.refetch()
		}
	}

	property QtObject __state: QtObject {
		property int pendingRequestId: 0
		property int totalCount: 0
		property int activeRevision: 0
	}

	// Only `textFilter` is read by the server controller, so no field ids are
	// configured here - a fieldsFilter would be built and then ignored.
	property CollectionFilter __filter: CollectionFilter {}

	property GetRevisionInfoListInput __input: GetRevisionInfoListInput {}

	property GqlSdlRequestSender __request: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_getRevisionInfoList

		sdlObjectComp: Component {
			RevisionInfoList {
				onFinished: {
					root.__state.totalCount = m_totalCount ? m_totalCount : 0
					root.__state.activeRevision = m_activeRevision ? m_activeRevision : 0
					root.onRequestSuccess(root.__state.pendingRequestId, root.__normalizeRevisions(m_revisions))
				}
			}
		}

		onFinished: {
			if (status < 0){
				root.onRequestError(root.__state.pendingRequestId, qsTr("Failed to load document history"), "GQL_ERROR")
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	function executeRequest(requestId, count, offset, filter){
		if (!root.hasDocumentContext){
			root.onRequestError(requestId, qsTr("No document selected"), "EMPTY_DOCUMENT")
			return
		}

		root.__state.pendingRequestId = requestId

		root.__input.m_documentId = root.documentId
		root.__input.m_collectionId = root.collectionId

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

	// Revisions have no id of their own, so the revision number is used as one -
	// it is unique per document, which is all the list view needs it for.
	//
	// The structured fields below are ready for a custom `delegateComponent`
	// (DocumentHistoryView uses its own revision row, showing
	// user/timestamp/description/isActive individually).
	function __normalizeRevisions(revisionsModel){
		var normalized = []
		if (!revisionsModel){
			return normalized
		}

		var count = revisionsModel.getItemsCount()
		for (var i = 0; i < count; i++){
			var revision = revisionsModel.getData("m_revision", i)
			if (revision === undefined || revision === null){
				continue
			}

			normalized.push({
				id: String(revision),
				revision: revision,
				user: String(revisionsModel.getData("m_user", i) || ""),
				timestamp: String(revisionsModel.getData("m_timestamp", i) || ""),
				description: String(revisionsModel.getData("m_description", i) || ""),
				isActive: revisionsModel.getData("m_isActive", i) === true
			})
		}


		return normalized
	}
}
