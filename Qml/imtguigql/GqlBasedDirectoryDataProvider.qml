// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthTenantDirectorySdl 1.0

/**
 * GqlBasedDirectoryDataProvider
 *
 * The directory of the organizations page by page over the collection "DirectoryProfiles" (DirectoryProfilesList),
 * for a SimpleCollectionPage. The server gives the listed organizations matching the text only: an empty text gives none.
 * An item: id, title (the name), industry, country, offeredRoles, description, website, acceptsInvitationsFromUnknown.
 */
FilterableSelectDataProvider {
	id: root

	collectionId: "DirectoryProfiles"
	pageSize: 50

	property string permissionPath: ""

	property int __pendingRequestId: 0

	property DirectoryProfilesListInput __listInput: DirectoryProfilesListInput {}

	property CollectionFilter __filter: CollectionFilter {
		Component.onCompleted: {
			setFilteringInfoIds(["tenantName"])
		}
	}

	function __normalizeItems(items) {
		var result = []
		var count = items ? items.getItemsCount() : 0
		for (var i = 0; i < count; i++) {
			result.push({"id": items.getData("m_id", i) || "",
						 "title": items.getData("m_tenantName", i) || "",
						 "industry": items.getData("m_industry", i) || "",
						 "country": items.getData("m_country", i) || "",
						 "offeredRoles": items.getData("m_offeredRoles", i) || "",
						 "description": items.getData("m_description", i) || "",
						 "website": items.getData("m_website", i) || "",
						 "acceptsInvitationsFromUnknown": items.getData("m_acceptsInvitationsFromUnknown", i) === true})
		}
		return result
	}

	property GqlSdlRequestSender __listRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantDirectorySdlCommandIds.s_directoryProfilesList
		sdlObjectComp: Component {
			DirectoryProfilesListPayload {
				onFinished: {
					root.onRequestSuccess(root.__pendingRequestId, root.__normalizeItems(m_items))
				}
			}
		}

		onFinished: {
			if (status === -1)
				root.onRequestError(root.__pendingRequestId, qsTr("The directory could not be searched"), "GQL_ERROR")
		}
	}

	// Without a text the server gives nothing: the request is not sent
	function executeRequest(requestId, count, offset, filter) {
		var text = (filter || "").trim()
		if (text === "") {
			root.onRequestSuccess(requestId, [])
			return
		}
		root.__pendingRequestId = requestId
		if (!root.__listInput.hasViewParams())
			root.__listInput.emplaceViewParams()
		var viewParams = root.__listInput.m_viewParams
		viewParams.m_offset = offset || 0
		viewParams.m_count = count || root.pageSize
		root.__filter.setTextFilter(text)
		viewParams.m_filterModel = root.__filter
		root.__listRequest.send(root.__listInput)
	}

	function executeFetchByIds(requestId, ids) {
		root.onFetchByIdsSuccess(requestId, [])
	}
}
