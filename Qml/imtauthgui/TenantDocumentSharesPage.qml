// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseDocumentSharesSdl 1.0

/**
 * TenantDocumentSharesPage
 *
 * Shared Documents tab of the TenantEditor: the documents other organizations share with this one,
 * and the documents this organization shares with others. Works on the organization of the current session.
 */
Item {
	id: sharesPage

	property var incomingShares: []
	property var outgoingShares: []
	property string errorText: ""

	Component.onCompleted: {
		sharesPage.updateModel()
	}

	function updateModel(){
		getIncomingInput.m_direction = "Incoming"
		getIncomingSender.send(getIncomingInput)

		getOutgoingInput.m_direction = "Outgoing"
		getOutgoingSender.send(getOutgoingInput)
	}

	function createShareRows(shares){
		let rows = []
		if (shares){
			for (let i = 0; i < shares.count; i++){
				let share = shares.get(i).item
				if (!share){
					continue
				}

				let groupIds = []
				if (share.m_targetGroupIds){
					for (let j = 0; j < share.m_targetGroupIds.length; j++){
						groupIds.push(String(share.m_targetGroupIds[j]))
					}
				}

				rows.push({
					"id": String(share.m_id),
					"objectId": String(share.m_objectId),
					"objectName": share.m_objectName ? String(share.m_objectName) : String(share.m_objectId),
					"collectionId": share.m_collectionId ? String(share.m_collectionId) : "",
					"ownerTenantName": share.m_ownerTenantName ? String(share.m_ownerTenantName) : "",
					"targetTenantName": share.m_targetTenantName ? String(share.m_targetTenantName) : "",
					"accessLevel": String(share.m_accessLevel),
					"status": String(share.m_status),
					"validUntil": share.m_validUntil ? String(share.m_validUntil) : "",
					"groupIds": groupIds
				})
			}
		}

		return rows
	}

	function accessText(row){
		let accessLevel = row.accessLevel === "Edit" ? qsTr("Edit") : qsTr("View")
		let status = row.status === "Declined" ? qsTr("declined") : row.status === "Pending" ? qsTr("pending") : ""
		let validUntil = row.validUntil !== "" ? qsTr("until %1").arg(row.validUntil.substring(0, 10)) : ""

		let parts = [row.collectionId, accessLevel]
		if (status !== ""){
			parts.push(status)
		}

		if (validUntil !== ""){
			parts.push(validUntil)
		}

		if (row.groupIds.length > 0){
			parts.push(qsTr("%1 groups").arg(row.groupIds.length))
		}

		return parts.join(" · ")
	}

	function setStatus(shareId, status){
		setStatusInput.m_shareIds = [shareId]
		setStatusInput.m_status = status
		setStatusSender.send(setStatusInput)
	}

	function removeShare(shareId){
		removeSharesInput.m_shareIds = [shareId]
		removeSharesSender.send(removeSharesInput)
	}

	// The page of a collection has the ID of the collection; the type of the document completes the path
	function openDocument(row, readOnly){
		sharesPage.pendingOpenRow = row
		sharesPage.pendingOpenReadOnly = readOnly
		getObjectTypeIdInput.m_collectionId = row.collectionId
		getObjectTypeIdInput.m_objectId = row.objectId
		getObjectTypeIdSender.send(getObjectTypeIdInput)
	}

	property var pendingOpenRow: null
	property bool pendingOpenReadOnly: false

	function setGroups(shareId, groupIds){
		setGroupsInput.m_shareId = shareId
		setGroupsInput.m_groupIds = groupIds
		setGroupsSender.send(setGroupsInput)
	}

	CustomScrollbar {
		id: scrollbar

		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable

		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: bodyColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn

			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(flickable.width - 2 * Style.marginXL, Style.contentWidthMax)
			spacing: Style.marginM

			BaseText {
				width: parent.width
				visible: sharesPage.errorText !== ""
				text: sharesPage.errorText
				color: Style.errorTextColor
				wrapMode: Text.WordWrap
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Received")
			}

			BaseText {
				width: parent.width
				text: qsTr("Documents shared with this organization are visible to its members within their own permissions. Restrict a received document to some groups to hide it from the others.")
				color: Style.inactiveTextColor
				font.pixelSize: Style.fontSizeS
				wrapMode: Text.WordWrap
			}

			Rectangle {
				width: parent.width
				height: sharesPage.incomingShares.length === 0 ? Style.controlHeightL + 2 * Style.marginM : incomingColumn.height + 2

				radius: Style.radiusM
				color: Style.baseColor
				border.width: 1
				border.color: Style.borderColor

				BaseText {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter

					visible: sharesPage.incomingShares.length === 0
					text: qsTr("No documents are shared with this organization")
					color: Style.inactiveTextColor
				}

				Column {
					id: incomingColumn

					anchors.top: parent.top
					anchors.topMargin: 1
					anchors.left: parent.left
					anchors.right: parent.right

					Repeater {
						model: sharesPage.incomingShares

						delegate: Item {
							width: incomingColumn.width
							height: Math.max(incomingTextColumn.height, incomingButtons.height) + 2 * Style.marginM

							Column {
								id: incomingTextColumn

								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.right: incomingButtons.left
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXXS

								BaseText {
									width: Math.min(implicitWidth, parent.width)
									text: modelData.objectName
									font.bold: true
									font.underline: incomingLinkArea.containsMouse && incomingLinkArea.enabled
									color: incomingLinkArea.enabled ? Style.linkColor : Style.textColor
									elide: Text.ElideRight

									MouseArea {
										id: incomingLinkArea

										anchors.fill: parent
										hoverEnabled: true
										enabled: modelData.status === "Active"
										cursorShape: Qt.PointingHandCursor

										onClicked: {
											sharesPage.openDocument(modelData, modelData.accessLevel !== "Edit")
										}
									}
								}

								BaseText {
									width: parent.width
									text: qsTr("From %1").arg(modelData.ownerTenantName) + " · " + sharesPage.accessText(modelData)
									color: Style.subtitleColor
									font.pixelSize: Style.fontSizeS
									elide: Text.ElideRight
								}
							}

							Row {
								id: incomingButtons

								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginS

								Button {
									text: qsTr("Groups")
									visible: modelData.status === "Active"
									onClicked: {
										ModalDialogManager.openDialog(groupsDialogComp, {"shareId": modelData.id, "groupIds": modelData.groupIds})
									}
								}

								Button {
									text: modelData.status === "Active" ? qsTr("Decline") : qsTr("Accept")
									onClicked: {
										sharesPage.setStatus(modelData.id, modelData.status === "Active" ? "Declined" : "Active")
									}
								}
							}

							Rectangle {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.bottom: parent.bottom

								height: 1
								color: Style.borderColor
								visible: index < sharesPage.incomingShares.length - 1
							}
						}
					}
				}
			}

			Item {
				width: parent.width
				height: Style.marginS
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Shared by us")
			}

			Rectangle {
				width: parent.width
				height: sharesPage.outgoingShares.length === 0 ? Style.controlHeightL + 2 * Style.marginM : outgoingColumn.height + 2

				radius: Style.radiusM
				color: Style.baseColor
				border.width: 1
				border.color: Style.borderColor

				BaseText {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter

					visible: sharesPage.outgoingShares.length === 0
					text: qsTr("This organization shares no documents")
					color: Style.inactiveTextColor
				}

				Column {
					id: outgoingColumn

					anchors.top: parent.top
					anchors.topMargin: 1
					anchors.left: parent.left
					anchors.right: parent.right

					Repeater {
						model: sharesPage.outgoingShares

						delegate: Item {
							width: outgoingColumn.width
							height: Math.max(outgoingTextColumn.height, revokeButton.height) + 2 * Style.marginM

							Column {
								id: outgoingTextColumn

								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.right: revokeButton.left
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXXS

								BaseText {
									width: Math.min(implicitWidth, parent.width)
									text: modelData.objectName
									font.bold: true
									font.underline: outgoingLinkArea.containsMouse
									color: Style.linkColor
									elide: Text.ElideRight

									MouseArea {
										id: outgoingLinkArea

										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor

										onClicked: {
											sharesPage.openDocument(modelData, false)
										}
									}
								}

								BaseText {
									width: parent.width
									text: qsTr("To %1").arg(modelData.targetTenantName) + " · " + sharesPage.accessText(modelData)
									color: Style.subtitleColor
									font.pixelSize: Style.fontSizeS
									elide: Text.ElideRight
								}
							}

							Button {
								id: revokeButton

								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter

								text: qsTr("Revoke")
								onClicked: {
									sharesPage.removeShare(modelData.id)
								}
							}

							Rectangle {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.bottom: parent.bottom

								height: 1
								color: Style.borderColor
								visible: index < sharesPage.outgoingShares.length - 1
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: groupsDialogComp

		Dialog {
			id: groupsDialog

			width: Style.sizeHintM
			title: qsTr("Groups seeing the document")

			property string shareId: ""
			property var groupIds: []
			property var selectedGroupIds: []

			Component.onCompleted: {
				addButton(Enums.ok, qsTr("Save"), true)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			onStarted: {
				groupsDialog.selectedGroupIds = groupsDialog.groupIds
			}

			onFinished: {
				if (buttonId == Enums.ok){
					sharesPage.setGroups(groupsDialog.shareId, groupsDialog.selectedGroupIds)
				}
			}

			contentComp: Component {
				Item {
					width: groupsDialog.width
					height: groupsColumn.height + 2 * Style.marginXL

					Column {
						id: groupsColumn

						anchors.top: parent.top
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginM

						BaseText {
							width: parent.width
							text: qsTr("Without groups every member of the organization sees the document.")
							color: Style.inactiveTextColor
							wrapMode: Text.WordWrap
						}

						GqlBasedItemSelectElementView {
							id: groupsSelectView

							width: parent.width
							collectionId: "Groups"
							label: qsTr("Groups")
							addButtonText: qsTr("Add Group")

							Component.onCompleted: {
								let items = []
								for (let i = 0; i < groupsDialog.selectedGroupIds.length; i++){
									items.push({"id": groupsDialog.selectedGroupIds[i], "name": ""})
								}

								groupsSelectView.items = items
							}

							onSelectionChanged: {
								let ids = []
								for (let i = 0; i < groupsSelectView.items.length; i++){
									ids.push(groupsSelectView.items[i].id)
								}

								groupsDialog.selectedGroupIds = ids
							}
						}
					}
				}
			}
		}
	}

	GetOrganizationDocumentSharesInput {
		id: getIncomingInput
	}

	GetObjectTypeIdInput {
		id: getObjectTypeIdInput
	}

	GqlSdlRequestSender {
		id: getObjectTypeIdSender

		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectTypeId

		sdlObjectComp: Component {
			GetObjectTypeIdPayload {
				onFinished: {
					let row = sharesPage.pendingOpenRow
					if (!row || !m_typeId || String(m_typeId) === ""){
						return
					}

					NavigationController.navigate(row.collectionId + "/" + String(m_typeId) + "/" + row.objectId, {"readOnly": sharesPage.pendingOpenReadOnly})
				}
			}
		}

		function onError(message, type){
			sharesPage.errorText = message
		}
	}

	GqlSdlRequestSender {
		id: getIncomingSender

		gqlCommandId: ImtbaseDocumentSharesSdlCommandIds.s_getOrganizationDocumentShares

		sdlObjectComp: Component {
			GetOrganizationDocumentSharesPayload {
				onFinished: {
					sharesPage.incomingShares = sharesPage.createShareRows(m_shares)
				}
			}
		}

		function onError(message, type){
			sharesPage.errorText = message
		}
	}

	GetOrganizationDocumentSharesInput {
		id: getOutgoingInput
	}

	GqlSdlRequestSender {
		id: getOutgoingSender

		gqlCommandId: ImtbaseDocumentSharesSdlCommandIds.s_getOrganizationDocumentShares

		sdlObjectComp: Component {
			GetOrganizationDocumentSharesPayload {
				onFinished: {
					sharesPage.outgoingShares = sharesPage.createShareRows(m_shares)
				}
			}
		}
	}

	SetDocumentSharesStatusInput {
		id: setStatusInput
	}

	GqlSdlRequestSender {
		id: setStatusSender

		gqlCommandId: ImtbaseDocumentSharesSdlCommandIds.s_setDocumentSharesStatus
		requestType: 1

		sdlObjectComp: Component {
			SetDocumentSharesStatusPayload {
				onFinished: {
					sharesPage.updateModel()
				}
			}
		}
	}

	SetDocumentShareGroupsInput {
		id: setGroupsInput
	}

	GqlSdlRequestSender {
		id: setGroupsSender

		gqlCommandId: ImtbaseDocumentSharesSdlCommandIds.s_setDocumentShareGroups
		requestType: 1

		sdlObjectComp: Component {
			SetDocumentShareGroupsPayload {
				onFinished: {
					sharesPage.updateModel()
				}
			}
		}
	}

	RemoveOrganizationDocumentSharesInput {
		id: removeSharesInput
	}

	GqlSdlRequestSender {
		id: removeSharesSender

		gqlCommandId: ImtbaseDocumentSharesSdlCommandIds.s_removeOrganizationDocumentShares
		requestType: 1

		sdlObjectComp: Component {
			RemoveOrganizationDocumentSharesPayload {
				onFinished: {
					sharesPage.updateModel()
				}
			}
		}
	}
}
