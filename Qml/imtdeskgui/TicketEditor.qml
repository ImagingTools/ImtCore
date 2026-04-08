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

	// --- Helper: find ComboBox index by "id" field ---
	function findComboIndex(combo, targetId, fallback) {
		if (combo.model) {
			for (let i = 0; i < combo.model.getItemsCount(); i++) {
				if (combo.model.getData("id", i) === targetId) {
					return i
				}
			}
		}
		return fallback
	}

	function updateGui() {
		if (root.isNewIssue) {
			newTitleInput.text = ticketData.m_title || ""
			newDescriptionInput.text = ticketData.m_description || ""
			newTypeCB.currentIndex = findComboIndex(newTypeCB, ticketData.m_ticketType, 1)
			newPriorityCB.currentIndex = findComboIndex(newPriorityCB, ticketData.m_priority, 1)
			newEnvironmentCB.currentIndex = findComboIndex(newEnvironmentCB, ticketData.m_environment, 2)
			newMilestoneInput.text = ticketData.m_milestoneId || ""

			newAssigneeCB.currentIndex = -1
			if (newAssigneeCB.model) {
				let ids = ticketData.m_assigneeIds || []
				let first = ids.length > 0 ? ids[0] : ""
				for (let i = 0; i < newAssigneeCB.model.getItemsCount(); i++) {
					if (newAssigneeCB.model.getData("id", i) === first) {
						newAssigneeCB.currentIndex = i
						break
					}
				}
			}
		}
		else {
			editTitleInput.text = ticketData.m_title || ""
			editDescriptionInput.text = ticketData.m_description || ""
			editTypeCB.currentIndex = findComboIndex(editTypeCB, ticketData.m_ticketType, 1)
			editPriorityCB.currentIndex = findComboIndex(editPriorityCB, ticketData.m_priority, 1)
			editEnvironmentCB.currentIndex = findComboIndex(editEnvironmentCB, ticketData.m_environment, 2)
			editStatusCB.currentIndex = findComboIndex(editStatusCB, ticketData.m_status, 0)
			editStateReasonCB.currentIndex = findComboIndex(editStateReasonCB, ticketData.m_stateReason, 0)
			editMilestoneInput.text = ticketData.m_milestoneId || ""
			editConversationInput.text = ticketData.m_conversationId || ""
			editLockReasonInput.text = ticketData.m_lockReason || ""
			editLockedCB.checkState = ticketData.m_locked ? Qt.Checked : Qt.Unchecked

			editAssigneeCB.currentIndex = -1
			if (editAssigneeCB.model) {
				let ids = ticketData.m_assigneeIds || []
				let first = ids.length > 0 ? ids[0] : ""
				for (let i = 0; i < editAssigneeCB.model.getItemsCount(); i++) {
					if (editAssigneeCB.model.getData("id", i) === first) {
						editAssigneeCB.currentIndex = i
						break
					}
				}
			}

			editReporterCB.currentIndex = -1
			if (editReporterCB.model) {
				for (let i = 0; i < editReporterCB.model.getItemsCount(); i++) {
					if (editReporterCB.model.getData("id", i) === ticketData.m_reporterId) {
						editReporterCB.currentIndex = i
						break
					}
				}
			}
		}
	}

	function updateModel() {
		if (root.isNewIssue) {
			ticketData.m_title = newTitleInput.text
			ticketData.m_description = newDescriptionInput.text
			ticketData.m_milestoneId = newMilestoneInput.text

			if (newAssigneeCB.model && newAssigneeCB.currentIndex >= 0) {
				ticketData.m_assigneeIds = [newAssigneeCB.model.getData("id", newAssigneeCB.currentIndex)]
			}
			else {
				ticketData.m_assigneeIds = []
			}

			if (newTypeCB.model && newTypeCB.currentIndex >= 0) {
				ticketData.m_ticketType = newTypeCB.model.getData("id", newTypeCB.currentIndex)
			}
			if (newPriorityCB.model && newPriorityCB.currentIndex >= 0) {
				ticketData.m_priority = newPriorityCB.model.getData("id", newPriorityCB.currentIndex)
			}
			if (newEnvironmentCB.model && newEnvironmentCB.currentIndex >= 0) {
				ticketData.m_environment = newEnvironmentCB.model.getData("id", newEnvironmentCB.currentIndex)
			}
		}
		else {
			ticketData.m_title = editTitleInput.text
			ticketData.m_description = editDescriptionInput.text
			ticketData.m_milestoneId = editMilestoneInput.text
			ticketData.m_conversationId = editConversationInput.text
			ticketData.m_locked = editLockedCB.checkState === Qt.Checked
			ticketData.m_lockReason = editLockReasonInput.text

			if (editAssigneeCB.model && editAssigneeCB.currentIndex >= 0) {
				ticketData.m_assigneeIds = [editAssigneeCB.model.getData("id", editAssigneeCB.currentIndex)]
			}
			else {
				ticketData.m_assigneeIds = []
			}

			if (editReporterCB.model && editReporterCB.currentIndex >= 0) {
				ticketData.m_reporterId = editReporterCB.model.getData("id", editReporterCB.currentIndex)
			}
			else {
				ticketData.m_reporterId = ""
			}

			if (editTypeCB.model && editTypeCB.currentIndex >= 0) {
				ticketData.m_ticketType = editTypeCB.model.getData("id", editTypeCB.currentIndex)
			}
			if (editPriorityCB.model && editPriorityCB.currentIndex >= 0) {
				ticketData.m_priority = editPriorityCB.model.getData("id", editPriorityCB.currentIndex)
			}
			if (editEnvironmentCB.model && editEnvironmentCB.currentIndex >= 0) {
				ticketData.m_environment = editEnvironmentCB.model.getData("id", editEnvironmentCB.currentIndex)
			}
			if (editStatusCB.model && editStatusCB.currentIndex >= 0) {
				ticketData.m_status = editStatusCB.model.getData("id", editStatusCB.currentIndex)
			}
			if (editStateReasonCB.model && editStateReasonCB.currentIndex >= 0) {
				ticketData.m_stateReason = editStateReasonCB.model.getData("id", editStateReasonCB.currentIndex)
			}
		}
	}

	// --- Shared data providers and models ---

	CollectionDataProvider {
		id: userCollectionProvider
		commandId: ImtauthUsersSdlCommandIds.s_usersList
		fields: [UserDataInputTypeMetaInfo.s_id, UserDataInputTypeMetaInfo.s_typeId, UserDataInputTypeMetaInfo.s_name]
		onCollectionModelChanged: {
			newAssigneeCB.model = collectionModel
			editAssigneeCB.model = collectionModel
			editReporterCB.model = collectionModel
			root.doUpdateGui()
		}
		Component.onCompleted: {
			updateModel()
		}
	}

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

	// ================================================================
	// VIEW 1: CREATE NEW ISSUE (isNewIssue === true)
	// Like GitHub's "New Issue" page — clean form, green Submit button
	// ================================================================

	Item {
		id: createView
		visible: root.isNewIssue
		anchors.fill: parent

		CustomScrollbar {
			id: createScrollV
			z: parent.z + 1
			anchors.right: parent.right
			anchors.top: createFlick.top
			anchors.bottom: createFlick.bottom
			secondSize: Style.marginM
			targetItem: createFlick
			visible: createView.visible
		}

		Flickable {
			id: createFlick
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			anchors.right: createScrollV.left
			anchors.rightMargin: Style.marginXL
			contentWidth: createRow.width
			contentHeight: createRow.height + Style.marginXL * 2
			boundsBehavior: Flickable.StopAtBounds
			clip: true

			Row {
				id: createRow
				spacing: Style.marginXL

				// Left: main content
				Column {
					id: createMainCol
					width: 560
					spacing: Style.marginM

					// Title
					Column {
						width: parent.width
						spacing: Style.spacingS

						Text {
							text: qsTr("Add a title")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: newTitleInput
							width: parent.width
							height: Style.controlHeightM
							placeHolderText: qsTr("Title")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					// Description
					Column {
						width: parent.width
						spacing: Style.spacingS

						Text {
							text: qsTr("Add a description")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						TextEditCustom {
							id: newDescriptionInput
							width: parent.width
							isDinamicHeight: false
							height: 300
							placeHolderText: qsTr("Leave a comment")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}
				}

				// Separator
				Rectangle {
					width: 1
					height: createMainCol.height
					color: Style.borderColor
				}

				// Right: sidebar
				Column {
					width: 260
					spacing: Style.spacingM

					// Assignees
					Column {
						width: parent.width
						spacing: Style.spacingS
						
						Text {
							text: qsTr("Assignees")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: newAssigneeCB
							width: parent.width
							height: Style.controlHeightM
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Type
					Column {
						width: parent.width
						spacing: Style.spacingS

						Text {
							text: qsTr("Type")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: newTypeCB
							width: parent.width
							height: Style.controlHeightM
							currentIndex: 1
							model: ticketTypeModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Priority
					Column {
						width: parent.width
						spacing: Style.spacingS

						Text {
							text: qsTr("Priority")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: newPriorityCB
							width: parent.width
							height: Style.controlHeightM
							currentIndex: 1
							model: priorityModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Milestone
					Column {
						width: parent.width
						spacing: Style.spacingS

						Text {
							text: qsTr("Milestone")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: newMilestoneInput
							width: parent.width
							height: Style.controlHeightM
							placeHolderText: qsTr("No milestone")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Environment
					Column {
						width: parent.width
						spacing: Style.spacingS

						Text {
							text: qsTr("Environment")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: newEnvironmentCB
							width: parent.width
							height: Style.controlHeightM
							currentIndex: 2
							model: environmentModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}
				}
			}
		}
	}

	// ================================================================
	// VIEW 2: EDIT EXISTING ISSUE (isNewIssue === false)
	// Like GitHub's Issue detail page — header with #N, status badges,
	// full sidebar, Close/Reopen action buttons
	// ================================================================

	Item {
		id: editView
		visible: !root.isNewIssue
		anchors.fill: parent

		CustomScrollbar {
			id: editScrollV
			z: parent.z + 1
			anchors.right: parent.right
			anchors.top: editFlick.top
			anchors.bottom: editFlick.bottom
			secondSize: Style.marginM
			targetItem: editFlick
			visible: editView.visible
		}

		CustomScrollbar {
			id: editScrollH
			z: parent.z + 1
			anchors.left: editFlick.left
			anchors.right: editFlick.right
			anchors.bottom: editFlick.bottom
			secondSize: Style.marginM
			vertical: false
			targetItem: editFlick
		}

		Flickable {
			id: editFlick
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			anchors.right: editScrollV.left
			anchors.rightMargin: Style.marginXL
			contentWidth: editRow.width
			contentHeight: editRow.height + Style.marginXL * 2
			boundsBehavior: Flickable.StopAtBounds
			clip: true

			Row {
				id: editRow
				spacing: Style.marginXL

				// Left: main content
				Column {
					id: editMainCol
					width: 560
					spacing: Style.marginM

					// Header: title + #number
					Row {
						width: parent.width
						spacing: Style.paddingS

						Text {
							text: ticketData ? ticketData.m_title : ""
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
							wrapMode: Text.Wrap
							width: parent.width - editNumberText.width - Style.paddingS
						}

						Text {
							id: editNumberText
							text: "#" + (ticketData ? ticketData.m_number : 0)
							font.pixelSize: Style.fontSizeXL
							color: Style.textSecondaryColor
						}
					}

					// Status badges
					Row {
						spacing: Style.paddingXS

						TicketBadge {
							badgeType: "status"
							value: editStatusCB.currentIndex
						}

						TicketBadge {
							badgeType: "stateReason"
							value: editStateReasonCB.currentIndex
							visible: editStateReasonCB.currentIndex > 0
						}
					}

					// Title edit
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Title")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: editTitleInput
							width: parent.width
							height: 40
							placeHolderText: qsTr("Title")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					// Description edit
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Description")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: editDescriptionInput
							width: parent.width
							height: 200
							placeHolderText: qsTr("Leave a comment")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					// Action buttons
					Row {
						spacing: Style.paddingS

						// Close as completed (open issues)
						Rectangle {
							visible: editStatusCB.currentIndex === 0
							width: editCloseCompText.width + Style.paddingL * 2
							height: Style.buttonHeightM
							radius: Style.radiusS
							color: "#8957e5"

							Text {
								id: editCloseCompText
								anchors.centerIn: parent
								text: qsTr("Close as completed")
								font.pixelSize: Style.fontSizeM
								color: "white"
								font.bold: true
							}

							MouseArea {
								anchors.fill: parent
								onClicked: {
									editStatusCB.currentIndex = 1
									editStateReasonCB.currentIndex = 1
									root.doUpdateModel()
								}
							}
						}

						// Close as not planned (open issues)
						Rectangle {
							visible: editStatusCB.currentIndex === 0
							width: editCloseNPText.width + Style.paddingL * 2
							height: Style.buttonHeightM
							radius: Style.radiusS
							color: "transparent"
							border.color: Style.borderColor
							border.width: 1

							Text {
								id: editCloseNPText
								anchors.centerIn: parent
								text: qsTr("Close as not planned")
								font.pixelSize: Style.fontSizeM
								color: Style.textColor
							}

							MouseArea {
								anchors.fill: parent
								onClicked: {
									editStatusCB.currentIndex = 1
									editStateReasonCB.currentIndex = 2
									root.doUpdateModel()
								}
							}
						}

						// Reopen (closed issues)
						Rectangle {
							visible: editStatusCB.currentIndex === 1
							width: editReopenText.width + Style.paddingL * 2
							height: Style.buttonHeightM
							radius: Style.radiusS
							color: "#1a7f37"

							Text {
								id: editReopenText
								anchors.centerIn: parent
								text: qsTr("Reopen issue")
								font.pixelSize: Style.fontSizeM
								color: "white"
								font.bold: true
							}

							MouseArea {
								anchors.fill: parent
								onClicked: {
									editStatusCB.currentIndex = 0
									editStateReasonCB.currentIndex = 3
									root.doUpdateModel()
								}
							}
						}
					}
				}

				// Separator
				Rectangle {
					width: 1
					height: editMainCol.height
					color: Style.borderColor
				}

				// Right: full sidebar
				Column {
					width: 260
					spacing: 0

					// Assignees
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Assignees")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editAssigneeCB
							width: parent.width
							height: Style.buttonHeightM
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Type
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Type")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editTypeCB
							width: parent.width
							height: Style.buttonHeightM
							currentIndex: 1
							model: ticketTypeModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Priority
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Priority")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editPriorityCB
							width: parent.width
							height: Style.buttonHeightM
							currentIndex: 1
							model: priorityModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Milestone
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Milestone")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: editMilestoneInput
							width: parent.width
							height: 40
							placeHolderText: qsTr("No milestone")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Environment
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Environment")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editEnvironmentCB
							width: parent.width
							height: Style.buttonHeightM
							currentIndex: 2
							model: environmentModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Reporter
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Reporter")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editReporterCB
							width: parent.width
							height: Style.buttonHeightM
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Linked Conversation
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Linked Conversation")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: editConversationInput
							width: parent.width
							height: 40
							placeHolderText: qsTr("No conversation")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Lock issue
					Row {
						width: parent.width
						spacing: Style.paddingS

						CheckBox {
							id: editLockedCB
							text: qsTr("Lock issue")
							onCheckStateChanged: {
								root.doUpdateModel()
							}
						}
					}

					// Lock reason
					Column {
						width: parent.width
						spacing: Style.paddingXS
						visible: editLockedCB.checkState === Qt.Checked

						Text {
							text: qsTr("Lock Reason")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						CustomTextField {
							id: editLockReasonInput
							width: parent.width
							height: 40
							placeHolderText: qsTr("Reason for locking")
							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Status
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("Status")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editStatusCB
							width: parent.width
							height: Style.buttonHeightM
							currentIndex: 0
							model: statusModel
							onCurrentIndexChanged: {
								root.doUpdateModel()
							}
						}
					}

					// State Reason
					Column {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: qsTr("State Reason")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						ComboBox {
							id: editStateReasonCB
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
}
