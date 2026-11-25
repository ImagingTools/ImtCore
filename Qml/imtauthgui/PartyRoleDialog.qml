// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * PartyRoleDialog
 *
 * Creates or changes a role of the parties (PartyRoleView). Save stays enabled only for a valid form;
 * the dialog stays open until the server has stored the role, a refused one is shown and can be corrected.
 * An archived role is restored here; a system role is shown read only.
 */
Dialog {
	id: roleDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))
	title: roleDialog.__role === null ? qsTr("New role") : (roleDialog.__role.name || qsTr("Role"))
	notClosingButtons: Enums.save | Enums.apply

	// The role edited, "" for a new one
	property string itemId: ""
	property string tenantId: ""
	// ContractsApiClient
	property var contractsApiClient: null
	property bool canEdit: false

	readonly property var __role: roleDialog.__findRole(roleDialog.itemId)
	readonly property bool __isEditable: roleDialog.canEdit
		&& (roleDialog.__role === null || (!roleDialog.__role.isSystem && roleDialog.__role.ownerTenantId === roleDialog.tenantId))
	readonly property bool __isArchived: roleDialog.__role !== null && roleDialog.__role.isArchived === true
	property bool __isSaving: false

	function __findRole(roleId) {
		var roles = roleDialog.contractsApiClient ? roleDialog.contractsApiClient.partyRoles : []
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].roleId === roleId)
				return roles[i]
		}
		return null
	}

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		if (roleDialog.__isEditable && !roleDialog.__isArchived)
			addButton(Enums.save, qsTr("Save"), roleDialog.__role !== null, false, "primary")
		if (roleDialog.__isEditable && roleDialog.__isArchived)
			addButton(Enums.apply, qsTr("Restore"), true, false, "primary")
		addButton(Enums.cancel, roleDialog.__isEditable && !roleDialog.__isArchived ? qsTr("Cancel") : qsTr("Close"), true)
		if (roleDialog.contentItem)
			roleDialog.contentItem.init()
	}

	onFinished: {
		var content = roleDialog.contentItem
		if (!content || roleDialog.__isSaving)
			return
		if (buttonId == Enums.save && content.form.save())
			roleDialog.__isSaving = true
		else if (buttonId == Enums.apply && roleDialog.__role) {
			roleDialog.__isSaving = true
			roleDialog.contractsApiClient.setPartyRoleArchived(roleDialog.tenantId, roleDialog.__role.roleId, false)
		}
	}

	Connections {
		target: roleDialog.contractsApiClient

		function onCatalogItemSaved(catalog, itemId) {
			if (catalog === "PartyRoles" && roleDialog.__isSaving) {
				roleDialog.__isSaving = false
				roleDialog.finished(Enums.close)
			}
		}

		function onCommandFailed(command, errorMessage) {
			if (command.indexOf("PartyRole") >= 0)
				roleDialog.__isSaving = false
		}
	}

	contentComp: Component {
		Item {
			id: roleContent

			width: roleDialog.width
			height: Math.min(roleForm.preferredHeight, ModalDialogManager.activeView.height - 200)

			property alias form: roleForm
			readonly property bool canSave: roleForm.isChangeable && roleForm.isValid && !roleDialog.__isSaving

			onCanSaveChanged: {
				if (roleDialog.buttons)
					roleDialog.buttons.setButtonState(Enums.save, roleContent.canSave)
			}

			function init() {
				if (roleDialog.buttons)
					roleDialog.buttons.setButtonState(Enums.save, roleContent.canSave)
			}

			PartyRoleView {
				id: roleForm

				anchors.fill: parent

				tenantId: roleDialog.tenantId
				role: roleDialog.__role
				contractsApiClient: roleDialog.contractsApiClient
				canEdit: roleDialog.canEdit
			}
		}
	}
}
