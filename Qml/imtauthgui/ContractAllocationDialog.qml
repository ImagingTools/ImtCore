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
	// The permission scope of this organization, as PermissionsTableView.rebuildFromFlatArray takes it
	property var permissionGroups: []

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("Add"), false, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
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
			// This organization gives what it holds: its scope is shown; another party is asked for its permissions by their ids
			readonly property bool choosesPermissions: allocationContent.grantorTenantId === allocationDialog.tenantId
			readonly property string productId: productInput.text.trim()
			property var checkedIds: []
			readonly property var permissionIds: allocationContent.choosesPermissions ? allocationContent.checkedIds : allocationContent.__typedIds(idsInput.text)
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

			function __typedIds(text) {
				var result = []
				var parts = text.split(",")
				for (var i = 0; i < parts.length; i++) {
					var id = parts[i].trim()
					if (id !== "" && result.indexOf(id) < 0)
						result.push(id)
				}
				return result
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

			onChoosesPermissionsChanged: {
				if (allocationContent.choosesPermissions)
					permissionsView.rebuildFromFlatArray(allocationDialog.permissionGroups)
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
				if (allocationContent.choosesPermissions)
					permissionsView.rebuildFromFlatArray(allocationDialog.permissionGroups)
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
						isRequired: true
						name: qsTr("Product")
						placeHolderText: qsTr("Id of the product")
					}

					TextInputElementView {
						id: idsInput
						isRequired: true
						visible: !allocationContent.choosesPermissions
						name: qsTr("Permissions")
						placeHolderText: qsTr("Ids of the permissions, separated by commas")
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

				PermissionsTableView {
					id: permissionsView
					width: parent.width
					height: Style.sizeHintS
					visible: allocationContent.choosesPermissions
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
