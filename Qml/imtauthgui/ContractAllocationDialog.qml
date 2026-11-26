// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractAllocationDialog
 *
 * The permissions this organization gives a party it is linked with by the contract: chosen among the permissions
 * it holds itself. The party receiving them gives them to its groups; their members then work in this organization.
 *
 * The result is read in onFinished from contentItem: granteeTenantId, permissionIds, validUntil(), comment.
 */
Dialog {
	id: allocationDialog

	width: Math.max(Style.sizeHintL, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// The parties this organization is linked with: [{tenantId, tenantName}]
	property var partners: []
	// TenantManagementApiClient: the permissions of the product arrive after the dialog opens
	property var tenantApiClient: null
	// The permission scope of this organization: the ids of its Available permissions
	property var scopeIds: []

	Connections {
		target: allocationDialog.tenantApiClient

		function onAllPermissionsReceived() {
			if (allocationDialog.contentItem)
				allocationDialog.contentItem.rebuildPermissions()
		}
	}

	// The permissions of the product the organization holds, as PermissionsTableView.rebuildFromFlatArray takes them
	function ownGroups() {
		var allowed = {}
		for (var i = 0; i < allocationDialog.scopeIds.length; i++)
			allowed[String(allocationDialog.scopeIds[i])] = true
		var groups = allocationDialog.tenantApiClient ? (allocationDialog.tenantApiClient.allPermissions || []) : []
		var result = []
		for (var g = 0; g < groups.length; g++) {
			var entries = groups[g].entries || []
			var allowedEntries = []
			for (var e = 0; e < entries.length; e++) {
				if (allowed[String(entries[e].permissionId)])
					allowedEntries.push(entries[e])
			}
			if (allowedEntries.length > 0)
				result.push({"groupId": groups[g].groupId, "groupName": groups[g].groupName, "entries": allowedEntries})
		}
		return result
	}

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("Give"), false, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (allocationDialog.tenantApiClient)
			allocationDialog.tenantApiClient.fetchAllPermissions()
		if (allocationDialog.contentItem)
			allocationDialog.contentItem.init()
	}

	function __updateOk(content) {
		if (allocationDialog.buttons)
			allocationDialog.buttons.setButtonState(Enums.ok, content.granteeTenantId !== "" && content.permissionIds.length > 0)
	}

	contentComp: Component {
		Item {
			id: allocationContent

			width: allocationDialog.width
			height: allocationColumn.height + 2 * Style.marginXL

			readonly property string granteeTenantId: partnerModel.idAt(partnerCB.currentIndex)
			readonly property string granteeName: partnerModel.nameAt(partnerCB.currentIndex)
			property var permissionIds: []
			readonly property string comment: commentInput.text.trim()

			// The time of the API of the end, empty while the contract is in force
			function validUntil() {
				return ContractTexts.pickerIso(untilInput)
			}

			function rebuildPermissions() {
				allocationContent.permissionIds = []
				permissionsView.rebuildFromFlatArray(allocationDialog.ownGroups())
			}

			function init() {
				var partners = allocationDialog.partners
				partnerCB.currentIndex = partnerModel.fill(partners, "tenantId", "tenantName", partners.length > 0 ? partners[0].tenantId : "")
				allocationContent.rebuildPermissions()
			}

			onPermissionIdsChanged: {
				allocationDialog.__updateOk(allocationContent)
			}

			onGranteeTenantIdChanged: {
				allocationDialog.__updateOk(allocationContent)
			}

			ChoiceModel {
				id: partnerModel
			}

			Column {
				id: allocationColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				GroupElementView {
					width: parent.width

					ComboBoxElementView {
						id: partnerCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose the party receiving the permissions")
						name: qsTr("To")
						model: partnerModel
					}
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
					text: qsTr("Choose among the permissions your organization holds. While the contract is in force, %1 gives them to its groups, and their members work in your organization with them.")
						.arg(allocationContent.granteeName !== "" ? allocationContent.granteeName : qsTr("the party"))
				}

				// The permissions are the main part of the dialog: the table takes the room of the window
				PermissionsTableView {
					id: permissionsView
					width: parent.width
					height: Math.max(Style.sizeHintS, ModalDialogManager.activeView.height - 560)
					showDetails: false
					emptyText: qsTr("Your organization holds no permissions to give. They are chosen on the page Available permissions.")

					onSelectionChanged: {
						allocationContent.permissionIds = permissionsView.getCheckedIds(false)
					}
				}

				GroupElementView {
					width: parent.width

					DateTimePickerElementView {
						id: untilInput
						name: qsTr("Until")
						description: qsTr("Empty: while the contract is in force")
						canShowCurrentDate: false
					}

					TextInputElementView {
						id: commentInput
						name: qsTr("Comment")
						placeHolderText: qsTr("Optional")
					}
				}
			}
		}
	}
}
