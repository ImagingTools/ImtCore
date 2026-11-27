// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * DirectoryProfileEditDialog
 *
 * Changes what the organization tells the others in the directory: about it, its industry, country, website
 * and the roles it offers. The visibility switches stay on the page. The dialog stays open until the server
 * has stored the profile, a refused one is shown and can be corrected.
 */
Dialog {
	id: editDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))
	title: qsTr("Profile in the directory")
	notClosingButtons: Enums.save

	property string tenantId: ""
	// ContractsApiClient
	property var contractsApiClient: null
	// The profile as saved: isListed, acceptsInvitationsFromUnknown, description, industry, country, website, offeredRoleIds
	property var profile: ({})

	property bool __isSaving: false

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.save, qsTr("Save"), true, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (editDialog.contentItem)
			editDialog.contentItem.init()
	}

	onFinished: {
		var content = editDialog.contentItem
		if (buttonId != Enums.save || !content || editDialog.__isSaving || !editDialog.contractsApiClient)
			return
		var profile = content.result()
		editDialog.__isSaving = true
		editDialog.buttons.setButtonState(Enums.save, false)
		editDialog.contractsApiClient.setDirectoryProfile(editDialog.tenantId, profile)
	}

	Connections {
		target: editDialog.contractsApiClient

		function onDirectoryProfileSaved() {
			if (!editDialog.__isSaving)
				return
			editDialog.__isSaving = false
			editDialog.finished(Enums.close)
		}

		function onCommandFailed(command, errorMessage) {
			if (command !== "SetDirectoryProfile" || !editDialog.__isSaving)
				return
			editDialog.__isSaving = false
			if (editDialog.buttons)
				editDialog.buttons.setButtonState(Enums.save, true)
		}
	}

	contentComp: Component {
		Item {
			id: editContent

			width: editDialog.width
			height: Math.min(editColumn.height + 2 * Style.marginXL, ModalDialogManager.activeView.height - 200)

			property var __offeredRoleIds: (editDialog.profile.offeredRoleIds || []).slice()
			property var __roleRows: []

			function init() {
				descriptionInput.text = editDialog.profile.description || ""
				industryInput.text = editDialog.profile.industry || ""
				countryInput.text = editDialog.profile.country || ""
				websiteInput.text = editDialog.profile.website || ""
				editContent.__fillRoles()
			}

			// An address typed without a scheme gets https://
			function __website() {
				var website = websiteInput.text.trim()
				if (website === "" || website.indexOf("http://") === 0 || website.indexOf("https://") === 0)
					return website
				return "https://" + website
			}

			// The whole profile with the visibility of the saved one
			function result() {
				return {"isListed": editDialog.profile.isListed === true,
						"acceptsInvitationsFromUnknown": editDialog.profile.acceptsInvitationsFromUnknown === true,
						"description": descriptionInput.text.trim(),
						"industry": industryInput.text.trim(),
						"country": countryInput.text.trim(),
						"website": editContent.__website(),
						"offeredRoleIds": editContent.__offeredRoleIds}
			}

			function __fillRoles() {
				var rows = []
				var roles = editDialog.contractsApiClient ? editDialog.contractsApiClient.partyRoles : []
				for (var i = 0; i < roles.length; i++) {
					if (roles[i].isPartyRole && !roles[i].isArchived)
						rows.push({"id": roles[i].roleId, "name": roles[i].name, "description": roles[i].description || "",
								   "isOffered": editContent.__offeredRoleIds.indexOf(roles[i].roleId) >= 0})
				}
				editContent.__roleRows = rows
			}

			function setRoleOffered(roleId, isOffered) {
				var ids = []
				for (var i = 0; i < editContent.__offeredRoleIds.length; i++) {
					if (editContent.__offeredRoleIds[i] !== roleId)
						ids.push(editContent.__offeredRoleIds[i])
				}
				if (isOffered)
					ids.push(roleId)
				editContent.__offeredRoleIds = ids
			}

			CustomScrollbar {
				z: editFlickable.z + 1
				anchors.right: parent.right
				anchors.top: editFlickable.top
				anchors.bottom: editFlickable.bottom
				secondSize: Style.marginM
				targetItem: editFlickable
			}

			Flickable {
				id: editFlickable

				anchors.fill: parent
				anchors.topMargin: Style.marginXL
				anchors.bottomMargin: Style.marginXL
				contentHeight: editColumn.height
				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: editColumn

					anchors.left: parent.left
					anchors.leftMargin: Style.marginXL
					anchors.right: parent.right
					anchors.rightMargin: Style.marginXL

					spacing: Style.marginM

					BaseText {
						width: parent.width
						wrapMode: Text.WordWrap
						text: qsTr("The others see this in the search and choose a partner by it. The members and the contracts of the organization stay hidden.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					GroupElementView {
						width: parent.width

						TextInputElementView {
							id: descriptionInput
							name: qsTr("About the organization")
							placeHolderText: qsTr("What the organization does, in a sentence or two")
							maximumLength: 2000
						}

						TextInputElementView {
							id: industryInput
							name: qsTr("Industry")
							placeHolderText: qsTr("e.g. Logistics")
							maximumLength: 200
						}

						TextInputElementView {
							id: countryInput
							name: qsTr("Country")
							placeHolderText: qsTr("e.g. Germany")
							maximumLength: 100
						}

						TextInputElementView {
							id: websiteInput
							name: qsTr("Website")
							placeHolderText: qsTr("e.g. example.com")
							description: qsTr("https:// is added when the address has no scheme")
							maximumLength: 500
						}
					}

					BaseText {
						width: parent.width
						wrapMode: Text.WordWrap
						text: qsTr("The roles the organization offers, e.g. Supplier: they help the others choose a role for you in a contract.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					Table {
						id: rolesTable

						width: parent.width
						height: rolesTable.headerHeight + rolesTable.contentHeight

						selectable: false
						elements: editContent.__roleRows
					}
				}
			}

			TableHeaders {
				id: rolesHeaders

				Component.onCompleted: {
					addHeader("isOffered", qsTr("Offered"), false, false)
					addHeader("name", qsTr("Role"), false, false)
					addHeader("description", qsTr("Description"), false, false)
					rolesTable.setColumnContentById("isOffered", offeredCellComp)
					rolesTable.headers = rolesHeaders
				}
			}

			Component {
				id: offeredCellComp

				TableCellDelegateBase {
					id: offeredCell

					readonly property var rowData: offeredCell.getRowData()

					CheckBox {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						checkState: offeredCell.rowData && offeredCell.rowData.isOffered ? Qt.Checked : Qt.Unchecked
						onCheckStateChanged: {
							if (offeredCell.rowData)
								editContent.setRoleOffered(offeredCell.rowData.id, checkState === Qt.Checked)
						}
					}
				}
			}
		}
	}
}
