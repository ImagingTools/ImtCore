// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtauthConnectedProductsSdl 1.0
import imtguigql 1.0

RemoteCollectionView {
	id: productCollectionView

	collectionId: "ConnectedProducts"
	visibleMetaInfo: false
	documentCollectionFilter: null
	showRemoteChangesAlert: false

	additionalFieldIds: [ConnectedProductItemDataTypeMetaInfo.s_productId]

	property var documentManager: null

	commandsDelegateComp: Component {
		DocCollectionViewDelegate {
			collectionView: productCollectionView
			documentManager: productCollectionView.documentManager

			removeDialogTitle: qsTr("Removing a product")
			removeMessage: qsTr("Remove the selected product? All tokens of its servers are revoked. Its roles stay.")

			Component.onCompleted: {
				registerDocumentType("ConnectedProduct", qsTr("Product"))
			}

			function updateStateBaseCommands(selection, commandsController, elementsModel){
				if (commandsController){
					commandsController.setCommandIsEnabled("Remove", selection.length > 0)
					commandsController.setCommandIsEnabled("Edit", selection.length === 1)
				}
			}
		}
	}

	Component.onCompleted: {
		table.setSortingInfo(ConnectedProductItemDataTypeMetaInfo.s_productName, "ASC")
	}

	// The state of a server changes without a change of the collection
	onVisibleChanged: {
		if (productCollectionView.visible){
			productCollectionView.doUpdateGui()
		}
	}
}
