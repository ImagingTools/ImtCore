// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtauthProductPermissionCatalogSdl 1.0
import imtauthgui 1.0
import imtguigql 1.0

/**
 * The publication status of a permission tree over GraphQL.
 */
ProductPermissionStatusProvider {
	id: root

	function requestStatus(productId){
		root.__input.m_productId = productId
		root.__request.send(root.__input)
	}

	property ProductPermissionStatusInput __input: ProductPermissionStatusInput {}

	property GqlSdlRequestSender __request: GqlSdlRequestSender {
		gqlCommandId: ImtauthProductPermissionCatalogSdlCommandIds.s_getProductPermissionStatus
		sdlObjectComp: Component {
			ProductPermissionStatus {
				onFinished: {
					root.statusReceived(m_isPublished, m_productVersion, m_publishedAt)
				}
			}
		}

		function onError(message, type){
			root.requestFailed(message)
		}
	}
}
