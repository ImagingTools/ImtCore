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

		assigneeIdInput.currentIndex = -1
		if (assigneeIdInput.model){
			let assigneeIds = ticketData.m_assigneeIds || []
			let firstAssignee = assigneeIds.length > 0 ? assigneeIds[0] : ""
			for (let i = 0; i < assigneeIdInput.model.getItemsCount(); i++){
				let userId = assigneeIdInput.model.getData("id", i)
				if (firstAssignee === userId){
					assigneeIdInput.currentIndex = i
					break
				}
			}
		}

		reporterIdInput.currentIndex = -1
		if (reporterIdInput.model){
			for (let i = 0; i < reporterIdInput.model.getItemsCount(); i++){
				let userId = reporterIdInput.model.getData("id", i)
				if (ticketData.m_reporterId === userId){
					reporterIdInput.currentIndex = i
					break
				}
			}
		}

		conversationIdInput.text = ticketData.m_conversationId || ""
		milestoneIdInput.text = ticketData.m_milestoneId || ""
		lockReasonInput.text = ticketData.m_lockReason || ""
		lockedCB.checked = ticketData.m_locked || false

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

		if (assigneeIdInput.model && assigneeIdInput.currentIndex >= 0){
			ticketData.m_assigneeIds = [assigneeIdInput.model.getData("id", assigneeIdInput.currentIndex)]
		}
		else{
			ticketData.m_assigneeIds = []
		}

		if (reporterIdInput.model && reporterIdInput.currentIndex >= 0){
			ticketData.m_reporterId = reporterIdInput.model.getData("id", reporterIdInput.currentIndex)
		}
		else{
			ticketData.m_reporterId = ""
		}

		ticketData.m_conversationId = conversationIdInput.text
		ticketData.m_milestoneId = milestoneIdInput.text
		ticketData.m_locked = lockedCB.checked
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
			assigneeIdInput.model = collectionModel
			reporterIdInput.model = collectionModel
			root.doUpdateGui()
		}
		Component.onCompleted: {
			updateModel()
		}
	}

	RegularExpressionValidator {
		id: notEmptyRegexp
		regularExpression: /^(?!\s*$).+/;
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
		contentHeight: Math.max(mainRow.height + 2 * Style.marginXL + 100)
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Row {
			id: mainRow
			spacing: Style.marginXL

			// Left: main content area (title + description)
			Column {
				id: mainContentColumn
				width: 500
				spacing: Style.marginL

				// Header
				Text {
					text: root.isNewIssue ? qsTr("New Issue") : qsTr("Edit Issue #%1").arg(ticketData ? ticketData.m_number : 0)
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textPrimaryColor
				}

				// Status badge row (visible only for existing issues)
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

				GroupElementView {
					width: parent.width

					TextInputElementView {
						id: titleInput

						name: qsTr("Add a title")
						placeHolderText: qsTr("Title")
						textInputValidator: notEmptyRegexp
						showErrorWhenInvalid: true

						onEditingFinished: {
							root.doUpdateModel()
						}
					}

					TextInputElementView {
						id: descriptionInput

						name: qsTr("Add a description")
						placeHolderText: qsTr("Leave a comment")

						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Status action buttons (visible only for existing issues)
				Row {
					visible: !root.isNewIssue
					spacing: Style.paddingS

					// Close as completed
					Rectangle {
						visible: statusCB.currentIndex === 0
						width: closeCompletedText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "#8957e5"

						Text {
							id: closeCompletedText
							anchors.centerIn: parent
							text: qsTr("Close as completed")
							font.pixelSize: Style.fontSizeXS
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

					// Close as not planned
					Rectangle {
						visible: statusCB.currentIndex === 0
						width: closeNotPlannedText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "transparent"
						border.color: Style.textSecondaryColor

						Text {
							id: closeNotPlannedText
							anchors.centerIn: parent
							text: qsTr("Close as not planned")
							font.pixelSize: Style.fontSizeXS
							color: Style.textSecondaryColor
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

					// Reopen
					Rectangle {
						visible: statusCB.currentIndex === 1
						width: reopenText.width + Style.paddingL * 2
						height: Style.buttonHeightM
						radius: Style.radiusS
						color: "#1a7f37"

						Text {
							id: reopenText
							anchors.centerIn: parent
							text: qsTr("Reopen issue")
							font.pixelSize: Style.fontSizeXS
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
				color: Style.separatorColor
			}

			// Right: sidebar metadata
			Column {
				id: sidebarColumn
				width: 240
				spacing: Style.paddingM

				Text {
					text: qsTr("Metadata")
					font.pixelSize: Style.fontSizeS
					font.bold: true
					color: Style.textSecondaryColor
				}

				// Separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.separatorColor
				}

				GroupElementView {
					width: parent.width

					ComboBoxElementView {
						id: assigneeIdInput
						name: qsTr("Assignee")
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}

					ComboBoxElementView {
						id: ticketTypeCB
						name: qsTr("Type")
						currentIndex: 1
						model: TreeItemModel {
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

						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}

					ComboBoxElementView {
						id: priorityCB
						name: qsTr("Priority")
						currentIndex: 1
						model: TreeItemModel {
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

						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}

					TextInputElementView {
						id: milestoneIdInput

						name: qsTr("Milestone")
						placeHolderText: qsTr("Milestone ID")

						onEditingFinished: {
							root.doUpdateModel()
						}
					}

					ComboBoxElementView {
						id: environmentCB
						name: qsTr("Environment")
						currentIndex: 2
						model: TreeItemModel {
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

						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}

					ComboBoxElementView {
						id: reporterIdInput
						name: qsTr("Reporter")
						onCurrentIndexChanged: {
							root.doUpdateModel()
						}
					}

					TextInputElementView {
						id: conversationIdInput

						name: qsTr("Linked Conversation")
						placeHolderText: qsTr("Conversation ID")

						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}

				// Lock section separator
				Rectangle {
					width: parent.width
					height: 1
					color: Style.separatorColor
				}

				// Hidden status/stateReason controls (used by action buttons)
				ComboBoxElementView {
					id: statusCB
					name: qsTr("Status")
					visible: !root.isNewIssue
					currentIndex: 0
					model: TreeItemModel {
						Component.onCompleted: {
							let index = insertNewItem()
							setData("id", "Open", index)
							setData("name", "Open", index)

							index = insertNewItem()
							setData("id", "Closed", index)
							setData("name", "Closed", index)
						}
					}

					onCurrentIndexChanged: {
						root.doUpdateModel()
					}
				}

				ComboBoxElementView {
					id: stateReasonCB
					name: qsTr("State Reason")
					visible: !root.isNewIssue
					currentIndex: 0
					model: TreeItemModel {
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

					onCurrentIndexChanged: {
						root.doUpdateModel()
					}
				}

				GroupElementView {
					width: parent.width

					CheckBoxElementView {
						id: lockedCB
						name: qsTr("Lock issue")
						onCheckedChanged: {
							root.doUpdateModel()
						}
					}

					TextInputElementView {
						id: lockReasonInput

						name: qsTr("Lock Reason")
						placeHolderText: qsTr("Reason for locking")
						visible: lockedCB.checked

						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}
			}
		}
	}
}
