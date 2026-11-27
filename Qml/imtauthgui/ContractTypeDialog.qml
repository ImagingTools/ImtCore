// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractTypeDialog
 *
 * Creates or changes a type of contracts (ContractTypeView, its parts on the pages of a MultiPageView).
 * The dialog stays open until the server has stored the type, a refused one is shown and can be corrected.
 * An archived type is restored here; a system type is shown read only.
 */
Dialog {
	id: typeDialog

	width: Math.max(Style.sizeHintL, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXXL))
	title: typeDialog.__type === null ? qsTr("New type of contracts") : (typeDialog.__type.name || qsTr("Type of contracts"))
	notClosingButtons: Enums.save | Enums.apply

	// The type edited, "" for a new one
	property string itemId: ""
	property string tenantId: ""
	// ContractsApiClient
	property var contractsApiClient: null
	property bool canEdit: false

	readonly property var __type: typeDialog.__findType(typeDialog.itemId)
	readonly property bool __isEditable: typeDialog.canEdit
		&& (typeDialog.__type === null || (!typeDialog.__type.isSystem && typeDialog.__type.ownerTenantId === typeDialog.tenantId))
	readonly property bool __isArchived: typeDialog.__type !== null && typeDialog.__type.isArchived === true
	property bool __isSaving: false

	function __findType(typeId) {
		var types = typeDialog.contractsApiClient ? typeDialog.contractsApiClient.contractTypes : []
		for (var i = 0; i < types.length; i++) {
			if (types[i].typeId === typeId)
				return types[i]
		}
		return null
	}

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		if (typeDialog.__isEditable && !typeDialog.__isArchived)
			addButton(Enums.save, qsTr("Save"), true, false, "primary")
		if (typeDialog.__isEditable && typeDialog.__isArchived)
			addButton(Enums.apply, qsTr("Restore"), true, false, "primary")
		addButton(Enums.cancel, typeDialog.__isEditable && !typeDialog.__isArchived ? qsTr("Cancel") : qsTr("Close"), true)
	}

	onFinished: {
		var content = typeDialog.contentItem
		if (!content || typeDialog.__isSaving)
			return
		if (buttonId == Enums.save && content.form.save()) {
			typeDialog.__isSaving = true
			typeDialog.buttons.setButtonState(Enums.save, false)
		}
		else if (buttonId == Enums.apply && typeDialog.__type) {
			typeDialog.__isSaving = true
			typeDialog.contractsApiClient.setContractTypeArchived(typeDialog.tenantId, typeDialog.__type.typeId, false)
		}
	}

	Connections {
		target: typeDialog.contractsApiClient

		function onCatalogItemSaved(catalog, itemId) {
			if (catalog === "ContractTypes" && typeDialog.__isSaving) {
				typeDialog.__isSaving = false
				typeDialog.finished(Enums.close)
			}
		}

		function onCommandFailed(command, errorMessage) {
			if (command.indexOf("ContractType") < 0 || !typeDialog.__isSaving)
				return
			typeDialog.__isSaving = false
			if (typeDialog.buttons)
				typeDialog.buttons.setButtonState(Enums.save, true)
		}
	}

	contentComp: Component {
		Item {
			id: typeContent

			width: typeDialog.width
			height: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.height - 220, Style.sizeHintXL))

			property alias form: typeForm

			ContractTypeView {
				id: typeForm

				anchors.fill: parent

				tenantId: typeDialog.tenantId
				contractType: typeDialog.__type
				contractsApiClient: typeDialog.contractsApiClient
				canEdit: typeDialog.canEdit
			}
		}
	}
}
