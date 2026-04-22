// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
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
	contentColor: Style.baseColor
	
	property TicketData ticketData: model
	property bool isNewIssue: ticketData ? (ticketData.m_number === 0) : true
	// Component factory for creating TicketComment instances
	property Component commentComp: Component { TicketComment {} }
	
	// Current user ID for chat-style alignment
	readonly property string currentUserId: AuthorizationController.getUserId()
	
	// Permission flags from server
	readonly property bool canEdit: isNewIssue || (ticketData ? (ticketData.m_canEdit === true) : false)
	readonly property bool canLock: isNewIssue || (ticketData ? (ticketData.m_canLock === true) : false)
	// Whether user is reporter (for lock-only access)
	readonly property bool isReporter: ticketData ? (ticketData.m_reporterId === currentUserId) : false
	
	// Pending image attachments for comment being composed
	// Each element: {id: "uuid.ext", preview: "localPreviewUrl"}
	property var pendingAttachments: []
	// Number of uploads currently in progress
	property int uploadsInProgress: 0
	// Pending entity references for the ticket
	// Each element: {entityType, entityId, displayName, entityLinkPath, typeId}
	property var pendingEntityRefs: []
	// Force chat scroll to bottom after sending a message
	property bool _forceScrollToBottom: false
	// Track whether entity refs changed to avoid unnecessary emplace calls
	property bool _entityRefsChanged: false
	// Title inline edit mode (display vs edit)
	property bool _titleEditing: root.isNewIssue
	// Pending assignees for multi-select
	property var pendingAssignees: []
	property bool _assigneesChanged: false
	// Reply-to message context (null = not replying)
	property var _replyToMessage: null
	
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

	function isImageAttachment(fileName) {
		if (!fileName) return false
		var lower = String(fileName).toLowerCase()
		return lower.endsWith(".png") || lower.endsWith(".jpg") || lower.endsWith(".jpeg")
				|| lower.endsWith(".gif") || lower.endsWith(".bmp") || lower.endsWith(".webp")
				|| lower.endsWith(".svg")
	}

	function chatParticipants() {
		var out = []
		var seen = {}
		if (!ticketData || !ticketData.m_comments) return out
		for (var i = 0; i < ticketData.m_comments.count; i++) {
			var item = ticketData.m_comments.get(i).item
			if (!item) continue
			var uid = String(item.m_userId || "")
			if (!uid || seen[uid]) continue
			seen[uid] = true
			out.push({ id: uid, name: String(item.m_userName || "") })
		}
		return out
	}

	function priorityColor(priorityId) {
		switch (String(priorityId || "")) {
		case "Critical": return "#D92D20"
		case "High": return "#F79009"
		case "Medium": return "#1570EF"
		case "Low": return "#12B76A"
		default: return "#98A2B3"
		}
	}

	function statusColor(statusId) {
		return String(statusId || "") === "Closed" ? "#12B76A" : "#F79009"
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
		// Set replyToId if replying to a message
		if (root._replyToMessage && root._replyToMessage.id) {
			newItem.m_replyToId = String(root._replyToMessage.id)
			newItem.m_replyToContent = String(root._replyToMessage.content || "")
			newItem.m_replyToUserName = String(root._replyToMessage.userName || "")
		}
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
		root._forceScrollToBottom = true
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
							arr.push({id: attachmentId, fileName: fileName, preview: "../../files/" + attachmentId})
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
						arr.push({id: attachmentId, fileName: fileName, preview: "../../files/" + attachmentId})
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
		
		// Populate entity type model from server-provided data
		entityTypeModel.clear()
		if (ticketData && ticketData.m_entityTypes) {
			var types = ticketData.m_entityTypes
			for (var t = 0; t < types.count; t++) {
				var typeItem = types.get(t).item
				if (typeItem) {
					var idx = entityTypeModel.insertNewItem()
					entityTypeModel.setData("id", typeItem.m_id, idx)
					entityTypeModel.setData("name", typeItem.m_name, idx)
				}
			}
		}
		
		// Load entity references from ticket data
		var refs = []
		if (ticketData.hasEntityReferences && ticketData.hasEntityReferences()) {
			var entityRefs = ticketData.m_entityReferences
			if (entityRefs) {
				for (var r = 0; r < entityRefs.count; r++) {
					var refItem = entityRefs.get(r).item
					if (refItem) {
						var linkPath = ""
						if (refItem.m_entityLink && refItem.m_entityLink.m_url && refItem.m_entityLink.m_url.m_path) {
							linkPath = String(refItem.m_entityLink.m_url.m_path)
						}
						refs.push({
									  entityType: String(refItem.m_entityType || ""),
									  entityId: String(refItem.m_entityId || ""),
									  displayName: String(refItem.m_displayName || ""),
									  entityLinkPath: linkPath
								  })
					}
				}
			}
		}
		root.pendingEntityRefs = refs
		root._entityRefsChanged = true
		
		// Load assignees as array for multi-select
		var assigns = []
		var aIds = ticketData.m_assigneeIds || []
		for (var a = 0; a < aIds.length; a++) {
			var aId = aIds[a]
			var aName = aId
			if (editAssigneeCB.model) {
				for (var ai = 0; ai < editAssigneeCB.model.getItemsCount(); ai++) {
					if (editAssigneeCB.model.getData("id", ai) === aId) {
						aName = editAssigneeCB.model.getData("name", ai) || aId
						break
					}
				}
			}
			assigns.push({id: aId, name: aName})
		}
		root.pendingAssignees = assigns
		root._assigneesChanged = false
		
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
		
		ticketData.m_assigneeIds = root.pendingAssignees.map(function(a) { return a.id })
		
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
		
		// Save entity references to ticket data only when they actually changed
		if (root._entityRefsChanged) {
			root._entityRefsChanged = false
			ticketData.emplaceEntityReferences()
			for (var r = 0; r < root.pendingEntityRefs.length; r++) {
				var ref = root.pendingEntityRefs[r]
				var refItem = ticketData.createEntityReferencesArrayElement()
				refItem.m_entityType = String(ref.entityType || "")
				refItem.m_entityId = String(ref.entityId || "")
				refItem.m_displayName = String(ref.displayName || "")
				// Build ObjectLink with navigation path
				refItem.emplaceEntityLink()
				refItem.m_entityLink.m_id = String(ref.entityId || "")
				refItem.m_entityLink.m_typeId = String(ref.typeId || ref.entityType || "")
				refItem.m_entityLink.m_name = String(ref.displayName || "")
				refItem.m_entityLink.emplaceUrl()
				refItem.m_entityLink.m_url.m_scheme = "applink"
				refItem.m_entityLink.m_url.m_path = String(ref.entityLinkPath || "")
				ticketData.m_entityReferences.addElement(refItem)
			}
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
	
	TreeItemModel {
		id: entityTypeModel
	}
	
	// ================================================================
	// Modern SaaS-level 2-column layout: LEFT (3 cards) + RIGHT (chat)
	// Chat dominates; left panel is compact and structured
	// ================================================================
	
	Item {
		id: editView
		anchors.fill: parent
		
		// Layout constants
		readonly property real contentMaxWidth: 800
		readonly property real cardPadding: 16
		readonly property real cardRadius: Style.radiusL
		readonly property string cardColor: Style.baseColor
		readonly property string cardBorderColor: Style.borderColor
		readonly property string pageBgColor: Style.backgroundColor2
		readonly property string accentColor: Style.imaginToolsAccentColor
		readonly property string labelColor: Style.textColor
		readonly property real avatarSize: 36
		readonly property real accentDividerHeight: 2
		readonly property real badgeHeight: 22
		readonly property string accentBgLight: "#DFECF9"
		readonly property string accentBorderLight: "#B4D3F2"
		readonly property string accentBadgeBg: "#E5F0FB"
		readonly property string chatBgColor: Style.baseColor
		readonly property string bubbleColor: "#DFECF9"
		readonly property string otherBubbleColor: "#F1F3F7"
		readonly property string myBubbleColor: "#EAF3FF"
		readonly property string sectionLabelColor: "#8C95A6"
		readonly property string timestampColor: Style.inactiveTextColor
		readonly property real columnGap: Style.spacingL
		readonly property real editButtonWidth: 72
		readonly property real avatarOverlap: -8
		readonly property real bubbleWidthRatio: 0.86
		readonly property real bubbleMaxWidth: 680
		readonly property real imageAttachmentWidth: 132
		readonly property real imageAttachmentHeight: 96
		readonly property real fileAttachmentMaxWidth: 240
		readonly property real fileAttachmentHeight: 44
		// 2-column layout proportions: left 35–45%, right 55–65%
		readonly property real leftColumnRatio: 0.4
		readonly property real leftColumnMinWidth: 380
		readonly property real leftColumnMaxWidth: 760
		
		// Page background
		Rectangle {
			anchors.fill: parent
			color: editView.pageBgColor
		}
		
		// ==================== LEFT-ALIGNED CONTAINER ====================
		Item {
			id: panelsContainer
			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			
			// ==================== LEFT COLUMN: 3 CARDS ====================
			Item {
				id: topRow
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.bottom: parent.bottom
				width: root.isNewIssue
					? parent.width
					: Math.max(editView.leftColumnMinWidth,
							   Math.min(editView.leftColumnMaxWidth, parent.width * editView.leftColumnRatio))
				
				// LEFT: Title + Description (Context moved to Properties card)
				Item {
					id: leftTopWrapper
					anchors.left: parent.left
					anchors.top: parent.top
					width: parent.width
					height: detailsCard.height
					
					Rectangle {
						id: detailsCard
						width: parent.width
						height: detailsCardCol.height + editView.cardPadding * 2
						radius: editView.cardRadius
						color: editView.cardColor
						border.color: editView.cardBorderColor
						border.width: 1
						
						Column {
							id: detailsCardCol
							x: editView.cardPadding
							y: editView.cardPadding
							width: parent.width - editView.cardPadding * 2
							spacing: Style.spacingM
							
							// Title display/edit row — H1, page anchor
							Row {
								id: titleDisplayRow
								visible: !root._titleEditing
								width: parent.width
								spacing: Style.spacingS
								
								Text {
									text: root.isNewIssue ? qsTr("New Ticket") : "#" + (root.ticketData ? root.ticketData.m_number : "") + "  " + editTitleInput.text
									font.pixelSize: Style.fontSizeXXL
									font.bold: true
									color: Style.textColor
									elide: Text.ElideRight
									width: parent.width - titleEditBtn.width - titleBadgeRow.width - Style.spacingS * 3
									anchors.verticalCenter: parent.verticalCenter
								}
								
								Row {
									id: titleBadgeRow
									spacing: Style.spacingXS
									anchors.verticalCenter: parent.verticalCenter
									
									TicketBadge {
										visible: !root.isNewIssue
										badgeType: "status"
										value: editStatusCB.currentIndex
									}
									
									TicketBadge {
										visible: !root.isNewIssue && editStateReasonCB.currentIndex > 0
										badgeType: "stateReason"
										value: editStateReasonCB.currentIndex
									}
								}
								
								Rectangle {
									id: titleEditBtn
									visible: root.canEdit
									width: Math.min(120, Math.max(editView.editButtonWidth, editLabel.contentWidth + Style.paddingS * 2))
									height: 28
									radius: 14
									color: titleEditBtnMa.containsMouse ? "#F0F2F5" : "transparent"
									anchors.verticalCenter: parent.verticalCenter

									Text {
										id: editLabel
										anchors.centerIn: parent
										text: qsTr("Edit")
										font.pixelSize: Style.fontSizeM
										color: Style.textColor
										font.bold: true
									}
									
									MouseArea {
										id: titleEditBtnMa
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											editTitleInput.oldText = editTitleInput.text
											root._titleEditing = true
											editTitleInput.forceActiveFocus()
										}
									}
								}
							}
							
							// Title edit row
							Row {
								id: titleEditRow
								visible: root._titleEditing
								width: parent.width
								spacing: Style.spacingS
								
								CustomTextField {
									id: editTitleInput
									width: parent.width - titleConfirmBtn.width - titleCloseBtn.width - 2*Style.spacingS
									height: Style.controlHeightM
									placeHolderText: qsTr("Enter ticket title...")
									readOnly: !root.canEdit
									KeyNavigation.tab: editDescriptionInput
									onAccepted: {
										root._titleEditing = false
										root.doUpdateModel()
									}
									onCancelled: {
										root._titleEditing = false
									}
									property string oldText
								}
								
								Rectangle {
									id: titleCloseBtn
									visible: !root.isNewIssue
									width: visible ? 28 : 0
									height: width
									radius: width/2
									color: titleCloseBtn.containsMouse ? "#E6F4EA" : "#F0F2F5"
									anchors.verticalCenter: parent.verticalCenter
									
									Image {
										anchors.centerIn: parent
										width: Style.iconSizeS
										height: width
										source: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
										sourceSize.width: width
										sourceSize.height: height
									}
									
									MouseArea {
										id: titleCloseBtnMa
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											editTitleInput.text = editTitleInput.oldText
											editTitleInput.oldText = ""
											editTitleInput.cancelled()
										}
									}
								}

								Rectangle {
									id: titleConfirmBtn
									visible: !root.isNewIssue
									width: visible ? 28 : 0
									height: width
									radius: width/2
									color: titleConfirmBtnMa.containsMouse ? "#E6F4EA" : "#F0F2F5"
									anchors.verticalCenter: parent.verticalCenter
									
									Image {
										anchors.centerIn: parent
										width: Style.iconSizeS
										height: width
										source: Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
										sourceSize.width: width
										sourceSize.height: height
									}
									
									MouseArea {
										id: titleConfirmBtnMa
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											editTitleInput.accepted()
										}
									}
								}
							}
							
							// ---------- Description ----------
							Column {
								width: parent.width
								spacing: 4
								
								Text {
									text: qsTr("Description")
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: editView.sectionLabelColor
								}
								
								Rectangle {
									width: parent.width
									height: Math.min(220, Math.max(50, editDescriptionInput.contentHeight)) + Style.paddingM * 2
									radius: Style.radiusM
									border.color: editDescriptionInput.activeFocus ? editView.accentColor : editView.cardBorderColor
									border.width: editDescriptionInput.activeFocus ? 2 : 1
									color: editDescriptionInput.activeFocus ? editView.cardColor : "#FAFBFC"
									
									Flickable {
										id: descriptionFlick
										anchors.fill: parent
										anchors.margins: Style.paddingM
										anchors.rightMargin: Style.paddingM + Style.marginM
										contentWidth: width
										contentHeight: editDescriptionInput.height
										clip: true
										boundsBehavior: Flickable.StopAtBounds
										
										TextEdit {
											id: editDescriptionInput
											width: descriptionFlick.width
											height: Math.max(50, contentHeight)
											font.pixelSize: Style.fontSizeM
											color: Style.textColor
											wrapMode: TextEdit.Wrap
											// textFormat: TextEdit.PlainText
											readOnly: !root.canEdit
											onEditingFinished: root.doUpdateModel()
											KeyNavigation.tab: editTypeCB
											KeyNavigation.backtab: editTitleInput
											
											onCursorRectangleChanged: {
												var cy = cursorRectangle.y
												var ch = cursorRectangle.height
												if (cy < descriptionFlick.contentY) {
													descriptionFlick.contentY = cy
												} else if (cy + ch > descriptionFlick.contentY + descriptionFlick.height) {
													descriptionFlick.contentY = cy + ch - descriptionFlick.height
												}
											}
											
											Text {
												anchors.fill: parent
												text: qsTr("Describe the issue...")
												color: Style.inactiveTextColor
												font.pixelSize: Style.fontSizeM
												visible: editDescriptionInput.text.length === 0
											}
										}
									}
									
									CustomScrollbar {
										id: descriptionScrollV
										z: parent.z + 1
										anchors.right: parent.right
										anchors.rightMargin: 2
										anchors.top: parent.top
										anchors.bottom: parent.bottom
										anchors.topMargin: 2
										anchors.bottomMargin: 2
										secondSize: Style.marginM
										targetItem: descriptionFlick
										visible: descriptionFlick.contentHeight > descriptionFlick.height
									}
								}
							}
							
						}
					}
					
					DropShadow {
						anchors.fill: detailsCard
						z: detailsCard.z - 1
						horizontalOffset: 3
						verticalOffset: 3
						radius: Style.radiusL
						spread: 0
						color: Style.shadowColor
						source: detailsCard
					}
				} // leftTopWrapper
				
				// MIDDLE: Context card
				Item {
					id: contextWrapper
					anchors.left: parent.left
					anchors.top: leftTopWrapper.bottom
					anchors.topMargin: editView.columnGap
					width: leftTopWrapper.width
					height: contextCard.height

					Rectangle {
						id: contextCard
						width: parent.width
						height: contextCardCol.height + editView.cardPadding * 2
						radius: editView.cardRadius
						color: editView.cardColor
						border.color: editView.cardBorderColor
						border.width: 1

						Column {
							id: contextCardCol
							x: editView.cardPadding
							y: editView.cardPadding
							width: parent.width - editView.cardPadding * 2
							spacing: Style.spacingS

							Item {
								width: parent.width
								height: Math.max(contextLabelText.height, addContextBtn.height)

								Row {
									anchors.left: parent.left
									anchors.verticalCenter: parent.verticalCenter
									spacing: Style.spacingS

									Text {
										id: contextLabelText
										text: qsTr("Context")
										font.pixelSize: Style.fontSizeM
										font.bold: true
										color: Style.textColor
										anchors.verticalCenter: parent.verticalCenter
									}

									Rectangle {
										visible: root.pendingEntityRefs.length > 0
										width: refCountLabel.contentWidth + Style.paddingS * 2
										height: editView.badgeHeight - 2
										radius: (editView.badgeHeight - 2) / 2
										color: editView.accentColor
										anchors.verticalCenter: parent.verticalCenter

										Text {
											id: refCountLabel
											anchors.centerIn: parent
											text: root.pendingEntityRefs.length
											font.pixelSize: Style.fontSizeM - 1
											font.bold: true
											color: Style.baseColor
										}
									}
								}

								Text {
									id: addContextBtn
									anchors.right: parent.right
									anchors.verticalCenter: parent.verticalCenter
									visible: root.canEdit
									text: "+ " + qsTr("Add context")
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: editView.accentColor

									MouseArea {
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											ModalDialogManager.openDialog(contextPickerDialogComp)
										}
									}
								}
							}

							Flow {
								width: parent.width
								spacing: Style.spacingXS
								visible: root.pendingEntityRefs.length > 0

								Repeater {
									model: root.pendingEntityRefs
									delegate: Rectangle {
										readonly property real maxRefWidth: 260
										width: Math.min(refLabelText.contentWidth + refRemoveBtn.width + Style.paddingS * 3, maxRefWidth)
										height: 28
										radius: 14
										color: editView.accentBgLight
										border.color: editView.accentBorderLight
										border.width: 1

										Text {
											id: refLabelText
											anchors.left: parent.left
											anchors.leftMargin: Style.paddingS + 2
											anchors.verticalCenter: parent.verticalCenter
											text: (modelData.entityType ? "[" + modelData.entityType + "] " : "") + (modelData.displayName || modelData.entityId || "")
											font.pixelSize: Style.fontSizeM
											color: editView.accentColor
											font.underline: modelData.entityLinkPath !== ""
											elide: Text.ElideRight
											maximumLineCount: 1

											MouseArea {
												anchors.fill: parent
												hoverEnabled: true
												cursorShape: Qt.PointingHandCursor
												onClicked: {
													if (modelData.entityLinkPath) {
														NavigationController.navigate(modelData.entityLinkPath)
													}
												}
											}
										}

										ToolButton {
											id: refRemoveBtn
											visible: root.canEdit
											anchors.right: parent.right
											anchors.verticalCenter: parent.verticalCenter
											iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
											decorator: Component {
												ToolButtonDecorator {
													color: "transparent"
													icon.width: Style.iconSizeXS
												}
											}
											onClicked: {
												var arr = root.pendingEntityRefs.slice()
												arr.splice(index, 1)
												root.pendingEntityRefs = arr
												root._entityRefsChanged = true
												root.doUpdateModel()
											}
										}
									}
								}
							}

							Text {
								visible: root.pendingEntityRefs.length === 0
								width: parent.width
								text: qsTr("No linked entities. Click \"+ Add context\" to attach entities to this ticket.")
								font.pixelSize: Style.fontSizeM
								color: Style.inactiveTextColor
								wrapMode: Text.WordWrap
							}

							Component {
								id: contextPickerDialogComp
								Dialog {
									id: ctxDialog
									title: qsTr("Link Entity to Ticket")
									canMove: false
									width: Math.min(ModalDialogManager.activeView.width - 80, 900)
									height: ModalDialogManager.activeView.height - 80

									property string selectedEntityTypeId: ""
									property RemoteCollectionView collectionView: null

									Component.onCompleted: {
										addButton(Enums.apply, qsTr("Attach Selected"), false)
										addButton(Enums.cancel, qsTr("Cancel"), true)
										setButtonEnabled(Enums.apply, false)
										if (entityTypeModel.getItemsCount() > 0) {
											selectedEntityTypeId = entityTypeModel.getData("id", 0)
										}
									}

									contentComp: Component {
										Item {
											width: ctxDialog.width
											height: ctxDialog.height - 100

											Component.onCompleted: {
												ctxTypeCB.model = entityTypeModel
												if (entityTypeModel.getItemsCount() > 0) {
													ctxTypeCB.currentIndex = 0
												}
											}

											Column {
												id: ctxContentCol
												anchors.fill: parent
												anchors.margins: Style.paddingM
												spacing: Style.spacingM

												Rectangle {
													width: parent.width
													height: ctxTypeRow.height + Style.paddingM * 2
													radius: Style.radiusM
													color: editView.accentBadgeBg
													border.color: editView.accentBorderLight
													border.width: 1

													Row {
														id: ctxTypeRow
														anchors.centerIn: parent
														spacing: Style.spacingM

														Text {
															text: qsTr("Entity type")
															font.pixelSize: Style.fontSizeM
															font.bold: true
															color: Style.textColor
															anchors.verticalCenter: parent.verticalCenter
														}

														ComboBox {
															id: ctxTypeCB
															width: 280
															height: Style.buttonHeightM
															onCurrentIndexChanged: {
																if (entityTypeModel.getItemsCount() > currentIndex) {
																	ctxDialog.selectedEntityTypeId = entityTypeModel.getData("id", currentIndex)
																	ctxDialog.setButtonEnabled(Enums.apply, false)
																}
															}
														}
													}
												}

												Item {
													width: parent.width
													height: parent.height - ctxTypeRow.height - Style.paddingM * 2 - Style.spacingM * 2

													Rectangle {
														anchors.fill: parent
														radius: Style.radiusM
														color: "transparent"
														border.color: Style.borderColor
														border.width: 1

														RemoteCollectionView {
															anchors.fill: parent
															anchors.margins: 1
															commandsControllerComp: null
															visibleMetaInfo: false
															commandsDelegateComp: null
															documentCollectionFilter: null
															showRemoteChangesAlert: false
															tableViewParamsStoredServer: false
															Component.onCompleted: {
																ctxDialog.collectionView = this
																collectionId = ctxDialog.selectedEntityTypeId
															}
															onSelectionChanged: {
																ctxDialog.setButtonEnabled(Enums.apply, selectedIds.length > 0)
															}
														}
													}
												}
											}
										}
									}

									onFinished: {
										if (buttonId === Enums.apply && collectionView) {
											var arr = root.pendingEntityRefs.slice()
											var mdl = collectionView.table.elements
											var indexes = collectionView.table.getSelectedIndexes()
											for (var i = 0; i < indexes.length; i++) {
												var idx = indexes[i]
												var displayName = mdl.getData("name", idx)
												var typeId = mdl.getData("typeId", idx)
												var elementId = mdl.getData("id", idx)
												var linkPath = selectedEntityTypeId
												if (typeId) linkPath += "/" + typeId
												linkPath += "/" + elementId
												arr.push({
															 entityType: selectedEntityTypeId,
															 entityId: elementId,
															 displayName: displayName,
															 entityLinkPath: linkPath,
															 typeId: typeId
														 })
											}
											root.pendingEntityRefs = arr
											root._entityRefsChanged = true
											root.doUpdateModel()
										}
									}
								}
							}
						}
					}

					DropShadow {
						anchors.fill: contextCard
						z: contextCard.z - 1
						horizontalOffset: 3
						verticalOffset: 3
						radius: Style.radiusL
						spread: 0
						color: Style.shadowColor
						source: contextCard
					}
				}

				// BOTTOM: Properties card
				Item {
					id: rightTopWrapper
					anchors.left: parent.left
					anchors.top: contextWrapper.bottom
					anchors.topMargin: editView.columnGap
					width: leftTopWrapper.width
					height: propertiesCard.height
					
					Rectangle {
						id: propertiesCard
						width: parent.width
						height: propsCardCol.height + editView.cardPadding * 2
						radius: editView.cardRadius
						color: editView.cardColor
						border.color: editView.cardBorderColor
						border.width: 1
						
						Column {
							id: propsCardCol
							x: editView.cardPadding
							y: editView.cardPadding
							width: parent.width - editView.cardPadding * 2
							spacing: Style.spacingM
							
							Text {
								text: qsTr("Properties")
								font.pixelSize: Style.fontSizeL
								font.bold: true
								color: Style.textColor
							}
							
							// Row 1: Type then Priority (stacked vertically in narrow Properties card)
							Column {
								width: parent.width
								spacing: Style.spacingM
								
								Column {
									width: parent.width
									spacing: 4
									
									Text {
										text: qsTr("Type")
										font.pixelSize: Style.fontSizeM
										color: editView.sectionLabelColor
									}
									
									ComboBox {
										id: editTypeCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 1
										model: ticketTypeModel
										enabled: root.canEdit
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editPriorityCB
										KeyNavigation.backtab: editDescriptionInput
									}
								}
								
								Column {
									width: parent.width
									spacing: 4
									
									Text {
										text: qsTr("Priority")
										font.pixelSize: Style.fontSizeM
										color: editView.sectionLabelColor
									}
									
									ComboBox {
										id: editPriorityCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 1
										model: priorityModel
										enabled: root.canEdit
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editAssigneeCB
										KeyNavigation.backtab: editTypeCB
									}

									Row {
										spacing: Style.spacingXS
										Rectangle {
											width: 8
											height: 8
											radius: 4
											color: root.priorityColor(editPriorityCB.model && editPriorityCB.currentIndex >= 0
																	 ? editPriorityCB.model.getData("id", editPriorityCB.currentIndex) : "")
											anchors.verticalCenter: parent.verticalCenter
										}
										Text {
											text: qsTr("Priority level")
											font.pixelSize: Style.fontSizeM - 1
											color: Style.inactiveTextColor
										}
									}
								}

								Column {
									visible: !root.isNewIssue
									width: parent.width
									spacing: 4
									
									Text {
										text: qsTr("Status")
										font.pixelSize: Style.fontSizeM
										color: editView.sectionLabelColor
									}
									
									ComboBox {
										id: editStatusCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 0
										model: statusModel
										enabled: root.canEdit
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editLockedCB
										KeyNavigation.backtab: editAssigneeCB
									}

									Rectangle {
										width: statusBadgeText.contentWidth + Style.paddingS * 2
										height: 22
										radius: 11
										color: root.statusColor(editStatusCB.model && editStatusCB.currentIndex >= 0
																 ? editStatusCB.model.getData("id", editStatusCB.currentIndex) : "")
										opacity: 0.15
										border.width: 1
										border.color: root.statusColor(editStatusCB.model && editStatusCB.currentIndex >= 0
																		? editStatusCB.model.getData("id", editStatusCB.currentIndex) : "")

										Text {
											id: statusBadgeText
											anchors.centerIn: parent
											text: editStatusCB.model && editStatusCB.currentIndex >= 0
												  ? editStatusCB.model.getData("name", editStatusCB.currentIndex)
												  : qsTr("Unknown")
											font.pixelSize: Style.fontSizeM - 1
											font.bold: true
											color: root.statusColor(editStatusCB.model && editStatusCB.currentIndex >= 0
																	 ? editStatusCB.model.getData("id", editStatusCB.currentIndex) : "")
										}
									}
								}
							}
							
							// Row 2: Assignees (multi-select with chips)
							Column {
								width: parent.width
								spacing: 4
								
								Item {
									width: parent.width
									height: Math.max(assigneesHeaderRow.height, addAssigneeBtn.height)
									
									Row {
										id: assigneesHeaderRow
										anchors.left: parent.left
										anchors.verticalCenter: parent.verticalCenter
										spacing: Style.spacingS
										
										Text {
											id: assigneesLabelText
											text: qsTr("Assignees")
											font.pixelSize: Style.fontSizeM
											font.bold: true
											color: editView.sectionLabelColor
											anchors.verticalCenter: parent.verticalCenter
										}
										
										Rectangle {
											visible: root.pendingAssignees.length > 0
											width: assigneeCountLabel.contentWidth + Style.paddingS * 2
											height: editView.badgeHeight - 2
											radius: (editView.badgeHeight - 2) / 2
											color: editView.accentColor
											anchors.verticalCenter: parent.verticalCenter
											
											Text {
												id: assigneeCountLabel
												anchors.centerIn: parent
												text: root.pendingAssignees.length
												font.pixelSize: Style.fontSizeM - 1
												font.bold: true
												color: Style.baseColor
											}
										}
									}
									
									Text {
										id: addAssigneeBtn
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										visible: root.canEdit
										text: "+ " + qsTr("Add assignee")
										font.pixelSize: Style.fontSizeM
										font.bold: true
										color: editView.accentColor
										
										MouseArea {
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: ModalDialogManager.openDialog(assigneePickerDialogComp)
										}
									}
								}
								
								Flow {
									width: parent.width
									spacing: Style.spacingXS
									visible: root.pendingAssignees.length > 0
									
									Repeater {
										model: root.pendingAssignees
										delegate: Rectangle {
											width: Math.min(assigneeChipText.contentWidth + assigneeChipRemove.width + Style.paddingS * 3, 200)
											height: 28
											radius: 14
											color: editView.accentBgLight
											border.color: editView.accentBorderLight
											border.width: 1
											
											Text {
												id: assigneeChipText
												anchors.left: parent.left
												anchors.leftMargin: Style.paddingS + 2
												anchors.verticalCenter: parent.verticalCenter
												text: modelData.name || modelData.id
												font.pixelSize: Style.fontSizeM
												color: editView.accentColor
												elide: Text.ElideRight
												maximumLineCount: 1
											}
											
											ToolButton {
												id: assigneeChipRemove
												visible: root.canEdit
												anchors.right: parent.right
												anchors.verticalCenter: parent.verticalCenter
												iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
												decorator: Component {
													ToolButtonDecorator {
														color: "transparent"
														icon.width: Style.iconSizeXS
													}
												}
												onClicked: {
													var arr = root.pendingAssignees.slice()
													arr.splice(index, 1)
													root.pendingAssignees = arr
													root._assigneesChanged = true
													root.doUpdateModel()
												}
											}
										}
									}
								}
								
								Text {
									visible: root.pendingAssignees.length === 0
									width: parent.width
									text: qsTr("No assignees. Click \"+ Add\" to assign users.")
									font.pixelSize: Style.fontSizeM
									color: Style.inactiveTextColor
									wrapMode: Text.WordWrap
								}
								
								// Assignee picker dialog — RemoteCollectionView with Users collection
								Component {
									id: assigneePickerDialogComp
									Dialog {
										id: assigneeDialog
										title: qsTr("Select Assignees")
										canMove: false
										width: Math.min(ModalDialogManager.activeView.width - 80, 900)
										height: ModalDialogManager.activeView.height - 80
										
										property RemoteCollectionView collectionView: null
										
										Component.onCompleted: {
											addButton(Enums.apply, qsTr("Attach Selected"), false)
											addButton(Enums.cancel, qsTr("Cancel"), true)
											setButtonEnabled(Enums.apply, false)
										}
										
										contentComp: Component {
											Item {
												width: assigneeDialog.width
												height: assigneeDialog.height - 100
												
												Rectangle {
													anchors.fill: parent
													anchors.margins: Style.paddingM
													radius: Style.radiusM
													color: "transparent"
													border.color: Style.borderColor
													border.width: 1
													
													RemoteCollectionView {
														anchors.fill: parent
														anchors.margins: 1
														commandsControllerComp: null
														visibleMetaInfo: false
														commandsDelegateComp: null
														documentCollectionFilter: null
														showRemoteChangesAlert: false
														tableViewParamsStoredServer: false
														collectionId: "Users"
														Component.onCompleted: {
															assigneeDialog.collectionView = this
														}
														onSelectionChanged: {
															assigneeDialog.setButtonEnabled(Enums.apply, selectedIds.length > 0)
														}
													}
												}
											}
										}
										
										onFinished: {
											if (buttonId === Enums.apply && collectionView) {
												var arr = root.pendingAssignees.slice()
												var mdl = collectionView.table.elements
												let indexes = collectionView.table.getSelectedIndexes()
												for (var i = 0; i < indexes.length; i++) {
													let idx = indexes[i]
													var userId = mdl.getData("id", idx)
													var userName = mdl.getData("name", idx) || userId
													// Skip duplicates
													var exists = false
													for (var j = 0; j < arr.length; j++) {
														if (arr[j].id === userId) { exists = true; break }
													}
													if (!exists) {
														arr.push({id: userId, name: userName})
													}
												}
												root.pendingAssignees = arr
												root._assigneesChanged = true
												root.doUpdateModel()
											}
										}
									}
								}
							}
							
							// Hidden assignee ComboBox (data source for dialog)
							ComboBox {
								id: editAssigneeCB
								visible: false
								width: 0
								height: 0
							}
							
							// Hidden Reporter (data still tracked for model)
							ComboBox {
								id: editReporterCB
								visible: false
								width: 0
								height: 0
								onCurrentIndexChanged: root.doUpdateModel()
							}

							// Hidden State Reason (data still tracked for model)
							ComboBox {
								id: editStateReasonCB
								visible: false
								width: 0
								height: 0
								currentIndex: 0
								model: stateReasonModel
								onCurrentIndexChanged: root.doUpdateModel()
							}
							
							// Context moved to separate card
							// Lock section (existing tickets only, reporter/admin only)
							Column {
								visible: !root.isNewIssue && root.canLock
								width: parent.width
								spacing: Style.spacingS
								
								Rectangle { width: parent.width; height: 1; color: editView.cardBorderColor; opacity: 0.4 }
								
								CheckBox {
									id: editLockedCB
									text: qsTr("Lock issue")
									enabled: root.canLock
									onCheckStateChanged: root.doUpdateModel()
									KeyNavigation.tab: editLockedCB.checkState === Qt.Checked ? editLockReasonInput : commentInputField
									KeyNavigation.backtab: editStateReasonCB
								}
								
								Column {
									width: parent.width
									spacing: 4
									visible: editLockedCB.checkState === Qt.Checked
									
									Text {
										text: qsTr("Lock Reason")
										font.pixelSize: Style.fontSizeM
										color: editView.sectionLabelColor
									}
									
									CustomTextField {
										id: editLockReasonInput
										width: parent.width
										height: Style.controlHeightM
										placeHolderText: qsTr("Reason for locking")
										readOnly: !root.canLock
										onEditingFinished: root.doUpdateModel()
										KeyNavigation.tab: commentInputField
										KeyNavigation.backtab: editLockedCB
									}
								}
							}
						}
					}
					
					DropShadow {
						anchors.fill: propertiesCard
						z: propertiesCard.z - 1
						horizontalOffset: 3
						verticalOffset: 3
						radius: Style.radiusL
						spread: 0
						color: Style.shadowColor
						source: propertiesCard
					}
				} // rightTopWrapper
			} // topRow
			
			// ==================== RIGHT COLUMN: CHAT (PRIMARY AREA) ====================
			Rectangle {
				id: commentsPanel
				visible: !root.isNewIssue
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.left: topRow.right
				anchors.leftMargin: editView.columnGap
				anchors.right: parent.right
				radius: editView.cardRadius
				color: editView.cardColor
				border.color: editView.cardBorderColor
				border.width: 1
				clip: true
				
				// Chat header
				Rectangle {
					id: chatHeader
					anchors.top: parent.top
					anchors.topMargin: 1
					anchors.left: parent.left
					anchors.leftMargin: 1
					anchors.right: parent.right
					anchors.rightMargin: 1
					height: 48
					color: editView.cardColor
					radius: editView.cardRadius - 1
					
					// Bottom edge square-off
					Rectangle {
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.bottom: parent.bottom
						height: editView.cardRadius
						color: editView.cardColor
					}
					
					Row {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: editView.cardPadding
						anchors.right: parent.right
						anchors.rightMargin: editView.cardPadding
						spacing: Style.spacingS
						
						Text {
							text: qsTr("Comments")
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
							anchors.verticalCenter: parent.verticalCenter
						}
						
						Rectangle {
							visible: commentsThread.count > 0
							width: chatCountLabel.contentWidth + Style.paddingS * 2
							height: editView.badgeHeight
							radius: editView.badgeHeight / 2
							color: editView.accentColor
							anchors.verticalCenter: parent.verticalCenter
							
							Text {
								id: chatCountLabel
								anchors.centerIn: parent
								text: commentsThread.count
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.baseColor
							}
						}

						Row {
							anchors.verticalCenter: parent.verticalCenter
							spacing: editView.avatarOverlap

							Repeater {
								model: root.chatParticipants().slice(0, 5)
								delegate: Rectangle {
									width: 26
									height: 26
									radius: 13
									border.width: 2
									border.color: editView.cardColor
									color: modelData.id === root.currentUserId ? editView.accentColor : "#9AA5B8"

									Text {
										anchors.centerIn: parent
										text: modelData.name ? modelData.name.charAt(0).toUpperCase() : "?"
										font.pixelSize: Style.fontSizeM - 1
										font.bold: true
										color: Style.baseColor
									}
								}
							}
						}
					}
				}
				
				CustomScrollbar {
					id: commentsScrollV
					z: parent.z + 1
					anchors.right: parent.right
					anchors.rightMargin: 2
					anchors.top: commentsFlick.top
					anchors.bottom: commentsFlick.bottom
					secondSize: Style.marginM
					targetItem: commentsFlick
					visible: commentsPanel.visible
				}
				
				// Chat area with subtle tint
				Rectangle {
					anchors.top: chatHeader.bottom
					anchors.bottom: addCommentSection.visible ? addCommentSection.top
															  : lockNoticeRow.visible ? lockNoticeRow.top
																					  : parent.bottom
					anchors.left: parent.left
					anchors.leftMargin: 1
					anchors.right: parent.right
					anchors.rightMargin: 1
					color: editView.chatBgColor
				}
				
				Flickable {
					id: commentsFlick
					anchors.top: chatHeader.bottom
					anchors.topMargin: Style.spacingS
					anchors.bottom: addCommentSection.visible ? addCommentSection.top
															  : lockNoticeRow.visible ? lockNoticeRow.top
																					  : parent.bottom
					anchors.bottomMargin: Style.spacingS
					anchors.left: parent.left
					anchors.right: commentsScrollV.left
					anchors.rightMargin: Style.spacingS
					anchors.leftMargin: editView.cardPadding
					contentHeight: commentsMainCol.height + Style.spacingL
					boundsBehavior: Flickable.StopAtBounds
					clip: true

					function scrollToBottom() {
						var maxY = contentHeight - height
						if (maxY > 0) {
							contentY = maxY
						} else {
							contentY = 0
						}
					}
					
					function scrollToMessage(msgId) {
						if (!msgId) return
						for (var i = 0; i < commentsThread.count; i++) {
							var item = commentsThread.itemAt(i)
							if (item && item.dataModel && item.dataModel.m_id === msgId) {
								var pos = item.mapToItem(commentsFlick.contentItem, 0, 0)
								var targetY = pos.y
								var maxY = Math.max(0, contentHeight - height)
								contentY = Math.min(Math.max(0, targetY - 20), maxY)
								return
							}
						}
					}

					onContentHeightChanged: {
						scrollToBottom()
					}
					
					onHeightChanged: {
						var maxY = contentHeight - height
						if (maxY > 0 && contentY >= maxY - 80) {
							contentY = maxY
						}
					}

					Column {
						id: commentsMainCol
						width: parent.width
						spacing: Style.spacingM
						
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
									height: commentBubbleCol.height + topGap
									
									readonly property bool isMe: model.item.m_userId === root.currentUserId
									readonly property var dataModel: model.item
									readonly property string _prevUserId: index > 0 && commentsThread.itemAt(index - 1) ? commentsThread.itemAt(index - 1).dataModel.m_userId : ""
									readonly property bool isGroupedWithPrev: index > 0 && _prevUserId === model.item.m_userId && _prevUserId.length > 0
									readonly property int topGap: index === 0 ? 0 : (isGroupedWithPrev ? 2 : Style.spacingS)
									
									Column {
										id: commentBubbleCol
										y: commentDelegate.topGap
										width: parent.width
										spacing: 4
										
										// Chat bubble (contains avatar + name + timestamp + content)
										Rectangle {
											readonly property real maxBubbleWidth: Math.min(parent.width * editView.bubbleWidthRatio, editView.bubbleMaxWidth)
											width: maxBubbleWidth
											anchors.right: commentDelegate.isMe ? parent.right : undefined
											anchors.left: commentDelegate.isMe ? undefined : parent.left
											height: bubbleContent.height + Style.paddingM * 2
											radius: 12
											color: commentDelegate.isMe ? editView.myBubbleColor : editView.otherBubbleColor
											border.width: 0
											
											Column {
												id: bubbleContent
												x: Style.paddingM
												y: Style.paddingM
												width: parent.width - Style.paddingM * 2
												spacing: Style.spacingS
												
												// Avatar + Name + Timestamp — for first message of group
												Row {
													spacing: Style.spacingS
													visible: !commentDelegate.isGroupedWithPrev
													height: visible ? editView.avatarSize : 0
													
													Rectangle {
														width: editView.avatarSize
														height: editView.avatarSize
														radius: editView.avatarSize / 2
														color: commentDelegate.isMe ? editView.accentColor : "#9AA5B8"
														
														Text {
															anchors.centerIn: parent
															text: model.item.m_userName ? model.item.m_userName.charAt(0).toUpperCase() : "?"
															font.pixelSize: Style.fontSizeM
															font.bold: true
															color: Style.baseColor
															font.family: Style.fontFamily
														}
													}
													
													Column {
														anchors.top: parent.top
														anchors.topMargin: 2
														spacing: 2
														
														Text {
															text: commentDelegate.isMe ? qsTr("You") : (model.item.m_userName || qsTr("Unknown"))
															font.pixelSize: Style.fontSizeM
															font.bold: true
															color: Style.textColor
														}
														
														Text {
															text: root.formatTimestamp(model.item.m_timestamp)
															font.pixelSize: Style.fontSizeM - 1
															color: editView.timestampColor
														}
													}
												}
												
												// Reply-to indicator inside bubble (clickable to scroll to source)
												Rectangle {
													id: replyIndicator
													visible: (model.item.m_replyToId && model.item.m_replyToId.length > 0)
													width: parent.width
													height: replyBubbleCol.height + Style.paddingS
													radius: Style.radiusM
													color: replyIndicatorMA.containsMouse ? "#C7D2E6" : "#D7DFEE"
													
													MouseArea {
														id: replyIndicatorMA
														anchors.fill: parent
														hoverEnabled: true
														cursorShape: Qt.PointingHandCursor
														onClicked: commentsFlick.scrollToMessage(model.item.m_replyToId)
													}
													
													Column {
														id: replyBubbleCol
														x: Style.paddingS + 4
														y: Style.paddingS / 2
														width: parent.width - Style.paddingS * 2 - 4
														spacing: 1
														
														Row {
															spacing: Style.spacingXS
															
															Rectangle {
																width: 2
																height: parent.height
																radius: 1
																color: editView.accentColor
															}
															
															Column {
																spacing: 1
																
																Text {
																	text: model.item.m_replyToUserName || ""
																	font.pixelSize: Style.fontSizeM - 1
																	font.bold: true
																	color: editView.accentColor
																	elide: Text.ElideRight
																}
																
																Text {
																	text: model.item.m_replyToContent || ""
																	font.pixelSize: Style.fontSizeM - 1
																	color: Style.inactiveTextColor
																	elide: Text.ElideRight
																	maximumLineCount: 1
																	width: bubbleContent.width - Style.paddingS * 3
																}
															}
														}
													}
												}
												
												Text {
													id: commentBodyText
													width: parent.width
													textFormat: Text.StyledText
													text: root.formatCommentHtml(model.item.m_content)
													font.pixelSize: Style.fontSizeM
													color: Style.textColor
													wrapMode: Text.Wrap
													visible: text.length > 0
													lineHeight: 1.45
												}
												
												// Attachments — inline links with paperclip icon
												Flow {
													width: parent.width
													spacing: Style.spacingS
													visible: (commentDelegate.dataModel.m_attachments || []).length > 0

													Repeater {
														model: commentDelegate.dataModel.m_attachments || []
														delegate: Rectangle {
															readonly property bool isImage: root.isImageAttachment(model.item.m_fileName)
															width: isImage ? editView.imageAttachmentWidth : Math.min(parent.width, editView.fileAttachmentMaxWidth)
															height: isImage ? editView.imageAttachmentHeight : editView.fileAttachmentHeight
															radius: Style.radiusM
															color: isImage ? "#EEF2F8" : "#F6F8FC"
															border.color: activeFocus ? editView.accentColor : editView.cardBorderColor
															border.width: activeFocus ? 2 : 1
															clip: true

															Image {
																id: messageAttachmentImage
																visible: parent.isImage
																anchors.fill: parent
																fillMode: Image.PreserveAspectCrop
																source: model.item.m_preview || ""
															}

															Column {
																visible: parent.isImage && messageAttachmentImage.status === Image.Error
																anchors.centerIn: parent
																spacing: 2

																Image {
																	anchors.horizontalCenter: parent.horizontalCenter
																	width: Style.iconSizeS
																	height: width
																	source: Style.getIconPath("Icons/Attachment", Icon.State.On, Icon.Mode.Normal)
																	sourceSize.width: width
																	sourceSize.height: height
																}

																Text {
																	text: qsTr("Preview unavailable")
																	font.pixelSize: Style.fontSizeM - 2
																	color: Style.inactiveTextColor
																}
															}

															Row {
																visible: !parent.isImage
																anchors.fill: parent
																anchors.margins: Style.paddingS
																spacing: Style.spacingS

																Image {
																	width: Style.iconSizeS
																	height: width
																	source: Style.getIconPath("Icons/Attachment", Icon.State.On, Icon.Mode.Normal)
																	sourceSize.width: width
																	sourceSize.height: height
																	anchors.verticalCenter: parent.verticalCenter
																}

																Text {
																	width: parent.width - Style.iconSizeS - Style.spacingS
																	anchors.verticalCenter: parent.verticalCenter
																	text: model.item.m_fileName || qsTr("file")
																	font.pixelSize: Style.fontSizeM
																	color: Style.textColor
																	elide: Text.ElideMiddle
																}
															}

															MouseArea {
																anchors.fill: parent
																hoverEnabled: true
																cursorShape: Qt.PointingHandCursor
																onClicked: if (model.item.m_preview) Qt.openUrlExternally(model.item.m_preview)
															}

															activeFocusOnTab: true
															Keys.onReturnPressed: {
																if (model.item.m_preview) Qt.openUrlExternally(model.item.m_preview)
															}
															Keys.onEnterPressed: {
																if (model.item.m_preview) Qt.openUrlExternally(model.item.m_preview)
															}
														}
													}
												}
												
												// Reply button
												Text {
													visible: root.canEdit
													text: qsTr("Reply")
													font.pixelSize: Style.fontSizeM
													color: Style.inactiveTextColor
													// x: Style.paddingM
													
													MouseArea {
														anchors.fill: parent
														hoverEnabled: true
														cursorShape: Qt.PointingHandCursor
														onClicked: {
															root._replyToMessage = {
																id: model.item.m_id || "",
																userName: model.item.m_userName || qsTr("Unknown"),
																content: model.item.m_content || ""
															}
															commentInputField.forceActiveFocus()
														}
													}
												}
											}
										}
									}
								}
							}
							
							// Empty comments placeholder
							Item {
								visible: commentsThread.count === 0
								width: parent.width
								height: commentsFlick.height - Style.spacingL * 2
								
								Column {
									anchors.centerIn: parent
									spacing: Style.spacingS
									
									Text {
										anchors.horizontalCenter: parent.horizontalCenter
										text: qsTr("No comments yet")
										font.pixelSize: Style.fontSizeL
										font.bold: true
										color: Style.inactiveTextColor
									}
									
									Text {
										anchors.horizontalCenter: parent.horizontalCenter
										text: qsTr("Be the first to leave a comment.")
										font.pixelSize: Style.fontSizeM
										color: Style.inactiveTextColor
									}
								}
							}
						}
					}
				}
				
				// ---- Fixed bottom: Add comment input ----
				Rectangle {
					id: addCommentSection
					visible: root.canEdit && (!root.ticketData || !root.ticketData.m_locked)
					anchors.bottom: parent.bottom
					anchors.bottomMargin: 1
					anchors.left: parent.left
					anchors.leftMargin: 1
					anchors.right: parent.right
					anchors.rightMargin: 1
					height: addCommentInnerCol.height + editView.cardPadding + 12
					color: editView.cardColor
					radius: editView.cardRadius - 1
					
					// Square-off top edge
					Rectangle {
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						height: editView.cardRadius
						color: editView.cardColor
					}
					
					// Top border
					Rectangle {
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.leftMargin: Style.marginL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						height: 1
						color: editView.cardBorderColor
						opacity: 0.4
					}
					
					Column {
						id: addCommentInnerCol
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.bottom: parent.bottom
						anchors.leftMargin: editView.cardPadding
						anchors.rightMargin: editView.cardPadding
						anchors.bottomMargin: 12
						spacing: Style.spacingS
						
						// Reply-to indicator
						Rectangle {
							visible: root._replyToMessage !== null
							width: parent.width
							height: replyRow.height + Style.paddingS * 2
							radius: Style.radiusM
							color: editView.accentBadgeBg
							border.color: editView.accentBorderLight
							border.width: 1
							
							Row {
								id: replyRow
								anchors.left: parent.left
								anchors.right: replyCloseBtn.left
								anchors.leftMargin: Style.paddingS
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.spacingXS
								
								Rectangle {
									width: 3
									height: parent.height
									radius: 1
									color: editView.accentColor
								}
								
								Column {
									width: parent.width - 3 - Style.spacingXS
									spacing: 1
									
									Text {
										text: root._replyToMessage ? root._replyToMessage.userName : ""
										font.pixelSize: Style.fontSizeM
										font.bold: true
										color: editView.accentColor
										elide: Text.ElideRight
										width: parent.width
									}
									
									Text {
										text: root._replyToMessage ? root._replyToMessage.content : ""
										font.pixelSize: Style.fontSizeM
										color: Style.inactiveTextColor
										elide: Text.ElideRight
										width: parent.width
										maximumLineCount: 1
									}
								}
							}
							
							ToolButton {
								id: replyCloseBtn
								anchors.right: parent.right
								anchors.verticalCenter: parent.verticalCenter
								iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
								decorator: Component {
									ToolButtonDecorator {
										color: "transparent"
										icon.width: Style.iconSizeXS
									}
								}
								onClicked: root._replyToMessage = null
							}
						}
						
						// Pending attachments
						Flow {
							width: parent.width
							spacing: Style.spacingS
							visible: root.pendingAttachments.length > 0 || root.uploadsInProgress > 0
							
							Repeater {
								model: root.pendingAttachments
								delegate: Row {
									spacing: Style.spacingXS
									
									Image {
										anchors.verticalCenter: parent.verticalCenter
										width: Style.iconSizeS
										height: width
										source: Style.getIconPath("Icons/Attachment", Icon.State.On, Icon.Mode.Normal)
										sourceSize.width: width
										sourceSize.height: height
									}
									
									Text {
										anchors.verticalCenter: parent.verticalCenter
										text: modelData.fileName || qsTr("attachment")
										font.pixelSize: Style.fontSizeM
										color: Style.linkColor
										font.underline: true
										elide: Text.ElideMiddle
										
										MouseArea {
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: if (modelData.preview) Qt.openUrlExternally(modelData.preview)
										}
									}
									
									ToolButton {
										anchors.verticalCenter: parent.verticalCenter
										iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
										decorator: Component {
											ToolButtonDecorator {
												color: "transparent"
												icon.width: Style.iconSizeXS
											}
										}
										onClicked: {
											var removed = root.pendingAttachments[index]
											var arr = root.pendingAttachments.slice()
											arr.splice(index, 1)
											root.pendingAttachments = arr
											if (removed && removed.id) {
												var xhr = new XMLHttpRequest()
												xhr.open("DELETE", "../../files/" + encodeURIComponent(removed.id))
												xhr.onreadystatechange = function() {
													if (xhr.readyState === XMLHttpRequest.DONE && xhr.status !== 200) {
														console.warn("Failed to delete attachment from server:", xhr.status, xhr.responseText)
													}
												}
												xhr.send()
											}
										}
									}
								}
							}
							
							// Upload spinner
							Rectangle {
								width: uploadingLabel.contentWidth + Style.paddingM * 2
								height: 26
								radius: 13
								border.color: editView.cardBorderColor
								border.width: 1
								color: editView.pageBgColor
								visible: root.uploadsInProgress > 0
								
								Text {
									id: uploadingLabel
									anchors.centerIn: parent
									text: "⏳ " + qsTr("Uploading...")
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
								}
							}
						}
						
						// Input row: text field + actions
						Row {
							width: parent.width
							spacing: Style.spacingS
							
							// Comment input with rounded border and inline paperclip icon
							Rectangle {
								id: inputFieldRect
								property int maxInputHeight: 120
								width: parent.width - sendBtnRect.width - Style.spacingS
								// Extra 4px accounts for the border width change on focus (1→2px on each side)
								height: Math.min(maxInputHeight, Math.max(40, commentInputField.contentHeight + Style.paddingS * 2 + 4))
								radius: 20
								border.color: commentInputField.activeFocus ? editView.accentColor : editView.cardBorderColor
								border.width: commentInputField.activeFocus ? 2 : 1
								color: editView.cardColor
								
								Flickable {
									id: commentInputFlick
									anchors.left: parent.left
									anchors.right: attachButton.left
									anchors.top: parent.top
									anchors.bottom: parent.bottom
									anchors.leftMargin: 14
									anchors.rightMargin: 4
									anchors.topMargin: 4
									anchors.bottomMargin: 4
									contentWidth: width
									contentHeight: commentInputField.height
									clip: true
									boundsBehavior: Flickable.StopAtBounds
									
									TextEdit {
										id: commentInputField
										width: commentInputFlick.width
										height: Math.max(commentInputFlick.height, contentHeight)
										verticalAlignment: TextEdit.AlignVCenter
										font.pixelSize: Style.fontSizeM
										color: Style.textColor
										wrapMode: TextEdit.Wrap
										// textFormat: TextEdit.PlainText
										KeyNavigation.backtab: editLockReasonInput.visible ? editLockReasonInput : editLockedCB
										
										onCursorRectangleChanged: {
											var cy = cursorRectangle.y
											var ch = cursorRectangle.height
											if (cy < commentInputFlick.contentY) {
												commentInputFlick.contentY = cy
											} else if (cy + ch > commentInputFlick.contentY + commentInputFlick.height) {
												commentInputFlick.contentY = cy + ch - commentInputFlick.height
											}
										}
										
										Text {
											anchors.fill: parent
											text: qsTr("Write a comment...")
											color: Style.inactiveTextColor
											font.pixelSize: Style.fontSizeM
											visible: commentInputField.text.length === 0
											verticalAlignment: Text.AlignVCenter
										}
									}
								}
								
								ToolButton {
									id: attachButton
									anchors.right: parent.right
									anchors.rightMargin: 6
									anchors.bottom: parent.bottom
									anchors.bottomMargin: Math.max(0, (40 - height) / 2)
									iconSource: Style.getIconPath("Icons/Attachment", Icon.State.On, Icon.Mode.Normal)
									decorator: Component {
										ToolButtonDecorator { color: "transparent" }
									}
									onClicked: attachImageDialog.open()
								}
							}
							
							Rectangle {
								id: sendBtnRect
								width: sendBtnText.contentWidth + 28
								height: 40
								radius: 20
								anchors.bottom: parent.bottom
								color: commentButton.enabled
									   ? (sendBtnMa.pressed ? Qt.darker(editView.accentColor, 1.15)
															: sendBtnMa.containsMouse ? Qt.lighter(editView.accentColor, 1.1)
																					  : editView.accentColor)
									   : "#D0D5DD"
								
								Text {
									id: sendBtnText
									anchors.centerIn: parent
									text: root.uploadsInProgress > 0 ? qsTr("Uploading...") : qsTr("Send")
									font.pixelSize: Style.fontSizeM
									color: commentButton.enabled ? Style.baseColor : "#98A2B3"
								}
								
								// Hidden functional Button for enabled state
								Button {
									id: commentButton
									visible: false
									enabled: (commentInputField.text !== "" || root.pendingAttachments.length > 0) && root.uploadsInProgress === 0
								}
								
								MouseArea {
									id: sendBtnMa
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: commentButton.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
									onClicked: {
										if (!commentButton.enabled) return
										root.addComment(commentInputField.text.trim(), root.pendingAttachments.slice())
										commentInputField.text = ""
										root.pendingAttachments = []
										root._replyToMessage = null
									}
								}
							}
						}
					}
					
					// File dialog
					FileDialog {
						id: attachImageDialog
						title: qsTr("Attach image")
						fileMode: FileDialog.OpenFile
						nameFilters: [qsTr("Image files") + " (*.png *.jpg *.jpeg *.gif *.bmp *.svg *.webp)"]
						
						onAccepted: {
							if (Qt.platform.os === "web") {
								var fileObj = attachImageDialog.file
								var fileName = fileObj.name || "attachment"
								var previewReader = new FileReader()
								previewReader.readAsDataURL(fileObj)
								previewReader.onload = function() {
									root.uploadAttachment(fileObj, fileName, previewReader.result)
								}
								previewReader.onerror = function() {
									console.warn("Failed to generate preview for: " + fileName)
									root.uploadAttachment(fileObj, fileName, "")
								}
							} else {
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
				
				// Lock notice / read-only notice
				Row {
					id: lockNoticeRow
					visible: (root.ticketData && root.ticketData.m_locked) || (!root.isNewIssue && !root.canEdit)
					anchors.bottom: parent.bottom
					anchors.bottomMargin: Style.paddingM
					anchors.left: parent.left
					anchors.leftMargin: editView.cardPadding
					width: parent.width - editView.cardPadding * 2
					spacing: Style.paddingS

					Image {
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeS
						height: width
						source: Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
						sourceSize.width: width
						sourceSize.height: height
					}
					
					Text {
						text: root.canEdit
							  ? qsTr("This conversation has been locked. Only collaborators can comment.")
							  : qsTr("You have read-only access to this ticket. Only the reporter, assignees, and administrators can edit.")
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						wrapMode: Text.Wrap
						width: parent.width - Style.fontSizeM - Style.paddingS
					}
				}
			}
			
			DropShadow {
				anchors.fill: commentsPanel
				z: commentsPanel.z - 1
				horizontalOffset: 3
				verticalOffset: 3
				radius: Style.radiusL
				spread: 0
				color: Style.shadowColor
				source: commentsPanel
				visible: commentsPanel.visible
			}
			
		} // panelsContainer
	}
}
