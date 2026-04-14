// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtdeskImtDeskSdl 1.0
import imtdeskTicketCollectionDocumentManagerSdl 1.0
import imtauthUsersSdl 1.0
import imtchatgui 1.0

DocumentViewBase {
	id: root
	
	anchors.fill: parent
	
	property TicketData ticketData: model
	property bool isNewIssue: ticketData ? (ticketData.m_number === 0) : true
	// Component factory for creating TicketComment instances
	property Component commentComp: Component { TicketComment {} }
	
	// Current user ID for chat-style alignment
	readonly property string currentUserId: AuthorizationController.getUserId()
	
	signal commentSubmitted(string commentText)
	
	onIsNewIssueChanged: {
		doUpdateGui()
	}
	
	function formatTimestamp(isoStr) {
		if (!isoStr) return ""
		var d = new Date(isoStr)
		if (isNaN(d.getTime())) return isoStr
		return d.toLocaleDateString(Qt.locale(), "d MMM yyyy") + " " + d.toLocaleTimeString(Qt.locale(), "HH:mm")
	}

	function addComment(commentText) {
		if (!commentText || commentText.length === 0 || !ticketData)
			return
		
		setBlockingUpdateModel(true)
		var userId = AuthorizationController.getUserId()
		var userName = AuthorizationController.userTokenProvider.login || ""
		var now = new Date().toISOString()
		
		if (!ticketData.hasComments()){
			ticketData.emplaceComments()
		}
		
		let newItem = ticketData.createCommentsArrayElement()
		if (!newItem){
			setBlockingUpdateModel(false)
			return
		}
		
		newItem.m_userId = userId
		newItem.m_userName = userName
		newItem.m_timestamp = now
		newItem.m_content = commentText
		newItem.m_reactions = []
		ticketData.m_comments.addElement(newItem)
		
		setBlockingUpdateModel(false)
		ticketData.modelChanged()
		root.commentSubmitted(commentText)
	}
	
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
		editTitleInput.text = ticketData.m_title || ""
		editDescriptionInput.text = ticketData.m_description || ""
		editTypeCB.currentIndex = findComboIndex(editTypeCB, ticketData.m_ticketType, 1)
		editPriorityCB.currentIndex = findComboIndex(editPriorityCB, ticketData.m_priority, 1)
		editStatusCB.currentIndex = findComboIndex(editStatusCB, ticketData.m_status, 0)
		editStateReasonCB.currentIndex = findComboIndex(editStateReasonCB, ticketData.m_stateReason, 0)
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
			let reporterId = ticketData.m_reporterId || AuthorizationController.getUserId()
			for (let i = 0; i < editReporterCB.model.getItemsCount(); i++) {
				if (editReporterCB.model.getData("id", i) === reporterId) {
					editReporterCB.currentIndex = i
					break
				}
			}
		}
	}
	
	function updateModel() {
		ticketData.m_title = editTitleInput.text
		ticketData.m_description = editDescriptionInput.text
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
			ticketData.m_reporterId = root.isNewIssue ? AuthorizationController.getUserId() : ""
		}
		
		if (editTypeCB.model && editTypeCB.currentIndex >= 0) {
			ticketData.m_ticketType = editTypeCB.model.getData("id", editTypeCB.currentIndex)
		}
		if (editPriorityCB.model && editPriorityCB.currentIndex >= 0) {
			ticketData.m_priority = editPriorityCB.model.getData("id", editPriorityCB.currentIndex)
		}
		if (editStatusCB.model && editStatusCB.currentIndex >= 0) {
			ticketData.m_status = editStatusCB.model.getData("id", editStatusCB.currentIndex)
		}
		if (editStateReasonCB.model && editStateReasonCB.currentIndex >= 0) {
			ticketData.m_stateReason = editStateReasonCB.model.getData("id", editStateReasonCB.currentIndex)
		}
	}
	
	// --- Shared data providers and models ---
	
	CollectionDataProvider {
		id: userCollectionProvider
		commandId: ImtauthUsersSdlCommandIds.s_usersList
		fields: [UserDataInputTypeMetaInfo.s_id, UserDataInputTypeMetaInfo.s_typeId, UserDataInputTypeMetaInfo.s_name]
		onCollectionModelChanged: {
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
	// Ticket editor view — same layout for both new and existing tickets
	// Left: scrollable content (title, description, activity)
	// Right: fixed sidebar (assignees, type, priority, reporter, status, etc.)
	// ================================================================
	
	Item {
		id: editView
		anchors.fill: parent
		
		CustomScrollbar {
			id: editScrollV
			z: parent.z + 1
			anchors.right: editSidebarSep.left
			anchors.rightMargin: Style.marginM
			anchors.top: editFlick.top
			anchors.bottom: editFlick.bottom
			secondSize: Style.marginM
			targetItem: editFlick
			visible: editView.visible
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
			anchors.rightMargin: Style.marginM
			contentHeight: editMainCol.height + Style.marginXL * 2
			boundsBehavior: Flickable.StopAtBounds
			clip: true
			
			Column {
				id: editMainCol
				width: editFlick.width
				spacing: Style.marginM
				
				// Status badges (only for existing tickets)
				Row {
					visible: !root.isNewIssue
					spacing: Style.spacingS
					
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
					spacing: Style.spacingS
					
					Text {
						text: qsTr("Title")
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
					}
					
					CustomTextField {
						id: editTitleInput
						width: parent.width
						height: Style.controlHeightM
						placeHolderText: qsTr("Title")
						onEditingFinished: {
							root.doUpdateModel()
						}
					}
				}
				
				// Description edit
				Column {
					width: parent.width
					spacing: Style.spacingS
					
					Text {
						text: qsTr("Description")
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
					}
					
					Rectangle {
						width: parent.width
						height: editDescriptionInput.height + Style.paddingM * 2
						radius: Style.radiusS
						border.color: editDescriptionInput.activeFocus
									  ? Style.accentColor
									  : Style.borderColor
						border.width: 1
						color: Style.baseColor
						
						TextEdit {
							id: editDescriptionInput
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.top: parent.top
							anchors.margins: Style.paddingM
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
							height: 100
							wrapMode: TextEdit.Wrap
							clip: true
							onEditingFinished: {
								root.doUpdateModel()
							}
							
							Text {
								anchors.fill: parent
								text: qsTr("Enter the description")
								color: Style.textPlaceholderColor
								font.pixelSize: Style.fontSizeM
								visible: editDescriptionInput.text.length === 0
							}
						}
					}
				}
				
				Column {
					visible: !root.isNewIssue
					width: parent.width
					spacing: Style.spacingS
					
					Row {
						width: parent.width
						spacing: Style.spacingS
						
						Text {
							text: qsTr("Comments")
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
							anchors.verticalCenter: parent.verticalCenter
						}
						
						Text {
							text: commentsThread.count > 0
								  ? "(" + commentsThread.count + ")"
								  : ""
							font.pixelSize: Style.fontSizeM
							color: Style.textSecondaryColor
							anchors.verticalCenter: parent.verticalCenter
						}
					}
					
					Column {
						id: commentsListCol
						width: parent.width
						spacing: Style.spacingS
						
						Repeater {
							id: commentsThread
							model: root.ticketData ? root.ticketData.m_comments : 0
							
							delegate: Item {
								id: commentDelegate
								width: commentsListCol.width
								height: commentBubbleCol.height
								
								readonly property bool isMe: model.item.m_userId === root.currentUserId
								
								Column {
									id: commentBubbleCol
									width: parent.width * 0.75
									anchors.right: commentDelegate.isMe ? parent.right : undefined
									anchors.left: commentDelegate.isMe ? undefined : parent.left
									spacing: Style.spacingXS
									
									// Header: sender name + timestamp
									Row {
										spacing: Style.paddingS
										anchors.right: commentDelegate.isMe ? parent.right : undefined
										anchors.left: commentDelegate.isMe ? undefined : parent.left
										layoutDirection: commentDelegate.isMe ? Qt.RightToLeft : Qt.LeftToRight
										
										// Avatar circle
										Rectangle {
											width: 32
											height: width
											radius: width / 2
											color: Style.accentColor
											
											Text {
												anchors.centerIn: parent
												text: model.item.m_userName ? model.item.m_userName.charAt(0).toUpperCase() : "?"
												font.pixelSize: Style.fontSizeS
												font.bold: true
												color: Style.textColor
												font.family: Style.fontFamily
											}
										}
										
										Text {
											text: model.item.m_userName || qsTr("Unknown")
											font.pixelSize: Style.fontSizeM
											font.bold: true
											color: Style.textColor
											anchors.verticalCenter: parent.verticalCenter
										}
										
										Text {
											text: root.formatTimestamp(model.item.m_timestamp)
											font.pixelSize: Style.fontSizeS
											color: Style.textSecondaryColor
											anchors.verticalCenter: parent.verticalCenter
										}
									}
									
									// Comment body bubble
									Rectangle {
										width: commentBubbleCol.width
										height: commentBodyText.contentHeight + Style.paddingM * 2
										radius: Style.radiusS
										color: Style.baseColor
										border.color: Style.borderColor
										border.width: 1
										anchors.right: commentDelegate.isMe ? parent.right : undefined
										anchors.left: commentDelegate.isMe ? undefined : parent.left
										
										Text {
											id: commentBodyText
											x: Style.paddingM
											y: Style.paddingM
											width: parent.width - Style.paddingM * 2
											text: model.item.m_content || ""
											font.pixelSize: Style.fontSizeM
											color: Style.textColor
											wrapMode: Text.Wrap
										}
									}
								}
							}
						}
					}
					
					// "Write" input area (shared between both view modes)
					Rectangle {
						width: parent.width
						height: 1
						color: Style.borderColor
						visible: !root.ticketData || !root.ticketData.m_locked
					}
					
					Column {
						width: parent.width
						spacing: Style.spacingS
						visible: !root.ticketData || !root.ticketData.m_locked
						
						Text {
							text: qsTr("Add a comment")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}
						
						Row {
							width: parent.width
							spacing: Style.paddingS
							
							Rectangle {
								width: 32
								height: 32
								radius: 16
								color: Style.accentColor
								anchors.top: parent.top
								
								Text {
									anchors.centerIn: parent
									text: qsTr("You")
									font.pixelSize: Style.fontSizeM
									color: "white"
									font.bold: true
								}
							}
							
							Column {
								width: parent.width - 32 - Style.paddingS
								spacing: Style.spacingS
								
								Rectangle {
									width: parent.width
									height: commentInputField.height + Style.paddingM * 2
									radius: Style.radiusS
									border.color: commentInputField.activeFocus
												  ? Style.accentColor
												  : Style.borderColor
									border.width: 1
									color: Style.baseColor
									
									TextEdit {
										id: commentInputField
										anchors.left: parent.left
										anchors.right: parent.right
										anchors.top: parent.top
										anchors.margins: Style.paddingM
										height: 60
										font.pixelSize: Style.fontSizeM
										color: Style.textColor
										wrapMode: TextEdit.Wrap
										clip: true
										
										Text {
											anchors.fill: parent
											text: qsTr("Leave a comment")
											color: Style.textPlaceholderColor
											font.pixelSize: Style.fontSizeM
											visible: commentInputField.text.length === 0
										}
									}
								}
								
								Row {
									anchors.right: parent.right
									spacing: Style.spacingS
									
									Rectangle {
										width: commentBtnText.contentWidth + Style.marginM * 2
										height: Style.buttonHeightM
										radius: Style.buttonRadius
										color: commentBtnMa.pressed
											   ? Qt.darker(Style.imaginToolsAccentColor, 1.2)
											   : commentBtnMa.containsMouse
												 ? Qt.lighter(Style.imaginToolsAccentColor, 1.1)
												 : Style.imaginToolsAccentColor
										
										Text {
											id: commentBtnText
											anchors.centerIn: parent
											text: qsTr("Comment")
											font.pixelSize: Style.fontSizeM
											font.family: Style.fontFamily
											color: "white"
										}
										
										MouseArea {
											id: commentBtnMa
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: {
												root.addComment(commentInputField.text.trim())
												commentInputField.text = ""
											}
										}
									}
								}
							}
						}
					}
					
					// Lock notice
					Row {
						visible: root.ticketData && root.ticketData.m_locked
						width: parent.width
						spacing: Style.paddingS
						
						Text {
							text: "🔒"
							font.pixelSize: Style.fontSizeM
						}
						
						Text {
							text: qsTr("This conversation has been locked. Only collaborators can comment.")
							font.pixelSize: Style.fontSizeS
							color: Style.textSecondaryColor
							wrapMode: Text.Wrap
							width: parent.width - Style.fontSizeM - Style.paddingS
						}
					}
				}
			}
		}
		
		// Separator between left content and sidebar
		Rectangle {
			id: editSidebarSep
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.right: editSidebar.left
			anchors.rightMargin: Style.marginXL
			width: 1
			color: Style.borderColor
		}
		
		// Right: full sidebar (always visible, not scrollable)
		Column {
			id: editSidebar
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
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
				spacing: Style.spacingS
				
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
				spacing: Style.spacingS
				
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
			
			Rectangle { visible: !root.isNewIssue; width: parent.width; height: 1; color: Style.borderColor }
			
			// Reporter (only for existing tickets)
			Column {
				visible: !root.isNewIssue
				width: parent.width
				spacing: Style.spacingS
				
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
			
			Rectangle { visible: !root.isNewIssue; width: parent.width; height: 1; color: Style.borderColor }
			
			// Lock issue (only for existing tickets)
			Row {
				visible: !root.isNewIssue
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
				spacing: Style.spacingS
				visible: !root.isNewIssue && editLockedCB.checkState === Qt.Checked
				
				Text {
					text: qsTr("Lock Reason")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textColor
				}
				
				CustomTextField {
					id: editLockReasonInput
					width: parent.width
					height: Style.controlHeightM
					placeHolderText: qsTr("Reason for locking")
					onEditingFinished: {
						root.doUpdateModel()
					}
				}
			}
			
			Rectangle { visible: !root.isNewIssue; width: parent.width; height: 1; color: Style.borderColor }
			
			// Status (only for existing tickets)
			Column {
				visible: !root.isNewIssue
				width: parent.width
				spacing: Style.spacingS
				
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
			
			// State Reason (only for existing tickets)
			Column {
				visible: !root.isNewIssue
				width: parent.width
				spacing: Style.spacingS
				
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

