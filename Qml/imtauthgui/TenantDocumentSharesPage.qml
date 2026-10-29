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

				let row = {}
				row.id = String(share.m_id)
				row.objectId = String(share.m_objectId)
				row.objectName = share.m_objectName ? String(share.m_objectName) : String(share.m_objectId)
				row.collectionId = share.m_collectionId ? String(share.m_collectionId) : ""
				row.ownerTenantName = share.m_ownerTenantName ? String(share.m_ownerTenantName) : ""
				row.targetTenantName = share.m_targetTenantName ? String(share.m_targetTenantName) : ""
				row.accessLevel = String(share.m_accessLevel)
				row.status = String(share.m_status)
				row.validUntil = share.m_validUntil ? String(share.m_validUntil).substring(0, 10) : ""
				row.groupIds = groupIds
				row.access = row.accessLevel === "Edit" ? qsTr("Edit") : qsTr("View")
				row.state = row.validUntil !== ""
					? qsTr("%1, until %2").arg(sharesPage.stateText(row.status)).arg(row.validUntil)
					: sharesPage.stateText(row.status)
				rows.push(row)
			}
		}

		return rows
	}

	function stateText(status){
		if (status === "Declined")
			return qsTr("Declined")
		if (status === "Pending")
			return qsTr("Pending")
		return qsTr("Active")
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

	// The document gets the widest column, the menu button a fixed one, the other columns share the rest
	function setColumnWidths(table, headers){
		table.setWidth()
		let documentIndex = headers.getHeaderIndex("objectName")
		let actionsIndex = headers.getHeaderIndex("actions")
		if (documentIndex < 0 || actionsIndex < 0 || table.widthDecorator.getItemsCount() <= Math.max(documentIndex, actionsIndex)){
			return
		}

		table.widthDecorator.setData("WidthPercent", 40, documentIndex)
		table.widthDecorator.setData("Width", Style.controlHeightM + Style.marginM, actionsIndex)
		table.widthDecorator.setData("WidthPercent", -1, actionsIndex)
		table.setWidth()
	}

	property var menuRow: null

	function showShareActions(row, incoming, menuX, menuY){
		sharesPage.menuRow = row
		shareActionsModel.clear()
		if (!incoming){
			shareActionsModel.append({"id": "Revoke", "name": qsTr("Revoke"), "icon": "", "isEnabled": true})
		}
		else if (row.status === "Active"){
			let groupsText = row.groupIds.length > 0 ? qsTr("Groups (%1)...").arg(row.groupIds.length) : qsTr("Groups...")
			shareActionsModel.append({"id": "Groups", "name": groupsText, "icon": "", "isEnabled": true})
			shareActionsModel.append({"id": "Decline", "name": qsTr("Decline"), "icon": "", "isEnabled": true})
		}
		else {
			shareActionsModel.append({"id": "Accept", "name": qsTr("Accept"), "icon": "", "isEnabled": true})
		}

		ModalDialogManager.openDialog(shareActionsPopupComp, {"x": menuX, "y": menuY, "model": shareActionsModel})
	}

	function runShareAction(commandId){
		let row = sharesPage.menuRow
		if (!row){
			return
		}

		if (commandId === "Groups"){
			ModalDialogManager.openDialog(groupsDialogComp, {"shareId": row.id, "groupIds": row.groupIds})
		}
		else if (commandId === "Decline"){
			sharesPage.setStatus(row.id, "Declined")
		}
		else if (commandId === "Accept"){
			sharesPage.setStatus(row.id, "Active")
		}
		else if (commandId === "Revoke"){
			sharesPage.removeShare(row.id)
		}
	}

	ListModel {
		id: shareActionsModel
	}

	Component {
		id: shareActionsPopupComp

		PopupMenuDialog {
			onFinished: {
				sharesPage.runShareAction(commandId)
			}
		}
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

			BaseText {
				visible: sharesPage.incomingShares.length === 0
				text: qsTr("No documents are shared with this organization")
				color: Style.inactiveTextColor
			}

			Table {
				id: incomingTable
				visible: sharesPage.incomingShares.length > 0
				width: parent.width
				height: incomingTable.contentHeight + incomingTable.headerHeight
				selectable: false
				elements: sharesPage.incomingShares
			}

			Item {
				width: parent.width
				height: Style.marginS
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Shared by us")
			}

			BaseText {
				visible: sharesPage.outgoingShares.length === 0
				text: qsTr("This organization shares no documents")
				color: Style.inactiveTextColor
			}

			Table {
				id: outgoingTable
				visible: sharesPage.outgoingShares.length > 0
				width: parent.width
				height: outgoingTable.contentHeight + outgoingTable.headerHeight
				selectable: false
				elements: sharesPage.outgoingShares
			}
		}
	}

	TableHeaders {
		id: incomingHeaders

		Component.onCompleted: {
			addHeader("objectName", qsTr("Document"), false, false)
			addHeader("ownerTenantName", qsTr("From"), false, false)
			addHeader("access", qsTr("Access"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			incomingTable.headers = incomingHeaders
			incomingTable.setColumnContentById("objectName", incomingDocumentCellComp)
			incomingTable.setColumnContentById("actions", incomingActionsCellComp)
			sharesPage.setColumnWidths(incomingTable, incomingHeaders)
		}
	}

	TableHeaders {
		id: outgoingHeaders

		Component.onCompleted: {
			addHeader("objectName", qsTr("Document"), false, false)
			addHeader("targetTenantName", qsTr("To"), false, false)
			addHeader("access", qsTr("Access"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			outgoingTable.headers = outgoingHeaders
			outgoingTable.setColumnContentById("objectName", outgoingDocumentCellComp)
			outgoingTable.setColumnContentById("actions", outgoingActionsCellComp)
			sharesPage.setColumnWidths(outgoingTable, outgoingHeaders)
		}
	}

	Component {
		id: incomingDocumentCellComp

		TableCellDelegateBase {
			id: incomingDocumentCell
			readonly property var rowData: incomingDocumentCell.getRowData()
			readonly property bool canOpen: incomingDocumentCell.rowData ? incomingDocumentCell.rowData.status === "Active" : false

			BaseText {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				width: Math.min(implicitWidth, parent.width - 2 * Style.marginM)
				text: incomingDocumentCell.rowData ? incomingDocumentCell.rowData.objectName : ""
				font.underline: incomingLinkArea.containsMouse && incomingDocumentCell.canOpen
				color: incomingDocumentCell.canOpen ? Style.linkColor : Style.textColor
				elide: Text.ElideRight

				MouseArea {
					id: incomingLinkArea
					anchors.fill: parent
					hoverEnabled: true
					enabled: incomingDocumentCell.canOpen
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						sharesPage.openDocument(incomingDocumentCell.rowData, incomingDocumentCell.rowData.accessLevel !== "Edit")
					}
				}
			}
		}
	}

	Component {
		id: incomingActionsCellComp

		TableCellDelegateBase {
			id: incomingActionsCell
			readonly property var rowData: incomingActionsCell.getRowData()

			ToolButton {
				id: incomingActionsButton
				objectName: "ShareActionsButton"
				anchors.centerIn: parent
				visible: incomingActionsCell.rowData !== null && incomingActionsCell.rowData !== undefined
				tooltipText: qsTr("Actions")
				iconSource: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent"
						icon.width: Style.iconSizeXS
					}
				}

				onClicked: {
					// The menu ends at the button, so it stays inside the window at the right edge
					let point = incomingActionsButton.mapToItem(null, incomingActionsButton.width - Style.sizeHintXXS, incomingActionsButton.height)
					sharesPage.showShareActions(incomingActionsCell.rowData, true, point.x, point.y)
				}
			}
		}
	}

	Component {
		id: outgoingDocumentCellComp

		TableCellDelegateBase {
			id: outgoingDocumentCell
			readonly property var rowData: outgoingDocumentCell.getRowData()

			BaseText {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				width: Math.min(implicitWidth, parent.width - 2 * Style.marginM)
				text: outgoingDocumentCell.rowData ? outgoingDocumentCell.rowData.objectName : ""
				font.underline: outgoingLinkArea.containsMouse
				color: Style.linkColor
				elide: Text.ElideRight

				MouseArea {
					id: outgoingLinkArea
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						sharesPage.openDocument(outgoingDocumentCell.rowData, false)
					}
				}
			}
		}
	}

	Component {
		id: outgoingActionsCellComp

		TableCellDelegateBase {
			id: outgoingActionsCell
			readonly property var rowData: outgoingActionsCell.getRowData()

			ToolButton {
				id: outgoingActionsButton
				objectName: "ShareActionsButton"
				anchors.centerIn: parent
				visible: outgoingActionsCell.rowData !== null && outgoingActionsCell.rowData !== undefined
				tooltipText: qsTr("Actions")
				iconSource: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent"
						icon.width: Style.iconSizeXS
					}
				}

				onClicked: {
					let point = outgoingActionsButton.mapToItem(null, outgoingActionsButton.width - Style.sizeHintXXS, outgoingActionsButton.height)
					sharesPage.showShareActions(outgoingActionsCell.rowData, false, point.x, point.y)
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
