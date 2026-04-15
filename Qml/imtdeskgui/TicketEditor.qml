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
import Qt.labs.platform 1.1

DocumentViewBase {
	id: root
	
	anchors.fill: parent
	
	property TicketData ticketData: model
	property bool isNewIssue: ticketData ? (ticketData.m_number === 0) : true
	// Component factory for creating TicketComment instances
	property Component commentComp: Component { TicketComment {} }
	
	// Current user ID for chat-style alignment
	readonly property string currentUserId: AuthorizationController.getUserId()
	
	// Pending image attachments for comment being composed
	// Each element: {id: "uuid.ext", preview: "localPreviewUrl"}
	property var pendingAttachments: []
	// Number of uploads currently in progress
	property int uploadsInProgress: 0
	
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

	function formatCommentHtml(content) {
		if (!content) return ""
		return content
			.replace(/&/g, "&amp;")
			.replace(/</g, "&lt;")
			.replace(/>/g, "&gt;")
			.replace(/"/g, "&quot;")
			.replace(/'/g, "&#39;")
			.replace(/\\n/g, "<br>")
			.replace(/\n/g, "<br>")
	}

	function addComment(commentText, attachmentsList) {
		if ((!commentText || commentText.length === 0) && (!attachmentsList || attachmentsList.length === 0))
			return
		if (!ticketData)
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
		newItem.m_content = commentText || ""
		newItem.m_reactions = []
		if (attachmentsList && attachmentsList.length > 0) {
			newItem.emplaceAttachments()
			for (var i = 0; i < attachmentsList.length; i++) {
				var attObj = attachmentsList[i]
				var att = newItem.createAttachmentsArrayElement()
				att.m_id = String(attObj.id || "")
				att.m_fileName = String(attObj.fileName || "")
				att.m_preview = "../../files/" + String(attObj.id || "")
				newItem.m_attachments.addElement(att)
			}
		}
		ticketData.m_comments.addElement(newItem)
		
		setBlockingUpdateModel(false)
		ticketData.modelChanged()
		root.commentSubmitted(commentText)
	}

	// Upload attachment file to the server via HTTP POST.
	// On success, adds {id, preview} to pendingAttachments.
	function uploadAttachment(fileObj, fileName, localPreview) {
		root.uploadsInProgress++

		if (Qt.platform.os === "web") {
			// Web: read binary data with FileReader and POST to /files/<name>
			var reader = new FileReader()
			reader.readAsArrayBuffer(fileObj)
			reader.onload = function() {
				var xhr = new XMLHttpRequest()
				xhr.open("POST", "../../files/" + encodeURIComponent(fileName))
				xhr.onreadystatechange = function() {
					if (xhr.readyState === XMLHttpRequest.DONE) {
						root.uploadsInProgress--
						if (xhr.status === 200) {
							var attachmentId = xhr.responseText.trim()
							var arr = root.pendingAttachments.slice()
							arr.push({id: attachmentId, fileName: fileName, preview: localPreview || ("../../files/" + attachmentId)})
							root.pendingAttachments = arr
						} else {
							console.error("Attachment upload failed: " + xhr.status + " " + xhr.responseText)
						}
					}
				}
				xhr.send(reader.result)
			}
			reader.onerror = function() {
				root.uploadsInProgress--
				console.error("Failed to read attachment file: " + fileName)
			}
		} else {
			// Native: read file with FileIO and POST binary data
			attachmentFileIO.source = localPreview
			var fileData = attachmentFileIO.read()
			var xhr = new XMLHttpRequest()
			xhr.open("POST", "../../files/" + encodeURIComponent(fileName))
			xhr.onreadystatechange = function() {
				if (xhr.readyState === XMLHttpRequest.DONE) {
					root.uploadsInProgress--
					if (xhr.status === 200) {
						var attachmentId = xhr.responseText.trim()
						var arr = root.pendingAttachments.slice()
						arr.push({id: attachmentId, fileName: fileName, preview: localPreview || ("../../files/" + attachmentId)})
						root.pendingAttachments = arr
					} else {
						console.error("Attachment upload failed: " + xhr.status + " " + xhr.responseText)
					}
				}
			}
			xhr.send(fileData)
		}
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
				
				// Ticket number + status badges (only for existing tickets)
				Row {
					visible: !root.isNewIssue
					spacing: Style.spacingS
					
					Text {
						text: "#" + (root.ticketData ? root.ticketData.m_number : "")
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textColor
						anchors.verticalCenter: parent.verticalCenter
					}
					
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
						spacing: Style.spacingL
						
						Repeater {
							id: commentsThread
							model: root.ticketData ? root.ticketData.m_comments : 0
							
							delegate: Item {
								id: commentDelegate
								width: commentsListCol.width
								height: commentBubbleCol.implicitHeight
								
								readonly property bool isMe: model.item.m_userId === root.currentUserId
								readonly property var dataModel: model.item
								
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
										height: bubbleContent.implicitHeight + Style.paddingM * 2
										radius: Style.radiusS
										color: Style.baseColor
										border.color: Style.borderColor
										border.width: 1
										anchors.right: commentDelegate.isMe ? parent.right : undefined
										anchors.left: commentDelegate.isMe ? undefined : parent.left
										
										Column {
											id: bubbleContent
											x: Style.paddingM
											y: Style.paddingM
											width: parent.width - Style.paddingM * 2
											spacing: Style.spacingS
											
											Text {
												id: commentBodyText
												width: parent.width
												textFormat: Text.StyledText
												text: root.formatCommentHtml(model.item.m_content)
												font.pixelSize: Style.fontSizeM
												color: Style.textColor
												wrapMode: Text.Wrap
												visible: text.length > 0
											}
											
											// Attachment file links
											Column {
												width: parent.width
												spacing: Style.spacingXS
												visible: commentDelegate.dataModel.m_attachments
												
												Repeater {
													model: commentDelegate.dataModel.m_attachments || []
													delegate: Item {
														width: bubbleContent.width
														height: attachLinkLabel.contentHeight + Style.paddingXS * 2
														
														Text {
															id: attachLinkLabel
															anchors.left: parent.left
															anchors.right: parent.right
															anchors.verticalCenter: parent.verticalCenter
															text: "📎 " + (model.item.m_fileName || qsTr("attachment"))
															font.pixelSize: Style.fontSizeS
															font.underline: true
															color: Style.accentColor
															wrapMode: Text.Wrap
														}
														
														MouseArea {
															anchors.fill: parent
															cursorShape: Qt.PointingHandCursor
															hoverEnabled: true
															onClicked: {
																if (model.item.m_preview) Qt.openUrlExternally(model.item.m_preview)
															}
														}
													}
												}
											}
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
									color: Style.textColor
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
								
								// Pending attachments preview
								Flow {
									width: parent.width
									spacing: Style.spacingS
									visible: root.pendingAttachments.length > 0 || root.uploadsInProgress > 0
									
									Repeater {
										model: root.pendingAttachments
										delegate: Rectangle {
											readonly property real maxPillWidth: 250
											width: Math.min(pendingFileLabel.implicitWidth + pendingRemoveBtn.width + Style.paddingM * 3, maxPillWidth)
											height: Style.buttonHeightS
											radius: Style.radiusS
											border.color: Style.borderColor
											border.width: 1
											color: Style.baseColor

											Text {
												id: pendingFileLabel
												anchors.left: parent.left
												anchors.leftMargin: Style.paddingM
												anchors.right: pendingRemoveBtn.left
												anchors.rightMargin: Style.paddingS
												anchors.verticalCenter: parent.verticalCenter
												text: "📎 " + (modelData.fileName || qsTr("attachment"))
												font.pixelSize: Style.fontSizeS
												color: Style.textColor
												elide: Text.ElideMiddle
												maximumLineCount: 1
											}

											Text {
												id: pendingRemoveBtn
												anchors.right: parent.right
												anchors.rightMargin: Style.paddingM
												anchors.verticalCenter: parent.verticalCenter
												text: "✕"
												font.pixelSize: Style.fontSizeS
												color: Style.errorColor

												MouseArea {
													anchors.fill: parent
													cursorShape: Qt.PointingHandCursor
													onClicked: {
														var arr = root.pendingAttachments.slice()
														arr.splice(index, 1)
														root.pendingAttachments = arr
													}
												}
											}
										}
									}
									
									// Upload in progress indicator
									Rectangle {
										width: uploadingLabel.contentWidth + Style.paddingM * 2
										height: Style.buttonHeightS
										radius: Style.radiusS
										border.color: Style.borderColor
										border.width: 1
										color: Style.baseColor
										visible: root.uploadsInProgress > 0
										
										Text {
											id: uploadingLabel
											anchors.centerIn: parent
											text: "⏳ " + qsTr("Uploading...")
											font.pixelSize: Style.fontSizeS
											color: Style.textSecondaryColor
										}
									}
								}
								
								Row {
									anchors.right: parent.right
									spacing: Style.spacingS
									
									// Attach image button
									Rectangle {
										width: attachBtnContent.contentWidth + Style.marginM * 2
										height: Style.buttonHeightM
										radius: Style.buttonRadius
										color: attachBtnMa.pressed
											   ? Qt.darker(Style.borderColor, 1.1)
											   : attachBtnMa.containsMouse
												 ? Qt.lighter(Style.borderColor, 1.1)
												 : Style.baseColor
										border.color: Style.borderColor
										border.width: 1
										
										Text {
											id: attachBtnContent
											anchors.centerIn: parent
											text: "📎 " + qsTr("Attach")
											font.pixelSize: Style.fontSizeM
											font.family: Style.fontFamily
											color: Style.textColor
										}
										
										MouseArea {
											id: attachBtnMa
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: attachImageDialog.open()
										}
									}
									
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
											cursorShape: root.uploadsInProgress > 0 ? Qt.WaitCursor : Qt.PointingHandCursor
											onClicked: {
												if (root.uploadsInProgress > 0)
													return
												root.addComment(commentInputField.text.trim(), root.pendingAttachments.slice())
												commentInputField.text = ""
												root.pendingAttachments = []
											}
										}
									}
								}
							}
						}
						
						// File dialog for image attachments
						FileDialog {
							id: attachImageDialog
							title: qsTr("Attach image")
							fileMode: FileDialog.OpenFile
							nameFilters: [qsTr("Image files") + " (*.png *.jpg *.jpeg *.gif *.bmp *.svg *.webp)"]
							
							onAccepted: {
								if (Qt.platform.os === "web") {
									// Web: file is a JS File object — upload binary data via HTTP POST
									var fileObj = attachImageDialog.file
									var fileName = fileObj.name || "attachment"
									// Read as dataURL for local preview while upload is in progress
									var previewReader = new FileReader()
									previewReader.readAsDataURL(fileObj)
									previewReader.onload = function() {
										root.uploadAttachment(fileObj, fileName, previewReader.result)
									}
									previewReader.onerror = function() {
										// Upload without local preview
										console.warn("Failed to generate preview for: " + fileName)
										root.uploadAttachment(fileObj, fileName, "")
									}
								} else {
									// Native: read file via FileIO and upload via HTTP POST
									var filePath = String(attachImageDialog.file)
									var parts = filePath.replace("file:///", "").split("/")
									var nativeFileName = parts.length > 0 ? parts[parts.length - 1] : "attachment"
									root.uploadAttachment(null, nativeFileName, filePath)
								}
							}
						}
						
						FileIO {
							id: attachmentFileIO
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

