// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * PartyRoleView
 *
 * The form of a role of the parties: its name, where it is used and the role of the other side of a link.
 * A system role and a role of another organization are shown read only. The form of PartyRoleDialog.
 */
Item {
	id: roleView

	property string tenantId: ""
	// The role edited, null for a new one
	property var role: null
	// ContractsApiClient
	property var contractsApiClient: null
	property bool canEdit: false

	readonly property bool isNew: roleView.role === null
	readonly property bool isEditable: roleView.canEdit && (roleView.isNew || (!roleView.role.isSystem && roleView.role.ownerTenantId === roleView.tenantId))
	readonly property bool isArchived: roleView.role !== null && roleView.role.isArchived === true
	// An archived role is restored before it is changed
	readonly property bool isChangeable: roleView.isEditable && !roleView.isArchived
	readonly property bool isValid: nameInput.text.trim() !== "" && (partySwitch.checked || linkSwitch.checked)
	readonly property real preferredHeight: formColumn.height + 2 * Style.marginXL

	Component.onCompleted: {
		roleView.fill()
	}

	// A dialog gives the role after the form is created
	onRoleChanged: {
		roleView.fill()
	}

	// The roles that can be the other side of a link of this one, the role itself for a symmetric one
	function pairedRoles() {
		var result = []
		var roles = roleView.contractsApiClient ? roleView.contractsApiClient.partyRoles : []
		var pairedRoleId = roleView.role ? (roleView.role.pairedRoleId || "") : ""
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].isArchived && roles[i].roleId !== pairedRoleId)
				continue
			result.push({"roleId": roles[i].roleId, "name": roles[i].name})
		}
		return result
	}

	function fill() {
		var role = roleView.role
		nameInput.text = role ? (role.name || "") : ""
		descriptionInput.text = role ? (role.description || "") : ""
		partySwitch.checked = role ? role.isPartyRole === true : true
		linkSwitch.checked = role ? role.isLinkRole === true : true
		pairedCB.currentIndex = pairedModel.fill(roleView.pairedRoles(), "roleId", "name", role ? (role.pairedRoleId || "") : "")
	}

	// Sends the role; false, with the errors shown, when the form is not valid
	function save() {
		if (!roleView.contractsApiClient || !roleView.isChangeable)
			return false
		if (!roleView.isValid) {
			nameInput.revealError()
			return false
		}
		var role = roleView.role
		roleView.contractsApiClient.savePartyRole(roleView.tenantId, {
			"roleId": role ? role.roleId : "",
			"name": nameInput.text.trim(),
			"description": descriptionInput.text.trim(),
			"isPartyRole": partySwitch.checked,
			"isLinkRole": linkSwitch.checked,
			"pairedRoleId": pairedModel.idAt(pairedCB.currentIndex),
			"templateProductId": role ? (role.templateProductId || "") : "",
			"templatePermissionIds": role ? (role.templatePermissionIds || []) : []})
		return true
	}

	Connections {
		target: roleView.contractsApiClient

		function onPartyRolesReceived() {
			pairedCB.currentIndex = pairedModel.fill(roleView.pairedRoles(), "roleId", "name", pairedModel.idAt(pairedCB.currentIndex))
		}
	}

	ChoiceModel {
		id: pairedModel
	}

	RegularExpressionValidator {
		id: nameValidator
		regularExpression: /^(?!\s*$).+/
	}

	CustomScrollbar {
		z: formFlickable.z + 1
		anchors.right: parent.right
		anchors.top: formFlickable.top
		anchors.bottom: formFlickable.bottom
		secondSize: Style.marginM
		targetItem: formFlickable
	}

	Flickable {
		id: formFlickable

		anchors.fill: parent
		contentHeight: formColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: formColumn

			y: Style.marginXL
			x: Style.marginXL
			width: formFlickable.width - 2 * Style.marginXL

			spacing: Style.marginL

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: !roleView.isEditable
					  ? (roleView.role && roleView.role.isSystem
						 ? qsTr("A system role is the same for every organization and is read only. Create your own role if you need another one.")
						 : qsTr("You can read the role but not change it."))
					  : roleView.isArchived
						? qsTr("The role is archived: the contracts keep it, new contracts are not offered it. Restore it to use it again.")
						: qsTr("A role names what an organization is in a contract, e.g. Supplier, or towards the other side of a link, e.g. Customer of a supplier.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			GroupElementView {
				width: parent.width

				TextInputElementView {
					id: nameInput
					isRequired: true
					showErrorWhenInvalid: true
					deferErrorUntilEdited: true
					errorText: qsTr("Enter the name of the role")
					textInputValidator: nameValidator
					name: qsTr("Name")
					readOnly: !roleView.isChangeable
					placeHolderText: qsTr("e.g. Distributor")
				}

				TextInputElementView {
					id: descriptionInput
					name: qsTr("Description")
					readOnly: !roleView.isChangeable
					placeHolderText: qsTr("Optional")
				}

				SwitchElementView {
					id: partySwitch
					name: qsTr("Role of a party")
					description: qsTr("Offered when a party is added to a contract")
					readOnly: !roleView.isChangeable
				}

				SwitchElementView {
					id: linkSwitch
					name: qsTr("Role in a link")
					description: partySwitch.checked || linkSwitch.checked
								 ? qsTr("Offered for the sides of a link between two parties")
								 : qsTr("A role is used for a party, in a link or both: turn one of them on")
					descriptionColor: partySwitch.checked || linkSwitch.checked ? Style.inactiveTextColor : Style.errorTextColor
					readOnly: !roleView.isChangeable
				}

				ClearableComboBoxElementView {
					id: pairedCB
					name: qsTr("Paired role")
					description: qsTr("The role of the other side of a link, e.g. Customer for Supplier; the role itself for a symmetric one, e.g. Partner")
					model: pairedModel
					changeable: roleView.isChangeable
				}
			}
		}
	}
}
