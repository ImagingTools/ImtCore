// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
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
import imtdeskTicketCollectionDocumentServiceSdl 1.0
import imtchatgui 1.0
import Qt.labs.platform 1.1 as QLP

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
	
	// Permission flags derived from server-supplied accessLevel
	// FullAccess: admin/reporter — can edit everything
	// LimitedAccess: assignee — can change status, comment, add context
	// CommentOnly: same-group viewer — can view and comment only
	// ViewOnly: pure read-only
	readonly property string accessLevel: {
		if (root._removedFromAssignees) return "ViewOnly"
		return isNewIssue ? "FullAccess" : (ticketData ? (ticketData.m_accessLevel || "ViewOnly") : "ViewOnly")
	}
	readonly property bool canEdit: accessLevel === "FullAccess" || accessLevel === "LimitedAccess"
	readonly property bool canLock: accessLevel === "FullAccess"
	readonly property bool canComment: accessLevel !== "ViewOnly"
	readonly property bool canEditCoreTicketFields: isNewIssue || accessLevel === "FullAccess"
	
	// Pending image attachments for comment being composed
	// Each element: {id: "uuid.ext", preview: "localPreviewUrl"}
	property var pendingAttachments: []
	// Number of uploads currently in progress
	property int uploadsInProgress: 0
	// Pending entity references for the ticket
	// Each element: {entityType, entityId, displayName, entityLinkPath, typeId}
	property var pendingEntityRefs: []
	// Track whether entity refs changed to avoid unnecessary emplace calls
	property bool _entityRefsChanged: false
	// Title inline edit mode (display vs edit)
	property bool _titleEditing: root.isNewIssue
	// Guard flag: set briefly when entering title edit so the activeFocus blur
	// handler does not immediately cancel edit mode while the focus is being
	// transferred to the input.
	property bool __titleEditOpening: false
	// Pending assignees for multi-select
	property var pendingAssignees: []
	property bool _assigneesChanged: false
	// User name cache: id → name (populated from FilterableSelectPopup and userNameResolver)
	property var __userNameCache: ({})
	// Reply-to message context (null = not replying)
	property var _replyToMessage: null
	// Highlighted message (temporarily set when scrolling to a reply-to source)
	property string _highlightedMessageId: ""
	// ID of message currently being edited inline ("" = none)
	property string _editingMessageId: ""
	// Chat feedback
	property string _chatActionHint: ""
	property bool _exportChatCopiedState: false
	readonly property int chatHintDurationMs: 2000
	readonly property int chatHintHeightPx: 28
	readonly property int minCommentInputHeightPx: Style.controlHeightM - 10
	
	// Track assignee removal: when user is removed from assignees, switch to readOnly
	property bool _removedFromAssignees: false
	
	signal commentSubmitted(string commentText)

	onTicketDataChanged: {
		descriptionAutoSaveTimer.stop()
		try {
			commentsFlick.scrollToBottom()
		}
		catch (err) {
			console.warn("TicketEditor: Failed to scroll to bottom after ticket refresh:", err)
		}
	}
	
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

	// Convert literal \n sequences to real newlines (for plain-text editing)
	function __unescapeNewlines(content) {
		if (!content) return ""
		return String(content).replace(/\\n/g, "\n")
	}

	// Convert real newlines back to literal \n sequences (for server storage)
	function __escapeNewlines(text) {
		if (!text) return ""
		return String(text).replace(/\n/g, "\\n")
	}

	function normalizeUserId(userId) {
		if (userId === undefined || userId === null)
			return ""
		return String(userId).trim().replace(/[{}]/g, "").toLowerCase()
	}

	function isSameUserId(leftId, rightId) {
		return normalizeUserId(leftId) === normalizeUserId(rightId)
	}

	function isImageAttachment(fileName) {
		if (!fileName) return false
		var lower = String(fileName).toLowerCase()
		return lower.endsWith(".png") || lower.endsWith(".jpg") || lower.endsWith(".jpeg")
				|| lower.endsWith(".gif") || lower.endsWith(".bmp") || lower.endsWith(".webp")
				|| lower.endsWith(".svg")
	}

	function copyTextToClipboard(text, successHint) {
		if (!text || String(text).length === 0) return
		clipboardProxy.text = String(text)
		clipboardProxy.selectAll()
		clipboardProxy.copy()
		clipboardProxy.select(0, 0)
		root._chatActionHint = successHint || qsTr("Copied to clipboard")
		chatHintTimer.restart()
	}

	function getCollectionCount(collection) {
		if (!collection) return 0
		if (collection.length !== undefined) return collection.length
		if (collection.count !== undefined) return collection.count
		return 0
	}

	function getCollectionItem(collection, index) {
		if (!collection) return null
		if (collection.length !== undefined) return collection[index]
		if (collection.get) {
			var wrapped = collection.get(index)
			return wrapped && wrapped.item ? wrapped.item : wrapped
		}
		return null
	}

	function formatChatExportText() {
		var tabSep = "\t"
		var lines = []
		lines.push(qsTr("Ticket Comments Export"))
		if (ticketData && ticketData.m_number) {
			lines.push(qsTr("Ticket #") + String(ticketData.m_number))
		}
		lines.push("")
		if (!ticketData || !ticketData.m_comments) {
			lines.push(qsTr("No comments"))
			return lines.join("\n")
		}

		for (var i = 0; i < ticketData.m_comments.count; i++) {
			var wrapper = ticketData.m_comments.get(i)
			var item = wrapper ? wrapper.item : null
			if (!item) continue

			var linePrefix = String(i + 1) + "."
			lines.push(linePrefix + tabSep + root.formatTimestamp(item.m_timestamp) + tabSep + (item.m_userName || qsTr("Unknown")))
			var contentText = String(item.m_content || "")
			var contentLines = contentText.replace(/\r\n/g, "\n").replace(/\r/g, "\n").split("\n")
			var isContentEmpty = contentLines.length === 0 || (contentLines.length === 1 && contentLines[0].length === 0)
			if (isContentEmpty) {
				lines.push(tabSep + qsTr("Message") + tabSep)
			} else {
				lines.push(tabSep + qsTr("Message") + tabSep + contentLines[0])
				for (var c = 1; c < contentLines.length; c++) {
					lines.push(tabSep + tabSep + contentLines[c])
				}
			}

			if (item.m_replyToId && item.m_replyToId.length > 0) {
				lines.push(tabSep + "↳" + tabSep + (item.m_replyToUserName || qsTr("Unknown")) + tabSep + (item.m_replyToContent || ""))
			}

			var attachments = item.m_attachments || []
			var attachmentCount = root.getCollectionCount(attachments)
			for (var a = 0; a < attachmentCount; a++) {
				var att = root.getCollectionItem(attachments, a)
				if (!att) continue
				lines.push(tabSep + qsTr("Attachment") + tabSep + (att.m_fileName || qsTr("file")) + tabSep + (att.m_preview || ""))
			}
			lines.push("")
		}

		return lines.join("\n")
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
		ticketData.modelChanged()
		
		// Assign a temporary local ID to the newly-added comment so that
		// subsequent doUpdateModel() calls (e.g. assignee change) do not
		// re-send it as a new message. The server skips comments that
		// already have an ID set.
		newItem.m_id = "local-" + Date.now().toString(36) + Math.random().toString(36).slice(2, 8)
		root.commentSubmitted(commentText)
	}

	// Edit an existing comment in-place. The comment is located by its
	// server-assigned id; the new content and an "edited" marker are written
	// to the in-memory model, and modelChanged() pushes the change to the
	// server which performs the actual edit via IChatService::EditMessage.
	function editComment(messageId, newContent) {
		if (!messageId || !ticketData || !ticketData.m_comments) return
		setBlockingUpdateModel(true)
		for (var i = 0; i < ticketData.m_comments.count; i++) {
			var item = ticketData.m_comments.get(i).item
			if (!item) continue
			if (String(item.m_id || "") === String(messageId)) {
				item.m_content = String(newContent || "")
				item.m_edited = true
				break
			}
		}
		setBlockingUpdateModel(false)
		ticketData.modelChanged()
	}

	// Mark an existing comment as deleted. The deleted flag is propagated to
	// the server via modelChanged() → updateDocumentFromRepresentation(),
	// where IChatService::DeleteMessage physically removes the message;
	// locally the message is hidden immediately so the UI feels responsive.
	function deleteComment(messageId) {
		if (!messageId || !ticketData || !ticketData.m_comments) return
		setBlockingUpdateModel(true)
		for (var i = 0; i < ticketData.m_comments.count; i++) {
			var item = ticketData.m_comments.get(i).item
			if (!item) continue
			if (String(item.m_id || "") === String(messageId)) {
				item.m_deleted = true
				break
			}
		}
		setBlockingUpdateModel(false)
		ticketData.modelChanged()
	}

	// Remove an attachment from an existing comment by index. The change
	// is local-only; the server update happens when the user clicks Save
	// in the inline editor, which calls editComment() → modelChanged().
	function removeCommentAttachment(messageId, attachmentIndex) {
		if (!messageId || !ticketData || !ticketData.m_comments) return
		setBlockingUpdateModel(true)
		for (var i = 0; i < ticketData.m_comments.count; i++) {
			var item = ticketData.m_comments.get(i).item
			if (!item) continue
			if (String(item.m_id || "") === String(messageId)) {
				if (item.m_attachments && attachmentIndex >= 0 && attachmentIndex < item.m_attachments.count) {
					item.m_attachments.removeElement(attachmentIndex)
					item.m_edited = true
				}
				break
			}
		}
		setBlockingUpdateModel(false)
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
		// Stop updating GUI if user was removed from assignees (readOnly, no new data)
		if (root._removedFromAssignees) return
		
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
		// First, check if current user was previously an assignee (for removal detection)
		var wasAssignee = false
		if (!root.isNewIssue) {
			for (var prev = 0; prev < root.pendingAssignees.length; prev++) {
				if (root.isSameUserId(root.pendingAssignees[prev].id, root.currentUserId)) {
					wasAssignee = true
					break
				}
			}
		}
		
		// Load assignees — resolve names from cache, trigger server resolution for uncached
		var assigns = []
		var aIds = ticketData.m_assigneeIds || []
		var hasUnresolved = false
		for (var a = 0; a < aIds.length; a++) {
			var aId = aIds[a]
			var cachedName = root.__userNameCache[aId]
			assigns.push({id: aId, name: cachedName || aId})
			if (!cachedName) hasUnresolved = true
		}
		root.pendingAssignees = assigns
		root._assigneesChanged = false
		
		// Trigger user name resolution for uncached IDs
		if (hasUnresolved && aIds.length > 0) {
			userNameResolver.fetch("")
		}
		
		// Detect assignee removal: if user was an assignee but no longer is, show notification and go readOnly
		if (wasAssignee) {
			var stillAssignee = false
			for (var sa = 0; sa < assigns.length; sa++) {
				if (root.isSameUserId(assigns[sa].id, root.currentUserId)) {
					stillAssignee = true
					break
				}
			}
			if (!stillAssignee) {
				root._removedFromAssignees = true
				PopupManager.addWarningMessage(
					qsTr("You have been removed from the assignees of this ticket. The view is now read-only."),
					true
				)
			}
		}
	}
	
	function updateModel() {
		ticketData.m_title = editTitleInput.text
		ticketData.m_description = editDescriptionInput.text
		ticketData.m_locked = editLockedCB.checkState === Qt.Checked
		ticketData.m_lockReason = editLockReasonInput.text
		
		ticketData.m_assigneeIds = root.pendingAssignees.map(function(a) { return a.id })
		
		if (root.isNewIssue) {
			ticketData.m_reporterId = AuthorizationController.getUserId()
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

	TextEdit {
		id: clipboardProxy
		width: 0
		height: 0
		visible: false
		readOnly: true
	}

	Timer {
		id: chatHintTimer
		interval: root.chatHintDurationMs
		repeat: false
		onTriggered: root._chatActionHint = ""
	}

	Timer {
		id: exportChatBtnStateTimer
		interval: 2000
		repeat: false
		onTriggered: root._exportChatCopiedState = false
	}
	
	// Standalone user name resolver — fetches Users collection to resolve assignee IDs to display names
	FilterableSelectGqlDataProvider {
		id: userNameResolver
		collectionId: "Users"
		multiSelect: true
		pageSize: 100
		
		onDataChanged: {
			root.__resolveAssigneeNamesFromResolver()
		}
	}
	
	function __resolveAssigneeNamesFromResolver() {
		var resolverItems = userNameResolver.items
		if (!resolverItems || resolverItems.length === 0) return
		
		var cacheUpdated = false
		for (var i = 0; i < resolverItems.length; i++) {
			var item = resolverItems[i]
			if (item.id && item.title && item.title !== "") {
				root.__userNameCache[item.id] = item.title
				cacheUpdated = true
			}
		}
		
		if (!cacheUpdated) return
		
		// Re-build pendingAssignees with resolved names
		var assigns = []
		for (var j = 0; j < root.pendingAssignees.length; j++) {
			var a = root.pendingAssignees[j]
			var cachedName = root.__userNameCache[a.id]
			assigns.push({
				id: a.id,
				name: cachedName || a.name || a.id
			})
		}
		root.pendingAssignees = assigns
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
		// Pre-faded variant of cardBorderColor used by inputs to render a subtle
		// (~40% opacity, alpha 0x66) border in their unfocused state.
		readonly property string cardBorderColorFaded: "#d0d0d266"
		readonly property string pageBgColor: Style.backgroundColor2
		readonly property string accentColor: "#5b8fd6"
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
		readonly property real avatarOverlap: -8
		readonly property int maxVisibleParticipants: 5
		readonly property real bubbleWidthRatio: 0.86
		readonly property real bubbleMaxWidth: 680
		readonly property real imageAttachmentWidth: 132
		readonly property real imageAttachmentHeight: 96
		readonly property real fileAttachmentMaxWidth: 240
		readonly property real fileAttachmentHeight: 44
		readonly property string focusedAttachmentBgColor: "#ECF3FF"
		readonly property string imageAttachmentBgColor: "#EEF2F8"
		readonly property string fileAttachmentBgColor: "#F6F8FC"
		readonly property string chatHintBgColor: editView.cardColor
		// Fixed widths for top groups (left = Title/Desc/Context and Messages, right = Properties)
		readonly property real detailsWidth: 700
		readonly property real propertiesWidth: 320

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
			
			// ==================== TOP ROW: Title/Desc + Properties (independent heights) ====================
			Item {
				id: topRow
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				// Height = only detailsCard (Title/Desc). Properties is independent and may extend below.
				// For new tickets: stack properties below details (single left-aligned group), so include both heights.
				height: root.isNewIssue ? (detailsCard.height + editView.columnGap + propertiesCard.height) : detailsCard.height
				
				// LEFT: Title + Description (Context moved to Properties card)
				Item {
					id: leftTopWrapper
					anchors.left: parent.left
					anchors.top: parent.top
					readonly property real _propW: Math.min(editView.propertiesWidth, Math.max(220, parent.width * 0.4))
					width: root.isNewIssue
						? Math.min(editView.detailsWidth, Math.max(280, parent.width))
						: Math.min(editView.detailsWidth, Math.max(280, parent.width - _propW - editView.columnGap))
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
									visible: root.canEditCoreTicketFields
									width: visible ? 28 : 0
									height: width
									radius: width / 2
									color: titleEditBtnMa.containsMouse ? "#F0F2F5" : "transparent"
									anchors.verticalCenter: parent.verticalCenter
									
									Image {
										anchors.centerIn: parent
										width: Style.iconSizeS
										height: width
										source: Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)
										sourceSize.width: width
										sourceSize.height: height
									}
									
									MouseArea {
										id: titleEditBtnMa
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											editTitleInput.oldText = editTitleInput.text
											root.__titleEditOpening = true
											root._titleEditing = true
											titleEditFocusTimer.start()
										}
									}

									Timer {
										id: titleEditFocusTimer
										interval: 1
										repeat: false
										onTriggered: {
											editTitleInput.forceActiveFocus()
											root.__titleEditOpening = false
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
								
								Column {
									id: titleColumn
									width: root.isNewIssue ? parent.width : parent.width - titleConfirmBtn.width - titleCloseBtn.width - 2*Style.spacingS
									spacing: 4

									Text {
										text: qsTr("Title")
										font.pixelSize: Style.fontSizeM
										font.bold: true
										color: editView.sectionLabelColor
										visible: root.isNewIssue
									}
									
									CustomTextField {
										id: editTitleInput
										objectName: "TitleInput"
										width: titleColumn.width
										height: Style.controlHeightM
										placeHolderText: qsTr("Enter ticket title...")
										readOnly: !root.canEditCoreTicketFields
										onActiveFocusChanged: {
											if (!activeFocus && root._titleEditing && !root.isNewIssue && !root.__titleEditOpening) {
												editTitleInput.text = root.ticketData ? root.ticketData.m_title : editTitleInput.oldText
												root._titleEditing = false
											}
										}
										KeyNavigation.tab: editDescriptionInput
										onAccepted: {
											root._titleEditing = false
											root.doUpdateModel()
										}
										onCancelled: {
											root._titleEditing = false
										}
										onEditingFinished: {
											if (root.isNewIssue){
												root.doUpdateModel()
											}
										}

										property string oldText
									}
								}

								Rectangle {
									id: titleCloseBtn
									visible: !root.isNewIssue
									width: visible ? 28 : 0
									height: width
									radius: width/2
									color: titleCloseBtnMa.containsMouse ? "#E6F4EA" : "#F0F2F5"
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
											editTitleInput.text = root.ticketData ? root.ticketData.m_title : editTitleInput.oldText
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
									// Test instrumentation: this bordered box (Flickable+raw TextEdit, not
									// the standard CustomTextField wrapper) had no objectName pair at all;
									// the container needs one so gui.fill()'s own container-then-nested-
									// TextInput lookup can resolve it (see editDescriptionInput's own
									// "TextInput" objectName below). Inert.
									objectName: "DescriptionInput"
									width: parent.width
									height: Math.min(220, Math.max(50, editDescriptionInput.contentHeight)) + Style.paddingM * 2
									radius: Style.radiusM
									border.color: editDescriptionInput.activeFocus ? editView.accentColor : editView.cardBorderColorFaded
									border.width: editDescriptionInput.activeFocus ? 2 : 1
									color: root.canEditCoreTicketFields ? "white" : Style.alternateBaseColor

									Image {
										z: 2
										anchors.top: parent.top
										anchors.right: parent.right
										anchors.topMargin: Style.paddingS
										anchors.rightMargin: Style.paddingS
										width: Style.iconSizeS
										height: width
										source: Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
										sourceSize.width: width
										sourceSize.height: height
										visible: !root.canEditCoreTicketFields
									}
									
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
										objectName: "TextInput"
											width: descriptionFlick.width
											height: Math.max(50, contentHeight)
											font.pixelSize: Style.fontSizeM
											color: Style.textColor
											wrapMode: TextEdit.Wrap
											textFormat: TextEdit.PlainText
											readOnly: !root.canEditCoreTicketFields
											onEditingFinished: root.doUpdateModel()
											KeyNavigation.tab: editTypeCB
											KeyNavigation.backtab: editTitleInput

											onTextChanged: {
												if (!root.guiIsBlocked() && text !== root.ticketData.m_description) {
													descriptionAutoSaveTimer.restart()
												}
											}

											Timer {
												id: descriptionAutoSaveTimer
												interval: 1000
												repeat: false
												onTriggered: {
													root.doUpdateModel()
												}
											}

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
				} // leftTopWrapper
				
				// RIGHT: Properties (top-right group, narrow, attached to leftTopWrapper.right)
				// For new tickets: stacked below leftTopWrapper at same width, left-aligned (single group).
				Item {
					id: rightTopWrapper
					anchors.left: root.isNewIssue ? parent.left : leftTopWrapper.right
					anchors.leftMargin: root.isNewIssue ? 0 : editView.columnGap
					anchors.top: root.isNewIssue ? leftTopWrapper.bottom : parent.top
					anchors.topMargin: root.isNewIssue ? editView.columnGap : 0
					width: root.isNewIssue
						? leftTopWrapper.width
						: Math.min(editView.propertiesWidth, Math.max(220, parent.width - leftTopWrapper.width - editView.columnGap))
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
							spacing: Style.spacingL
							
							Text {
								text: qsTr("Properties")
								font.pixelSize: Style.fontSizeL
								font.bold: true
								color: Style.textColor
							}
							
							// Row 1: Type then Priority (stacked vertically in narrow Properties card)
							Column {
								width: parent.width
								spacing: Style.spacingL
								
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
										objectName: "TicketTypeCombo"
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 1
										model: ticketTypeModel
										changeable: root.canEditCoreTicketFields
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
										objectName: "TicketPriorityCombo"
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 1
										model: priorityModel
										changeable: root.canEditCoreTicketFields
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editStatusCB
										KeyNavigation.backtab: editTypeCB
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
										objectName: "TicketStatusCombo"
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 0
										model: statusModel
										changeable: root.canEdit
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editLockedCB
										KeyNavigation.backtab: editPriorityCB
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
										objectName: "AddAssigneeButton"
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										visible: root.canEditCoreTicketFields
										text: "+ " + qsTr("Add assignee")
										font.pixelSize: Style.fontSizeM
										font.bold: true
										color: editView.accentColor

										MouseArea {
											objectName: "MouseArea"
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: {
												var known = []
												for (var j = 0; j < root.pendingAssignees.length; j++) {
													var assignee = root.pendingAssignees[j]
													known.push({ id: assignee.id, title: assignee.name || assignee.id })
												}
												var ids = []
												for (var i = 0; i < root.pendingAssignees.length; i++)
													ids.push(root.pendingAssignees[i].id)
												var point = addAssigneeBtn.mapToItem(null, 0, addAssigneeBtn.height)
												ModalDialogManager.openDialog(assigneeSelectComp, {
													"x": point.x,
													"y": point.y,
													"knownItems": known,
													"preselectedIds": ids
												})
											}
										}
									}
								}
								
								Flow {
									width: parent.width
									clip: true
									spacing: Style.spacingXS
									visible: root.pendingAssignees.length > 0
									
									Repeater {
										model: root.pendingAssignees
										delegate: Rectangle {
											objectName: "AssigneeChip_" + index
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
												anchors.right: assigneeChipRemove.visible ? assigneeChipRemove.left : parent.right
												anchors.rightMargin: 2
												anchors.verticalCenter: parent.verticalCenter
												text: modelData.name || modelData.id
												font.pixelSize: Style.fontSizeM
												color: editView.accentColor
												elide: Text.ElideRight
												maximumLineCount: 1
											}
											
											ToolButton {
												id: assigneeChipRemove
												objectName: "RemoveButton"
												visible: root.canEditCoreTicketFields
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
									text: qsTr("No assignees")
									font.pixelSize: Style.fontSizeM
									color: Style.inactiveTextColor
									wrapMode: Text.WordWrap
								}
								
								// Assignee picker Component — opened via ModalDialogManager
								Component {
									id: assigneeSelectComp
									
									FilterableSelectPopup {
										dataProvider: FilterableSelectGqlDataProvider {
											collectionId: "Users"
											multiSelect: true
										}

										itemWidth: 280
										showCheckBox: true
										showSelectedGroup: true
										filterPlaceholder: qsTr("Type or choose a user")
										
										onSelectionChanged: {
											var arr = []
											for (var i = 0; i < selectedIds.length; i++) {
												var selId = selectedIds[i]
												var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
												if (!selName)
													selName = selId
												// Cache resolved name
												if (selName !== selId) root.__userNameCache[selId] = selName
												arr.push({id: selId, name: selName})
											}
											root.pendingAssignees = arr
											root._assigneesChanged = true
										}
										
										Component.onDestruction: {
											if (root._assigneesChanged) {
												root.doUpdateModel()
											}
										}
									}
								}
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
							
							// ---------- Context / Entity References (in properties card) ----------
							Rectangle { width: parent.width; height: 1; color: editView.cardBorderColor; opacity: 0.4 }
							
							Column {
								id: contextCardCol
								width: parent.width
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
											color: editView.sectionLabelColor
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
										objectName: "AddContextButton"
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										visible: root.canEdit
										text: "+ " + qsTr("Add context")
										font.pixelSize: Style.fontSizeM
										font.bold: true
										color: editView.accentColor

										MouseArea {
											objectName: "MouseArea"
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: {
												var point = addContextBtn.mapToItem(null, 0, addContextBtn.height)
												ModalDialogManager.openDialog(contextPickerDialogComp, {
													"x": point.x,
													"y": point.y
												})
											}
										}
									}
								}
								
								// Entity chips (tags-style)
								Flow {
									width: parent.width
									clip: true
									spacing: Style.spacingXS
									visible: root.pendingEntityRefs.length > 0
									
									Repeater {
										model: root.pendingEntityRefs
										delegate: Rectangle {
											objectName: "ContextChip_" + index
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
												anchors.right: refRemoveBtn.visible ? refRemoveBtn.left : parent.right
												anchors.rightMargin: 2
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
												objectName: "RemoveButton"
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

								// Empty context placeholder
								Text {
									visible: root.pendingEntityRefs.length === 0
									width: parent.width
									text: qsTr("No linked entities")
									font.pixelSize: Style.fontSizeM
									color: Style.inactiveTextColor
									wrapMode: Text.WordWrap
								}
							
								// Context picker — ComboBox + per-entity FilterableSelectPopup via Loader
								Component {
									id: contextPickerDialogComp

									PopupView {
										id: ctxWrapper

										property string selectedEntityTypeId: ""
										property bool __ctxReady: false
										readonly property int __popupWidth: 320
										readonly property int __maxHeight: 600

										width: __popupWidth + 2 * Style.marginL
										height: Math.min(Style.marginL + ctxTypeCB.height + ctxPopupItem.height + 2*Style.marginL, __maxHeight)

										function started() {
											ctxWrapper.__ctxReady = false
											if (entityTypeModel.getItemsCount() > 0) {
												ctxWrapper.selectedEntityTypeId = entityTypeModel.getData("id", 0)
												ctxTypeCB.currentIndex = 0
											}
											ctxWrapper.__ctxReady = true
											// Trigger Loader (re)creation via sourceComponent swap
											if (ctxWrapper.selectedEntityTypeId !== "") {
												ctxPopupLoader.sourceComponent = null
												ctxPopupLoader.sourceComponent = ctxPopupComp
											}
										}

										function closePopup() {
											ModalDialogManager.closeDialog()
										}

										onSelectedEntityTypeIdChanged: {
											if (ctxWrapper.__ctxReady) {
												ctxPopupLoader.sourceComponent = null
												ctxPopupLoader.sourceComponent = ctxPopupComp
											}
										}

										// Unified background
										Rectangle {
											id: ctxBg
											anchors.fill: parent
											color: Style.baseColor
											radius: Style.radiusL
											border.width: 1
											border.color: Style.alternateBaseColor
										}

										MouseArea {
											anchors.fill: ctxBg
											onWheel: { wheel.accepted = true }
											onClicked: { mouse.accepted = true }
											onReleased: {}
											onPressAndHold: {}
											onPressed: {}
											onPositionChanged: {}
										}

										// Unified shadow
										DropShadow {
											anchors.fill: ctxBg
											z: ctxBg.z - 1
											horizontalOffset: 3
											verticalOffset: 3
											radius: Style.radiusL
											color: Style.shadowColor
											source: ctxBg
										}

										ComboBox {
											id: ctxTypeCB
											objectName: "ContextEntityTypeCombo"
											anchors.left: parent.left
											anchors.right: parent.right
											anchors.top: parent.top
											anchors.margins: Style.marginL
											width: parent.width
											height: Style.buttonHeightM
											model: entityTypeModel
											currentIndex: 0
											onCurrentIndexChanged: {
												if (!ctxWrapper.__ctxReady) return
												if (entityTypeModel.getItemsCount() > currentIndex && currentIndex >= 0) {
													var newTypeId = entityTypeModel.getData("id", currentIndex)
													if (ctxWrapper.selectedEntityTypeId !== newTypeId) {
														ctxWrapper.selectedEntityTypeId = newTypeId
													}
												}
											}
										}

										// Per-entity FilterableSelectPopup (recreated on entity type change)
										Component {
											id: ctxPopupComp

											FilterableSelectPopup {
												id: ctxInnerPopup

												embedded: true

												dataProvider: FilterableSelectGqlDataProvider {
													collectionId: ctxWrapper.selectedEntityTypeId
													multiSelect: true
												}

												itemWidth: ctxWrapper.__popupWidth
												showCheckBox: true
												showSelectedGroup: true
												filterPlaceholder: qsTr("Search entities...")

												onRequestClose: ctxWrapper.closePopup()

												onSelectionChanged: {
													var otherRefs = []
													for (var j = 0; j < root.pendingEntityRefs.length; j++) {
														if (root.pendingEntityRefs[j].entityType !== ctxWrapper.selectedEntityTypeId) {
															otherRefs.push(root.pendingEntityRefs[j])
														}
													}
													for (var i = 0; i < selectedIds.length; i++) {
														var selId = selectedIds[i]
														var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
														if (!selName)
															selName = selId
														var selTypeId = dataProvider ? dataProvider.getSelectedItemTypeId(selId) : ""
														var linkPath = ctxWrapper.selectedEntityTypeId
																+ (selTypeId ? "/" + selTypeId : "")
																+ "/" + selId
														otherRefs.push({
															entityType: ctxWrapper.selectedEntityTypeId,
															entityId: selId,
															displayName: selName,
															entityLinkPath: linkPath,
															typeId: selTypeId
														})
													}
													root.pendingEntityRefs = otherRefs
													root._entityRefsChanged = true
													root.doUpdateModel()
												}
											}
										}

										Item {
											id: ctxPopupItem
											anchors.left: parent.left
											anchors.leftMargin: Style.marginL
											anchors.right: parent.right
											anchors.rightMargin: Style.marginL
											anchors.top: ctxTypeCB.bottom
											anchors.topMargin: Style.marginL

											Loader {
												id: ctxPopupLoader
												width: parent.width
												onLoaded: {
													var preIds = []
													var knownItms = []
													for (var i = 0; i < root.pendingEntityRefs.length; i++) {
														var ref = root.pendingEntityRefs[i]
														if (ref.entityType === ctxWrapper.selectedEntityTypeId) {
															preIds.push(ref.entityId)
															knownItms.push({
																id: ref.entityId,
																title: ref.displayName || ref.entityId
															})
														}
													}
													ctxPopupItemConnections.target = item
													ctxPopupItem.height = item.height
													item.knownItems = knownItms
													item.preselectedIds = preIds
													item.started()
												}
											}
											Connections {
												id:ctxPopupItemConnections
												function onHeightChanged(){
													ctxPopupItem.height = ctxPopupLoader.item.height
												}
											}
										}

										Shortcut {
											sequence: "Escape"
											onActivated: ctxWrapper.closePopup()
										}
									}
								}
							}
							// Lock section (existing tickets only, reporter/admin only)
							Column {
								visible: !root.isNewIssue && root.canLock
								width: parent.width
								spacing: Style.spacingS
								
								Rectangle { width: parent.width; height: 1; color: editView.cardBorderColor; opacity: 0.4 }
								
								CheckBox {
									id: editLockedCB
									objectName: "LockIssueCheckBox"
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
										objectName: "LockReasonInput"
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
			
			// ==================== BOTTOM: Chat (same width as Title/Desc, below topRow) ====================
			Rectangle {
				id: commentsPanel
				visible: !root.isNewIssue
				anchors.top: topRow.bottom
				anchors.topMargin: editView.columnGap
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				width: leftTopWrapper.width
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
					}

					Row {
						anchors.right: parent.right
						anchors.rightMargin: editView.cardPadding
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.spacingL
						
						Row {
							id: participantsRow
							anchors.verticalCenter: parent.verticalCenter
							spacing: editView.avatarOverlap
							
							Repeater {
								model: root.chatParticipants().slice(0, editView.maxVisibleParticipants)
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
						
						ToolButton {
							id: exportChatBtn
							anchors.verticalCenter: parent.verticalCenter
							enabled: !root._exportChatCopiedState
							iconSource: root._exportChatCopiedState
										? Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
										: Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
							decorator: Component {
								ToolButtonDecorator {
									color: "transparent"
									icon.width: Style.iconSizeM
								}
							}
							onClicked: {
								root.copyTextToClipboard(root.formatChatExportText(), qsTr("Chat export copied"))
								root._exportChatCopiedState = true
								exportChatBtnStateTimer.restart()
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

				Rectangle {
					visible: root._chatActionHint.length > 0
					anchors.top: chatHeader.bottom
					anchors.topMargin: Style.spacingS
					anchors.horizontalCenter: parent.horizontalCenter
					radius: Style.radiusL
					height: root.chatHintHeightPx
					width: chatHintText.contentWidth + Style.paddingM * 2
					color: editView.chatHintBgColor
					border.color: editView.cardBorderColor
					border.width: 1
					opacity: 0.97
					z: 3

					Text {
						id: chatHintText
						anchors.centerIn: parent
						text: root._chatActionHint
						font.pixelSize: Style.fontSizeM - 1
						color: Style.textColor
					}
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
								root._highlightedMessageId = msgId
								highlightClearTimer.restart()
								return
							}
						}
					}

					Timer {
						id: highlightClearTimer
						interval: 1500
						onTriggered: root._highlightedMessageId = ""
					}

					onContentHeightChanged: {
						scrollToBottom()
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
									height: visible ? (commentBubbleCol.height + topGap) : 0
									visible: !(model.item.m_deleted === true)
									
									readonly property bool isMe: root.isSameUserId(model.item.m_userId, root.currentUserId)
									readonly property var dataModel: model.item
									readonly property string _prevUserId: index > 0 && commentsThread.itemAt(index - 1) ? commentsThread.itemAt(index - 1).dataModel.m_userId : ""
									readonly property bool isGroupedWithPrev: index > 0
																			  && _prevUserId.length > 0
																			  && root.isSameUserId(_prevUserId, model.item.m_userId)
									readonly property int topGap: index === 0 ? 0 : (isGroupedWithPrev ? 2 : Style.spacingS)
									readonly property bool isEditingThis: root._editingMessageId.length > 0
																		  && String(model.item.m_id || "") === root._editingMessageId
									readonly property bool canEditOrDelete: isMe
																			&& root.canComment
																			&& String(model.item.m_id || "").length > 0
																			&& !(root.ticketData && root.ticketData.m_locked)
									
									Column {
										id: commentBubbleCol
										y: commentDelegate.topGap
										width: parent.width
										spacing: 4
										
										// Chat bubble (contains avatar + name + timestamp + content)
										Rectangle {
											id: chatBubbleRect
											readonly property real maxBubbleWidth: Math.min(parent.width * editView.bubbleWidthRatio, editView.bubbleMaxWidth)
											readonly property bool isHighlighted: root._highlightedMessageId.length > 0 && commentDelegate.dataModel.m_id === root._highlightedMessageId
											width: maxBubbleWidth
											anchors.right: commentDelegate.isMe ? parent.right : undefined
											anchors.left: commentDelegate.isMe ? undefined : parent.left
											height: bubbleContent.height + Style.paddingM * 2
											radius: 12
											color: commentDelegate.isMe ? editView.myBubbleColor : editView.otherBubbleColor
											border.width: isHighlighted ? 2 : 0
											border.color: editView.accentColor

											// Highlight overlay when scrolling to a reply-to source
											Rectangle {
												anchors.fill: parent
												radius: parent.radius
												color: editView.accentColor
												opacity: chatBubbleRect.isHighlighted ? 0.15 : 0.0
											}
											
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
														width: bubbleContent.width - editView.avatarSize - Style.spacingS
														clip: true
														spacing: 2
														
														Text {
															width: parent.width
															text: commentDelegate.isMe ? qsTr("You") : (model.item.m_userName || qsTr("Unknown"))
															font.pixelSize: Style.fontSizeM
															font.bold: true
															color: Style.textColor
															elide: Text.ElideRight
														}
														
														Row {
															spacing: Style.spacingXS
															Text {
																text: root.formatTimestamp(model.item.m_timestamp)
																font.pixelSize: Style.fontSizeM - 1
																color: editView.timestampColor
															}
															Text {
																visible: model.item.m_edited === true
																text: "· " + qsTr("edited")
																font.pixelSize: Style.fontSizeM - 1
																font.italic: true
																color: editView.timestampColor
															}
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
													visible: String(model.item.m_content || "").length > 0 && !commentDelegate.isEditingThis
													lineHeight: 1.45
												}

												// Inline edit field (replaces the body when editing this message)
												Column {
													visible: commentDelegate.isEditingThis
													width: parent.width
													spacing: Style.spacingXS

													Rectangle {
														width: parent.width
														height: Math.max(40, editMessageInput.contentHeight + Style.paddingS * 2)
														radius: Style.radiusM
														color: "white"
														border.color: editView.accentColor
														border.width: 1

														TextEdit {
															id: editMessageInput
															anchors.fill: parent
															anchors.margins: Style.paddingS
															font.pixelSize: Style.fontSizeM
															color: Style.textColor
															wrapMode: TextEdit.Wrap
															textFormat: TextEdit.PlainText
															text: root.__unescapeNewlines(model.item.m_content)
															onVisibleChanged: {
																if (visible) {
																	text = root.__unescapeNewlines(model.item.m_content)
																	editMessageFocusTimer.start()
																}
															}
															onActiveFocusChanged: {
																if (!activeFocus && commentDelegate.isEditingThis) {
																	editCancelOnBlurTimer.start()
																}
															}

															Timer {
																id: editMessageFocusTimer
																interval: 1
																repeat: false
																onTriggered: editMessageInput.forceActiveFocus()
															}

															// Cancel edit when focus leaves; short delay lets
															// Save/Cancel click handlers fire first.
															Timer {
																id: editCancelOnBlurTimer
																interval: 50
																repeat: false
																onTriggered: {
																	if (root._editingMessageId.length > 0) {
																		root._editingMessageId = ""
																	}
																}
															}
														}
													}

													Row {
														spacing: Style.spacingS
														Text {
															text: qsTr("Save")
															font.pixelSize: Style.fontSizeM
															color: editView.accentColor
															font.bold: true
															MouseArea {
																anchors.fill: parent
																hoverEnabled: true
																cursorShape: Qt.PointingHandCursor
																onClicked: {
																	editCancelOnBlurTimer.stop()
																	var newText = root.__escapeNewlines(editMessageInput.text)
																	root._editingMessageId = ""
																	root.editComment(model.item.m_id, newText)
																}
															}
														}
														Text {
															text: qsTr("Cancel")
															font.pixelSize: Style.fontSizeM
															color: Style.inactiveTextColor
															MouseArea {
																anchors.fill: parent
																hoverEnabled: true
																cursorShape: Qt.PointingHandCursor
																onClicked: {
																	editCancelOnBlurTimer.stop()
																	root._editingMessageId = ""
																}
															}
														}
													}
												}
												
												Flow {
													width: parent.width
													spacing: Style.spacingS
													visible: commentDelegate.dataModel.m_attachments.count > 0

													Repeater {
														model: commentDelegate.dataModel.m_attachments || []
														delegate: Row {
															id: attachmentDelegate
															spacing: Style.spacingS
															readonly property string attachmentUrl: model.item.m_preview || ""

															Text {
																font.pixelSize: Style.fontSizeM
																font.underline: attachmentDelegate.attachmentUrl.length > 0
																elide: Text.ElideRight
																color: attachmentDelegate.attachmentUrl.length > 0 ? editView.accentColor : Style.textColor
																text: model.item.m_fileName || qsTr("file")

																MouseArea {
																	anchors.fill: parent
																	enabled:attachmentDelegate.attachmentUrl.length > 0
																	hoverEnabled: enabled
																	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
																	onClicked: Qt.openUrlExternally(attachmentDelegate.attachmentUrl)
																}
															}

															Text {
																visible: commentDelegate.isEditingThis
																text: "\u2715"
																font.pixelSize: Style.fontSizeM
																color: Style.inactiveTextColor

																MouseArea {
																	anchors.fill: parent
																	hoverEnabled: true
																	cursorShape: Qt.PointingHandCursor
																	onClicked: root.removeCommentAttachment(commentDelegate.dataModel.m_id, index)
																}
															}
														}
													}
												}
												
												Row {
													spacing: Style.spacingM
													visible: !commentDelegate.isEditingThis

													Text {
														visible: root.canComment
														text: qsTr("Reply")
														font.pixelSize: Style.fontSizeM
														color: Style.inactiveTextColor

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

													Text {
														text: qsTr("Copy")
														font.pixelSize: Style.fontSizeM
														color: Style.inactiveTextColor

														MouseArea {
															anchors.fill: parent
															hoverEnabled: true
															cursorShape: Qt.PointingHandCursor
															onClicked: root.copyTextToClipboard(model.item.m_content || "", qsTr("Message copied"))
														}
													}

													Text {
														visible: commentDelegate.canEditOrDelete
														text: qsTr("Edit")
														font.pixelSize: Style.fontSizeM
														color: Style.inactiveTextColor

														MouseArea {
															anchors.fill: parent
															hoverEnabled: true
															cursorShape: Qt.PointingHandCursor
															onClicked: root._editingMessageId = String(model.item.m_id || "")
														}
													}

													Text {
														visible: commentDelegate.canEditOrDelete
														text: qsTr("Delete")
														font.pixelSize: Style.fontSizeM
														color: Style.inactiveTextColor

														MouseArea {
															anchors.fill: parent
															hoverEnabled: true
															cursorShape: Qt.PointingHandCursor
															onClicked: root.deleteComment(model.item.m_id)
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
					visible: root.canComment && (!root.ticketData || !root.ticketData.m_locked)
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
								objectName: "CommentInput"
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
										objectName: "TextInput"
										width: commentInputFlick.width
										height: Math.max(contentHeight, root.minCommentInputHeightPx)
										y: Math.max(0, (commentInputFlick.height - height) / 2)
										font.pixelSize: Style.fontSizeM
										color: Style.textColor
										wrapMode: TextEdit.Wrap
										textFormat: TextEdit.PlainText
										KeyNavigation.backtab: editLockReasonInput.visible ? editLockReasonInput : editLockedCB
										
										onCursorRectangleChanged: {
											var cy = cursorRectangle.y + y
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
									objectName: "AttachButton"
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
								objectName: "SendCommentButton"
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
									enabled: commentInputField.text.trim().length > 0 && root.uploadsInProgress === 0
								}
								
								MouseArea {
									id: sendBtnMa
									objectName: "MouseArea"
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
					QLP.FileDialog {
						id: attachImageDialog
						title: qsTr("Attach image")
						fileMode: QLP.FileDialog.OpenFile
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
					visible: (root.ticketData && root.ticketData.m_locked) || (!root.isNewIssue && !root.canComment)
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
						text: root.canComment
							  ? qsTr("This conversation has been locked. Only collaborators can comment.")
							  : qsTr("You have read-only access to this ticket. Only the reporter, assignees, and administrators can edit.")
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						wrapMode: Text.Wrap
						width: parent.width - Style.fontSizeM - Style.paddingS
					}
				}
			}

		} // panelsContainer
	}
}
