// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtauthgui 1.0

/**
 * ContractPermissionsPage
 *
 * The permissions the parties give each other within the links of a contract. Each party gives and takes back
 * its own permissions; the one receiving them gives them to its groups while the contract is in force.
 * A page of ContractView: the permissions are the ones of the stored contract.
 */
Item {
	id: permissionsPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __canGive: permissionsPage.editor ? permissionsPage.editor.canChangeOwnPermissions : false
	// The receiver chooses its groups once the contract is sent, so the permissions work from the day it comes into force
	readonly property bool __canChooseGroups: permissionsPage.editor !== null && permissionsPage.editor.canDistribute && permissionsPage.editor.isSubmitted
		&& ["Cancelled", "Expired", "Terminated", "Superseded"].indexOf(permissionsPage.editor.contractState) < 0

	signal addAllocationRequested()

	readonly property var allocationRows: permissionsPage.__allocationRows(permissionsPage.editor && permissionsPage.editor.contract ? (permissionsPage.editor.contract.allocations || []) : [],
																		  permissionsPage.editor ? permissionsPage.editor.privateInfo : null,
																		  permissionsPage.__catalog)
	// The permissions of the product, for their names
	readonly property var __catalog: permissionsPage.editor && permissionsPage.editor.tenantApiClient ? permissionsPage.editor.tenantApiClient.allPermissions : []

	Component.onCompleted: {
		// The names of the permissions; one request at a time, the provider keeps one pending request
		if (permissionsPage.editor && permissionsPage.editor.tenantApiClient)
			permissionsPage.editor.tenantApiClient.fetchAllPermissions()
	}

	// The names of the permissions; an id unknown to the product stays an id
	function __permissionNames(ids, catalog) {
		var names = []
		for (var i = 0; i < ids.length; i++) {
			var name = ""
			for (var g = 0; g < catalog.length && name === ""; g++) {
				var entries = catalog[g].entries || []
				for (var e = 0; e < entries.length; e++) {
					if (entries[e].permissionId === ids[i]) {
						name = entries[e].displayName || ""
						break
					}
				}
			}
			names.push(name !== "" ? name : ids[i])
		}
		return names.join(", ")
	}

	function __stateText(allocation) {
		if (allocation.isInForce)
			return qsTr("In force")
		var contract = permissionsPage.editor.contract
		if (!contract || !contract.signedAt)
			return qsTr("Once all parties sign")
		return permissionsPage.editor.contractState === "Signed" ? qsTr("Once the contract comes into force") : qsTr("Not in force")
	}

	// The receiver chooses its groups and only it sees them
	function __groupsText(allocation, isReceived) {
		if (!isReceived)
			return qsTr("Set by receiver")
		var count = permissionsPage.editor.allocationGroupIds(allocation.allocationId).length
		if (count === 0)
			return qsTr("None yet")
		return count === 1 ? qsTr("1 group") : qsTr("%1 groups").arg(count)
	}

	function __allocationRows(allocations, privateInfo, catalog) {
		var rows = []
		for (var i = 0; i < allocations.length; i++) {
			var allocation = allocations[i]
			var isReceived = allocation.granteeTenantId === permissionsPage.editor.tenantId
			var isGiven = allocation.grantorTenantId === permissionsPage.editor.tenantId
			rows.push({"id": String(i),
					   "index": i,
					   "allocationId": allocation.allocationId || "",
					   "grantor": permissionsPage.editor.partyName(allocation.grantorTenantId),
					   "grantee": permissionsPage.editor.partyName(allocation.granteeTenantId),
					   "permissions": permissionsPage.__permissionNames(allocation.permissionIds || [], catalog || []),
					   "validUntil": ContractTexts.dateText(allocation.validUntil),
					   "groups": permissionsPage.__groupsText(allocation, isReceived),
					   "state": permissionsPage.__stateText(allocation),
					   "comment": allocation.comment || "",
					   "isReceived": isReceived,
					   "isGiven": isGiven})
		}
		return rows
	}

	Item {
		id: headerItem

		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		height: Math.max(descriptionText.height, addButton.height)

		BaseText {
			id: descriptionText
			anchors.left: parent.left
			anchors.right: addButton.left
			anchors.rightMargin: Style.marginL
			anchors.verticalCenter: parent.verticalCenter
			wrapMode: Text.WordWrap
			text: (permissionsPage.allocationRows.length === 0 ? qsTr("No permissions are given yet.") + " " : "")
				+ qsTr("Each party gives its own permissions to the parties it is linked with, and may take them back at any time. The receiver gives them to its groups (button Groups) once the contract is sent; while the contract is in force, the members of these groups work in the organization of the giver. Only the receiver sees and chooses its groups.")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}

		Button {
			id: addButton
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			visible: permissionsPage.__canGive
			variant: "primary"
			text: qsTr("Give permissions")
			onClicked: {
				permissionsPage.addAllocationRequested()
			}
		}
	}

	Table {
		id: allocationsTable

		anchors.top: headerItem.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL

		visible: permissionsPage.allocationRows.length > 0
		selectable: false
		elements: permissionsPage.allocationRows
	}

	TableHeaders {
		id: allocationsHeaders

		Component.onCompleted: {
			addHeader("grantor", qsTr("Gives"), false, false)
			addHeader("grantee", qsTr("Receives"), false, false)
			addHeader("permissions", qsTr("Permissions"), false, false)
			addHeader("validUntil", qsTr("Until"), false, false)
			addHeader("groups", qsTr("Groups"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			allocationsTable.setColumnContentById("actions", allocationActionsComp)
			allocationsTable.headers = allocationsHeaders

			// The buttons need their own width only, the rest goes to the text columns
			allocationsTable.setWidth()
			let actionsIndex = allocationsTable.getHeaderIndex("actions")
			if (actionsIndex >= 0 && allocationsTable.widthDecorator.getItemsCount() > actionsIndex){
				allocationsTable.widthDecorator.setData("Width", Style.sizeHintXXS + Style.marginXL, actionsIndex)
				allocationsTable.widthDecorator.setData("WidthPercent", -1, actionsIndex)
				allocationsTable.setWidth()
			}
		}
	}

	Component {
		id: allocationActionsComp

		TableCellDelegateBase {
			id: allocationActions

			readonly property var rowData: allocationActions.getRowData()
			readonly property bool canGiveToGroups: allocationActions.rowData !== null && allocationActions.rowData !== undefined
				&& allocationActions.rowData.isReceived === true && permissionsPage.__canChooseGroups

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginS

				Button {
					id: groupsButton
					text: qsTr("Groups")
					visible: allocationActions.canGiveToGroups
					onClicked: {
						var point = groupsButton.mapToItem(null, 0, groupsButton.height)
						var allocationId = allocationActions.rowData.allocationId
						ModalDialogManager.openDialog(groupsPopupComp, {"x": point.x, "y": point.y, "allocationId": allocationId,
																		"preselectedIds": permissionsPage.editor.allocationGroupIds(allocationId)})
					}
				}

				Button {
					text: qsTr("Take back")
					visible: permissionsPage.__canGive && allocationActions.rowData !== null && allocationActions.rowData !== undefined
						&& allocationActions.rowData.isGiven === true
					onClicked: {
						var allocationId = allocationActions.rowData.allocationId
						ModalDialogManager.showConfirmationDialog(qsTr("Take back permissions"),
																  qsTr("Take back these permissions? The members of the other organization lose them at once."),
																  function(result) {
																	  if (result === Enums.yes)
																		  permissionsPage.editor.withdrawPermissions(allocationId)
																  })
					}
				}
			}
		}
	}

	Component {
		id: groupsPopupComp

		FilterableSelectPopup {
			id: groupsPopup
			dataProvider: permissionsPage.editor && permissionsPage.editor.tenantApiClient ? permissionsPage.editor.tenantApiClient.groupsMultiSelectDataProvider : null

			property string allocationId: ""
			property var newSelection: []

			itemWidth: 280
			showCheckBox: true
			// Only the ids of the chosen groups are known: they are ticked in the one list
			showSelectedGroup: false
			filterPlaceholder: qsTr("Groups receiving the permissions")

			footerComponent: Component {
				Item {
					height: saveGroupsButton.height

					property Item keyboardItem: saveGroupsButton

					Button {
						id: saveGroupsButton
						anchors.right: parent.right
						text: qsTr("Save")
						variant: "primary"
						onClicked: {
							permissionsPage.editor.giveAllocationToGroups(groupsPopup.allocationId, groupsPopup.newSelection)
							groupsPopup.closePopup()
						}
					}
				}
			}

			// The parameters of openDialog are set after the popup is created
			onPreselectedIdsChanged: {
				groupsPopup.newSelection = groupsPopup.preselectedIds.slice()
			}

			onSelectionChanged: {
				groupsPopup.newSelection = selectedIds.slice()
			}
		}
	}
}
