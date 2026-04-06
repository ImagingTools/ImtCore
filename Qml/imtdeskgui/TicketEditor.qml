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
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0

ViewBase {
	id: root

	anchors.fill: parent

	property TicketData ticketData: model

	function updateGui(){
		titleInput.text = ticketData.m_title
		descriptionInput.text = ticketData.m_description || ""
		assigneeIdInput.text = ticketData.m_assigneeId || ""
		reporterIdInput.text = ticketData.m_reporterId || ""
		conversationIdInput.text = ticketData.m_conversationId || ""

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
		ticketData.m_assigneeId = assigneeIdInput.text
		ticketData.m_reporterId = reporterIdInput.text
		ticketData.m_conversationId = conversationIdInput.text

		if (ticketTypeCB.model && ticketTypeCB.currentIndex >= 0){
			ticketData.m_ticketType = ticketTypeCB.model.getData("id", ticketTypeCB.currentIndex)
		}

		if (statusCB.model && statusCB.currentIndex >= 0){
			ticketData.m_status = statusCB.model.getData("id", statusCB.currentIndex)
		}

		if (priorityCB.model && priorityCB.currentIndex >= 0){
			ticketData.m_priority = priorityCB.model.getData("id", priorityCB.currentIndex)
		}

		if (environmentCB.model && environmentCB.currentIndex >= 0){
			ticketData.m_environment = environmentCB.model.getData("id", environmentCB.currentIndex)
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

	CustomScrollbar{
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
		contentWidth: bodyColumn.width
		contentHeight: Math.max(bodyColumn.height + 2 * Style.marginXL + 100)
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: 700
			spacing: Style.marginXL

			GroupElementView {
				width: parent.width
				TextInputElementView {
					id: titleInput
	
					name: qsTr("Title")
					placeHolderText: qsTr("Enter the ticket title")
					textInputValidator: notEmptyRegexp
					showErrorWhenInvalid: true
	
					onEditingFinished: {
						root.doUpdateModel()
					}
				}
	
				RegularExpressionValidator {
					id: notEmptyRegexp
					regularExpression: /^(?!\s*$).+/;
				}
	
				TextInputElementView {
					id: descriptionInput
	
					name: qsTr("Description")
					placeHolderText: qsTr("Describe the issue in detail")
	
					onEditingFinished: {
						root.doUpdateModel()
					}
				}
	
				ComboBoxElementView {
					id: ticketTypeCB
					name: qsTr("Ticket Type")
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
					id: statusCB
					name: qsTr("Status")
					currentIndex: 0
					model: TreeItemModel {
						Component.onCompleted: {
							let index = insertNewItem()
							setData("id", "Open", index)
							setData("name", "Open", index)
	
							index = insertNewItem()
							setData("id", "InProgress", index)
							setData("name", "In Progress", index)
	
							index = insertNewItem()
							setData("id", "Resolved", index)
							setData("name", "Resolved", index)
	
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
	
				TextInputElementView {
					id: assigneeIdInput
	
					name: qsTr("Assignee ID")
					placeHolderText: qsTr("Enter assignee user ID")
	
					onEditingFinished: {
						root.doUpdateModel()
					}
				}
	
				TextInputElementView {
					id: reporterIdInput
	
					name: qsTr("Reporter ID")
					placeHolderText: qsTr("Enter reporter user ID")
	
					onEditingFinished: {
						root.doUpdateModel()
					}
				}
	
				TextInputElementView {
					id: conversationIdInput
	
					name: qsTr("Linked Conversation")
					placeHolderText: qsTr("Conversation ID (links to Conversations page)")
	
					onEditingFinished: {
						root.doUpdateModel()
					}
				}
			}
		}
	}
}
