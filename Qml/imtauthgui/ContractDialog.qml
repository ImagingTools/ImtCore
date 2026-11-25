// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractDialog
 *
 * The card of a contract (ContractView) in a dialog: its steps are the commands of the card,
 * its parts the pages of its MultiPageView. The dialog closes with Close, or by itself once a draft is removed.
 */
Dialog {
	id: contractDialog

	width: Math.max(Style.sizeHintXL, Math.min(ModalDialogManager.activeView.width - 80, Style.sizeHintXXXL + Style.sizeHintS))
	title: contractDialog.__title !== "" ? contractDialog.__title : qsTr("Contract")

	// The contract, "" for a new one
	property string itemId: ""
	property string tenantId: ""
	property string tenantName: ""
	// ContractsApiClient
	property var contractsApiClient: null
	// TenantManagementApiClient
	property var tenantApiClient: null
	property var stateManager: null

	property string __title: ""

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.close, qsTr("Close"), true)
		if (contractDialog.contentItem)
			contractDialog.contentItem.view.start()
	}

	contentComp: Component {
		Item {
			width: contractDialog.width
			height: Math.max(Style.sizeHintL, ModalDialogManager.activeView.height - 180)

			property alias view: contractView

			ContractView {
				id: contractView

				anchors.fill: parent

				tenantId: contractDialog.tenantId
				tenantName: contractDialog.tenantName
				contractId: contractDialog.itemId
				contractsApiClient: contractDialog.contractsApiClient
				tenantApiClient: contractDialog.tenantApiClient
				stateManager: contractDialog.stateManager

				onTitleChanged: {
					contractDialog.__title = title
				}

				onCloseRequested: {
					contractDialog.finished(Enums.close)
				}
			}
		}
	}
}
