// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtdeskImtDeskSdl 1.0
import imtdeskTicketCollectionDocumentManagerSdl 1.0
import imtauthUsersSdl 1.0

ViewBase {
	id: root

	anchors.fill: parent

	property TicketData ticketData: model
	property bool isNewIssue: ticketData ? (ticketData.m_number === 0) : true

	function updateGui(){
		titleInput.text = ticketData.m_title
		descriptionInput.text = ticketData.m_description || ""

		assigneeCB.currentIndex = -1
		if (assigneeCB.model){
			let assigneeIds = ticketData.m_assigneeIds || []
			let firstAssignee = assigneeIds.length > 0 ? assigneeIds[0] : ""
			for (let i = 0; i < assigneeCB.model.getItemsCount(); i++){
				let userId = assigneeCB.model.getData("id", i)
				if (firstAssignee === userId){
					assigneeCB.currentIndex = i
					break
				}
			}
		}

		reporterCB.currentIndex = -1
		if (reporterCB.model){
			for (let i = 0; i < reporterCB.model.getItemsCount(); i++){
				let userId = reporterCB.model.getData("id", i)
				if (ticketData.m_reporterId === userId){
					reporterCB.currentIndex = i
					break
				}
			}
		}

		conversationIdInput.text = ticketData.m_conversationId || ""
		milestoneIdInput.text = ticketData.m_milestoneId || ""
		lockReasonInput.text = ticketData.m_lockReason || ""
		lockedCB.checkState = (ticketData.m_locked || false) ? Qt.Checked : Qt.Unchecked

		ticketTypeCB.currentIndex = -1
		if (ticketTypeCB.model){
			for (let i = 0; i < ticketTypeCB.model.getItemsCount(); i++){
				let typeId = ticketTypeCB.model.getData("id", i)
				if (ticketData.m_ticketType === typeId){
					ticketTypeCB.currentIndex = i
					break
				}
			}
		}
		if (ticketTypeCB.currentIndex < 0){
			ticketTypeCB.currentIndex = 1
		}

		statusCB.currentIndex = -1
		if (statusCB.model){
			for (let i = 0; i < statusCB.model.getItemsCount(); i++){
				let statusId = statusCB.model.getData("id", i)
				if (ticketData.m_status === statusId){
					statusCB.currentIndex = i
					break
				}
			}
		}
		if (statusCB.currentIndex < 0){
			statusCB.currentIndex = 0
		}

		stateReasonCB.currentIndex = -1
		if (stateReasonCB.model){
			for (let i = 0; i < stateReasonCB.model.getItemsCount(); i++){
				let reasonId = stateReasonCB.model.getData("id", i)
				if (ticketData.m_stateReason === reasonId){
					stateReasonCB.currentIndex = i
					break
				}
			}
		}
		if (stateReasonCB.currentIndex < 0){
			stateReasonCB.currentIndex = 0
		}

		priorityCB.currentIndex = -1
		if (priorityCB.model){
			for (let i = 0; i < priorityCB.model.getItemsCount(); i++){
				let priorityId = priorityCB.model.getData("id", i)
				if (ticketData.m_priority === priorityId){
					priorityCB.currentIndex = i
					break
				}
			}
		}
		if (priorityCB.currentIndex < 0){
			priorityCB.currentIndex = 1
		}

		environmentCB.currentIndex = -1
		if (environmentCB.model){
			for (let i = 0; i < environmentCB.model.getItemsCount(); i++){
				let envId = environmentCB.model.getData("id", i)
				if (ticketData.m_environment === envId){
					environmentCB.currentIndex = i
					break
				}
			}
		}
		if (environmentCB.currentIndex < 0){
			environmentCB.currentIndex = 2
		}
	}

	function updateModel(){
		ticketData.m_title = titleInput.text
		ticketData.m_description = descriptionInput.text

		if (assigneeCB.model && assigneeCB.currentIndex >= 0){
			ticketData.m_assigneeIds = [assigneeCB.model.getData("id", assigneeCB.currentIndex)]
		}
		else{
			ticketData.m_assigneeIds = []
		}

		if (reporterCB.model && reporterCB.currentIndex >= 0){
			ticketData.m_reporterId = reporterCB.model.getData("id", reporterCB.currentIndex)
		}
		else{
			ticketData.m_reporterId = ""
		}

		ticketData.m_conversationId = conversationIdInput.text
		ticketData.m_milestoneId = milestoneIdInput.text
		ticketData.m_locked = lockedCB.checkState === Qt.Checked
		ticketData.m_lockReason = lockReasonInput.text

		if (ticketTypeCB.model && ticketTypeCB.currentIndex >= 0){
			ticketData.m_ticketType = ticketTypeCB.model.getData("id", ticketTypeCB.currentIndex)
		}

		if (statusCB.model && statusCB.currentIndex >= 0){
			ticketData.m_status = statusCB.model.getData("id", statusCB.currentIndex)
		}

		if (stateReasonCB.model && stateReasonCB.currentIndex >= 0){
			ticketData.m_stateReason = stateReasonCB.model.getData("id", stateReasonCB.currentIndex)
		}

		if (priorityCB.model && priorityCB.currentIndex >= 0){
			ticketData.m_priority = priorityCB.model.getData("id", priorityCB.currentIndex)
		}

		if (environmentCB.model && environmentCB.currentIndex >= 0){
			ticketData.m_environment = environmentCB.model.getData("id", environmentCB.currentIndex)
		}
	}

	CollectionDataProvider {
		id: userCollectionProvider
		commandId: ImtauthUsersSdlCommandIds.s_usersList
		fields: [UserDataInputTypeMetaInfo.s_id, UserDataInputTypeMetaInfo.s_typeId, UserDataInputTypeMetaInfo.s_name]
		onCollectionModelChanged: {
			assigneeCB.model = collectionModel
			reporterCB.model = collectionModel
			root.doUpdateGui()
		}
		Component.onCompleted: {
			updateModel()
		}
	}

	// Status model
	TreeItemModel {
		id: statusModel
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "Open", index)
			setData("name", "Open", index)

			index = insertNewItem()
			setData("id", "Closed", index)
			setData("name", "Closed", index)
		}
	}

	// State reason model
	TreeItemModel {
		id: stateReasonModel
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "None", index)
			setData("name", "None", index)

			index = insertNewItem()
			setData("id", "Completed", index)
			setData("name", "Completed", index)

			index = insertNewItem()
			setData("id", "NotPlanned", index)
			setData("name", "Not Planned", index)

			index = insertNewItem()
			setData("id", "Reopened", index)
			setData("name", "Reopened", index)
		}
	}

	// Ticket type model
	TreeItemModel {
		id: ticketTypeModel
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "AccessRequest", index)
			setData("name", "Access Request", index)

			index = insertNewItem()
			setData("id", "SupportRequest", index)
			setData("name", "Support Request", index)

			index = insertNewItem()
			setData("id", "FeatureRequest", index)
			setData("name", "Feature Request", index)

			index = insertNewItem()
			setData("id", "BugReport", index)
			setData("name", "Bug Report", index)
		}
	}

	// Priority model
	TreeItemModel {
		id: priorityModel
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "Low", index)
			setData("name", "Low", index)

			index = insertNewItem()
			setData("id", "Medium", index)
			setData("name", "Medium", index)

			index = insertNewItem()
			setData("id", "High", index)
			setData("name", "High", index)

			index = insertNewItem()
			setData("id", "Critical", index)
			setData("name", "Critical", index)
		}
	}

	// Environment model
	TreeItemModel {
		id: environmentModel
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "Development", index)
			setData("name", "Development", index)

			index = insertNewItem()
			setData("id", "Staging", index)
			setData("name", "Staging", index)

			index = insertNewItem()
			setData("id", "Production", index)
			setData("name", "Production", index)
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
		visible: root.visible
	}

	CustomScrollbar {
		id: scrollHoriz
		z: parent.z + 1
		anchors.left: flickable.left
		anchors.right: flickable.right
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		vertical: false
		targetItem: flickable
	}

	Flickable {
		id: flickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: scrollbar.left
		anchors.rightMargin: Style.marginXL
		contentWidth: mainRow.width
		contentHeight: mainRow.height + Style.marginXL * 2
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Row {
			id: mainRow
			spacing: Style.marginXL

			// Left column: main content (title + description + action buttons)
			Column {
				id: mainContentColumn
				width: 560
				spacing: Style.marginM

				// Header: "New Issue" or "Edit Issue #N"
				Text {
					text: root.isNewIssue ? qsTr("New Issue") : qsTr("Edit Issue #%1").arg(ticketData ? ticketData.m_number : 0)
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				// Status badges (existing issues only)
				Row {
					visible: !root.isNewIssue
					spacing: Style.paddingXS

					TicketBadge {
						badgeType: "status"
						value: statusCB.currentIndex
					}

					TicketBadge {
						badgeType: "stateReason"
						value: stateReasonCB.currentIndex
						visible: stateReasonCB.currentIndex > 0
					}
				}

				// Title input
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Add a title")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					CustomTextField {
						id: titleInput
						width: parent.width
						height: 40
						placeHolderText: qsTr("Title")
						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Description input
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Add a description")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					CustomTextField {
						id: descriptionInput
						width: parent.width
						height: 120
						placeHolderText: qsTr("Leave a comment")
						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Action buttons row
				Row {
					spacing: Style.paddingS

					// Submit new issue (new issues only)
					Rectangle {
						visible: root.isNewIssue
						width: submitText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "#1a7f37"

						Text {
							id: submitText
							anchors.centerIn: parent
							text: qsTr("Submit new issue")
							font.pixelSize: Style.fontSizeS
							color: "white"
							font.bold: true
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
								root.doUpdateModel()
							}
						}
					}

					// Close as completed (existing open issues)
					Rectangle {
						visible: !root.isNewIssue && statusCB.currentIndex === 0
						width: closeCompletedText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "#8957e5"

						Text {
							id: closeCompletedText
							anchors.centerIn: parent
							text: qsTr("Close as completed")
							font.pixelSize: Style.fontSizeS
							color: "white"
							font.bold: true
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
								statusCB.currentIndex = 1
								stateReasonCB.currentIndex = 1
								root.doUpdateModel()
							}
						}
					}

					// Close as not planned (existing open issues)
					Rectangle {
						visible: !root.isNewIssue && statusCB.currentIndex === 0
						width: closeNotPlannedText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "transparent"
						border.color: Style.borderColor
						border.width: 1

						Text {
							id: closeNotPlannedText
							anchors.centerIn: parent
							text: qsTr("Close as not planned")
							font.pixelSize: Style.fontSizeS
							color: Style.textColor
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
								statusCB.currentIndex = 1
								stateReasonCB.currentIndex = 2
								root.doUpdateModel()
							}
						}
					}

					// Reopen (existing closed issues)
					Rectangle {
						visible: !root.isNewIssue && statusCB.currentIndex === 1
						width: reopenText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "#1a7f37"

						Text {
							id: reopenText
							anchors.centerIn: parent
							text: qsTr("Reopen issue")
							font.pixelSize: Style.fontSizeS
							color: "white"
							font.bold: true
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
								statusCB.currentIndex = 0
								stateReasonCB.currentIndex = 3
								root.doUpdateModel()
							}
						}
					}
				}
			}

			// Separator line
			Rectangle {
				width: 1
				height: mainContentColumn.height
				color: Style.borderColor
			}

			// Right column: sidebar metadata
			Column {
				id: sidebarColumn
				width: 260
				spacing: 0

				// Assignees section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Assignees")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: assigneeCB
						width: parent.width
						height: Style.buttonHeightM
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Type section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Type")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: ticketTypeCB
						width: parent.width
						height: Style.buttonHeightM
						currentIndex: 1
						model: ticketTypeModel
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Priority section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Priority")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: priorityCB
						width: parent.width
						height: Style.buttonHeightM
						currentIndex: 1
						model: priorityModel
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Milestone section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Milestone")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					CustomTextField {
						id: milestoneIdInput
						width: parent.width
						height: 40
						placeHolderText: qsTr("No milestone")
						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Environment section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Environment")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: environmentCB
						width: parent.width
						height: Style.buttonHeightM
						currentIndex: 2
						model: environmentModel
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Reporter section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Reporter")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: reporterCB
						width: parent.width
						height: Style.buttonHeightM
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Linked Conversation section
				Column {
					width: parent.width
					spacing: Style.paddingXS

					Text {
						text: qsTr("Linked Conversation")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					CustomTextField {
						id: conversationIdInput
						width: parent.width
						height: 40
						placeHolderText: qsTr("No conversation")
						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}

				// Lock section
				Row {
					width: parent.width
					spacing: Style.paddingS

					CheckBox {
						id: lockedCB
						text: qsTr("Lock issue")
						onCheckStateChanged: {
							root.doUpdateModel()
						}
					}
				}

				// Lock reason (visible when locked)
				Column {
					width: parent.width
					spacing: Style.paddingXS
					visible: lockedCB.checkState === Qt.Checked

					Text {
						text: qsTr("Lock Reason")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					CustomTextField {
						id: lockReasonInput
						width: parent.width
						height: 40
						placeHolderText: qsTr("Reason for locking")
						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
					visible: !root.isNewIssue
				}

				// Status section (existing issues only)
				Column {
					width: parent.width
					spacing: Style.paddingXS
					visible: !root.isNewIssue

					Text {
						text: qsTr("Status")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: statusCB
						width: parent.width
						height: Style.buttonHeightM
						currentIndex: 0
						model: statusModel
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}

				// State reason section (existing issues only)
				Column {
					width: parent.width
					spacing: Style.paddingXS
					visible: !root.isNewIssue

					Text {
						text: qsTr("State Reason")
						font.pixelSize: Style.fontSizeS
						font.bold: true
						color: Style.textColor
					}

					ComboBox {
						id: stateReasonCB
						width: parent.width
						height: Style.buttonHeightM
						currentIndex: 0
						model: stateReasonModel
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}
				}
			}
		}
	}
}
