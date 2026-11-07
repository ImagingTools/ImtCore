// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtguigql 1.0

/**
 * The page of the connected products with its GraphQL services.
 */
Item {
	id: page

	anchors.fill: parent

	GqlBasedCollectionDocumentService {
		id: gqlDocumentService
		collectionId: "ConnectedProducts"
	}

	MultiDocumentCollectionView {
		anchors.fill: parent
		documentManager: gqlDocumentService
		historyPath: "ConnectedProducts"
		contentColor: Style.baseColor
		pageIcon: "Icons/Product"

		Component.onCompleted: {
			setCollectionViewComp(qsTr("Products"), productListComp)
		}
	}

	Component.onCompleted: {
		gqlDocumentService.registerDocumentViewData("ConnectedProduct", "Editor", productEditorComp, productControllerComp)
	}

	Component {
		id: productEditorComp

		ConnectedProductView {
			tokenApiClient: gqlTokenApiClient
			permissionsProvider: gqlPermissionsProvider
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: "ConnectedProduct"
				}
			}

			// Each opened product has its own services: their models and signals belong to one product
			GqlBasedServerTokenApiClient {
				id: gqlTokenApiClient
			}

			GqlBasedPermissionsProvider {
				id: gqlPermissionsProvider
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
