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
 * The permissions one party of a link gives the other within the contract: the product and its permissions,
 * for a period. The permissions of this organization are chosen in its permission scope, the ones another
 * party gives are typed by their ids. The party receiving them gives them to its groups after the conclusion.
 *
 * The result is read in onFinished from contentItem: linkId, grantorTenantId, granteeTenantId, productId,
 * permissionIds, validUntil, comment.
 */
Dialog {
	id: allocationDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// The links with an id: [{linkId, tenantAId, tenantAName, tenantBId, tenantBName}]
	property var links: []
	property string tenantId: ""
	property string productId: ""
	// TenantManagementApiClient: the permissions of the product arrive after the dialog opens
	property var tenantApiClient: null
	// The permission scope of this organization: the ids of its Available permissions
	property var scopeIds: []

	Connections {
		target: allocationDialog.tenantApiClient

		function onAllPermissionsReceived() {
			if (allocationDialog.contentItem)
				allocationDialog.contentItem.__rebuildPermissions()
		}
	}

	// The permissions of the product, as PermissionsTableView.rebuildFromFlatArray takes them
	function productGroups() {
		return allocationDialog.tenantApiClient ? (allocationDialog.tenantApiClient.allPermissions || []) : []
	}

	// The permissions of the product the organization holds
	function ownGroups() {
		var allowed = {}
		for (var i = 0; i < allocationDialog.scopeIds.length; i++)
			allowed[String(allocationDialog.scopeIds[i])] = true
		var groups = allocationDialog.productGroups()
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
		addButton(Enums.ok, qsTr("Add"), false, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (allocationDialog.tenantApiClient) {
			allocationDialog.tenantApiClient.fetchAllPermissions()
		}
		if (allocationDialog.contentItem)
			allocationDialog.contentItem.init()
	}

	function __updateOk(content) {
		if (allocationDialog.buttons)
			allocationDialog.buttons.setButtonState(Enums.ok, content.linkId !== "" && content.productId !== "" && content.permissionIds.length > 0)
	}

	contentComp: Component {
		Item {
			id: allocationContent

			width: allocationDialog.width
			height: allocationColumn.height + 2 * Style.marginXL

			readonly property string linkId: linkModel.idAt(linkCB.currentIndex)
			readonly property var link: allocationContent.__linkById(allocationContent.linkId)
			readonly property bool isReversed: directionModel.idAt(directionCB.currentIndex) === "BA"
			readonly property string grantorTenantId: allocationContent.link ? (allocationContent.isReversed ? allocationContent.link.tenantBId : allocationContent.link.tenantAId) : ""
			readonly property string granteeTenantId: allocationContent.link ? (allocationContent.isReversed ? allocationContent.link.tenantAId : allocationContent.link.tenantBId) : ""
			// This organization gives what it holds; for another party the permissions of the product are offered
			readonly property bool isOwnGrant: allocationContent.grantorTenantId === allocationDialog.tenantId
			readonly property string grantorName: allocationContent.link ? (allocationContent.isReversed ? allocationContent.link.tenantBName : allocationContent.link.tenantAName) : ""
			readonly property string granteeName: allocationContent.link ? (allocationContent.isReversed ? allocationContent.link.tenantAName : allocationContent.link.tenantBName) : ""
			readonly property string productId: productInput.text.trim()
			property var checkedIds: []
			readonly property var permissionIds: allocationContent.checkedIds
			readonly property string comment: commentInput.text.trim()

			// The time of the API of the end, empty while the contract is in force
			function validUntil() {
				return ContractTexts.pickerIso(untilInput)
			}

			function __linkById(linkId) {
				for (var i = 0; i < allocationDialog.links.length; i++) {
					if (allocationDialog.links[i].linkId === linkId)
						return allocationDialog.links[i]
				}
				return null
			}

			function __rebuildPermissions() {
				allocationContent.checkedIds = []
				// Compared here: the bindings of the grantor may not be updated yet
				var isOwn = allocationContent.grantorTenantId === allocationDialog.tenantId
				permissionsView.rebuildFromFlatArray(isOwn ? allocationDialog.ownGroups() : allocationDialog.productGroups())
			}

			onPermissionIdsChanged: {
				allocationDialog.__updateOk(allocationContent)
			}

			onLinkIdChanged: {
				allocationDialog.__updateOk(allocationContent)
			}

			onProductIdChanged: {
				allocationDialog.__updateOk(allocationContent)
			}

			onLinkChanged: {
				var link = allocationContent.link
				if (!link) {
					directionModel.fill([], "id", "name", "")
					directionCB.currentIndex = -1
					return
				}
				var directions = [
					{"id": "AB", "name": qsTr("%1 gives %2").arg(link.tenantAName).arg(link.tenantBName)},
					{"id": "BA", "name": qsTr("%1 gives %2").arg(link.tenantBName).arg(link.tenantAName)}
				]
				// This organization receives by default when it is the second party of the link
				directionCB.currentIndex = directionModel.fill(directions, "id", "name", link.tenantBId === allocationDialog.tenantId ? "BA" : "AB")
			}

			onGrantorTenantIdChanged: {
				rebuildTimer.restart()
			}

			// The table is rebuilt after the bindings of the chosen grantor are updated
			Timer {
				id: rebuildTimer
				interval: 0
				repeat: false
				onTriggered: {
					allocationContent.__rebuildPermissions()
				}
			}

			function init() {
				var links = []
				for (var i = 0; i < allocationDialog.links.length; i++) {
					var link = allocationDialog.links[i]
					links.push({"linkId": link.linkId, "name": link.tenantAName + " — " + link.tenantBName})
				}
				var first = links.length > 0 ? links[0].linkId : ""
				linkCB.currentIndex = linkModel.fill(links, "linkId", "name", first)
				productInput.text = allocationDialog.productId
				allocationContent.__rebuildPermissions()
			}

			ChoiceModel {
				id: linkModel
			}

			ChoiceModel {
				id: directionModel
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
						id: linkCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose the two parties")
						name: qsTr("Parties")
						model: linkModel
					}

					ComboBoxElementView {
						id: directionCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose who gives the permissions")
						name: qsTr("Who gives")
						model: directionModel
					}

					TextInputElementView {
						id: productInput
						readOnly: true
						name: qsTr("Product")
						description: qsTr("The permissions work in this product")
					}

					DateTimePickerElementView {
						id: untilInput
						name: qsTr("Valid until")
						description: qsTr("Empty while the contract is in force")
						canShowCurrentDate: false
					}

					TextInputElementView {
						id: commentInput
						name: qsTr("Comment")
						placeHolderText: qsTr("Optional")
					}
				}

				// Who gets what: the members of the groups the receiver chooses act in the organization of the giver
				BaseText {
					width: parent.width
					visible: allocationContent.link !== null
					wrapMode: Text.WordWrap
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
					text: allocationContent.isOwnGrant
						? qsTr("Your organization gives what it holds itself (its Available permissions). Once the contract is in force, %1 gives them to its groups, and their members work in your organization with them.").arg(allocationContent.granteeName)
						: qsTr("%1 gives these permissions to %2. Only the ones %1 holds itself (its Available permissions) work. Once the contract is in force, %2 gives them to its groups, and their members work in %1 with them.").arg(allocationContent.grantorName).arg(allocationContent.granteeName)
				}

				PermissionsTableView {
					id: permissionsView
					width: parent.width
					// The dialog stays inside the window; the table scrolls
					height: Math.max(Style.sizeHintXXS / 2, Math.min(Style.sizeHintS, ModalDialogManager.activeView.height - 760))
					showDetails: false
					emptyText: qsTr("The organization holds no permissions to give. They are chosen on the page Available permissions.")

					onSelectionChanged: {
						allocationContent.checkedIds = permissionsView.getCheckedIds(false)
					}
				}
			}
		}
	}
}
