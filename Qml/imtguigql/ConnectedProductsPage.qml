// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtguigql 1.0

/**
 * Composes the page of the connected products with its GraphQL services.
 */
ConnectedProductsView {
	id: page

	documentManager: gqlDocumentService
	collectionViewComp: productListComp

	GqlBasedCollectionDocumentService {
		id: gqlDocumentService
		collectionId: "ConnectedProducts"
	}

	GqlBasedServerTokenApiClient {
		id: gqlTokenApiClient
	}

	GqlBasedProductPermissionStatusProvider {
		id: gqlPermissionStatusProvider
	}

	GqlBasedPermissionsProvider {
		id: gqlPermissionsProvider
	}

	Component.onCompleted: {
		gqlDocumentService.registerDocumentViewData("ConnectedProduct", "Editor", productEditorComp, productControllerComp)
	}

	Component {
		id: productEditorComp

		ConnectedProductView {
			tokenApiClient: gqlTokenApiClient
			permissionStatusProvider: gqlPermissionStatusProvider
			permissionsProvider: gqlPermissionsProvider
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: "ConnectedProduct"
				}
			}
		}
	}

	Component {
		id: productControllerComp

		ConnectedProductDocumentController {
			documentManager: gqlDocumentService
		}
	}

	Component {
		id: productListComp

		ConnectedProductCollectionView {
			documentManager: gqlDocumentService
			contentColor: Style.baseColor
		}
	}
}
