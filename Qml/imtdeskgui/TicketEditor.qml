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
	// Pending entity references for the ticket
	// Each element: {entityType, entityId, displayName, entityLinkPath, typeId}
	property var pendingEntityRefs: []
	
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

		// Save entity references to ticket data (always emplace so deletions are sent as empty array)
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
	// Ticket editor view — same layout for both new and existing tickets
	// Left: scrollable content (title, description, activity) with max-width centered
	// Right: fixed sidebar (assignees, type, priority, reporter, status, etc.)
	// Bottom: fixed "Add comment" input pinned below the scroll area
	// ================================================================
	
	Item {
		id: editView
		anchors.fill: parent
		
		// Maximum width for the left content area
		readonly property real contentMaxWidth: 800
		
		CustomScrollbar {
			id: editScrollV
			z: parent.z + 1
			anchors.right: root.isNewIssue ? parent.right : editSidebarSep.left
			anchors.rightMargin: root.isNewIssue ? Style.marginXL : Style.marginM
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
			anchors.bottomMargin: Style.marginM
			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			anchors.right: editScrollV.left
			anchors.rightMargin: Style.marginM
			contentHeight: editMainCol.height + Style.marginXL * 2
			boundsBehavior: Flickable.StopAtBounds
			clip: true
			
			Column {
				id: editMainCol
				width: Math.min(editFlick.width, editView.contentMaxWidth)
				anchors.horizontalCenter: parent.horizontalCenter
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
									  ? Style.imaginToolsAccentColor
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
				
				// --- Entity References section ---
				Column {
					width: parent.width
					spacing: Style.spacingS
					
					Row {
						width: parent.width
						spacing: Style.spacingS
						
						Text {
							text: qsTr("Context")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
							anchors.verticalCenter: parent.verticalCenter
						}
						
						Text {
							text: root.pendingEntityRefs.length > 0
								  ? "(" + root.pendingEntityRefs.length + ")"
								  : ""
							font.pixelSize: Style.fontSizeS
							color: Style.textColor
							anchors.verticalCenter: parent.verticalCenter
						}
					}
					
					// Existing entity references
					Flow {
						width: parent.width
						spacing: Style.spacingS
						visible: root.pendingEntityRefs.length > 0
						
						Repeater {
							model: root.pendingEntityRefs
							delegate: Rectangle {
								readonly property real maxRefWidth: 300
								width: Math.min(refLabelText.contentWidth + refRemoveBtn.width + 2*Style.paddingS, maxRefWidth)
								height: Style.buttonHeightS
								radius: Style.radiusS
								border.color: Style.borderColor
								border.width: 1
								color: Style.baseColor
								
								Text {
									id: refLabelText
									anchors.left: parent.left
									anchors.leftMargin: Style.paddingS
									anchors.verticalCenter: parent.verticalCenter
									text: (modelData.entityType ? "[" + modelData.entityType + "] " : "") + (modelData.displayName || modelData.entityId || "")
									font.pixelSize: Style.fontSizeS
									color: Style.imaginToolsAccentColor
									font.underline: modelData.entityLinkPath !== ""
									elide: Text.ElideRight
									maximumLineCount: 1
									
									MouseArea {
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											if (modelData.entityLinkPath){
												NavigationController.navigate(modelData.entityLinkPath)
											}
										}
									}
								}
								
								ToolButton {
									id: refRemoveBtn
									anchors.right: parent.right
									anchors.verticalCenter: parent.verticalCenter
									iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
									decorator: 	Component {
										ToolButtonDecorator {
											color: "transparent"
											icon.width: Style.iconSizeXS
										}
									}
									onClicked: {
										var arr = root.pendingEntityRefs.slice()
										console.log("arr1", arr)
										arr.splice(index, 1)
										
										root.pendingEntityRefs = arr
										console.log("arr2", arr)
										root.doUpdateModel()
									}
								}
							}
						}
					}
					
					// Entity type selection + "Link entity" button row
					Row {
						width: parent.width
						spacing: Style.spacingS
						
						Column {
							width: Math.min(200, parent.width * 0.4)
							spacing: Style.spacingXS
							
							Text {
								text: qsTr("Entity Type")
								font.pixelSize: Style.fontSizeS
								color: Style.textColor
							}
							
							ComboBox {
								id: refTypeCB
								width: parent.width
								height: Style.buttonHeightM
								model: entityTypeModel
							}
						}
						
						Rectangle {
							width: addRefBtnContent.contentWidth + Style.marginM * 2
							height: Style.buttonHeightM
							radius: Style.buttonRadius
							anchors.bottom: parent.bottom
							color: addRefBtnMa.pressed
								   ? Qt.darker(Style.borderColor, 1.1)
								   : addRefBtnMa.containsMouse
									 ? Qt.lighter(Style.borderColor, 1.1)
									 : Style.baseColor
							border.color: Style.borderColor
							border.width: 1
							
							Text {
								id: addRefBtnContent
								anchors.centerIn: parent
								text: "🔗 " + qsTr("Link entity")
								font.pixelSize: Style.fontSizeM
								font.family: Style.fontFamily
								color: Style.textColor
							}
							
							MouseArea {
								id: addRefBtnMa
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (!refTypeCB.model || refTypeCB.currentIndex < 0) return
									var typeId = refTypeCB.model.getData("id", refTypeCB.currentIndex)
									if (!typeId || typeId.length === 0) return
									entityRefDialogLoader.entityTypeId = typeId
									entityRefDialogLoader.entityTypeName = refTypeCB.model.getData("name", refTypeCB.currentIndex)
									ModalDialogManager.openDialog(entityRefDialogComp)
								}
							}
						}
					}
					
					// Entity reference collection dialog component
					QtObject {
						id: entityRefDialogLoader
						property string entityTypeId: ""
						property string entityTypeName: ""
					}
					
					Component {
						id: entityRefDialogComp
						RemoteCollectionViewDialog {
							id: entityRefDialog
							title: qsTr("Select") + " " + entityRefDialogLoader.entityTypeName
							collectionId: entityRefDialogLoader.entityTypeId
							
							Component.onCompleted: {
								addButton(Enums.apply, qsTr("Attach"), false)
								addButton(Enums.cancel, qsTr("Cancel"), true)
							}
							
							onCollectionViewChanged: {
								if (collectionView) {
									collectionView.selectionChanged.connect(function(ids) {
										entityRefDialog.setButtonEnabled(Enums.apply, ids.length > 0)
									})
								}
							}
							
							onFinished: {
								if (buttonId === Enums.apply && collectionView) {
									var arr = root.pendingEntityRefs.slice()
									var model = collectionView.table.elements
									let indexes = collectionView.table.getSelectedIndexes()
									for (var i = 0; i < indexes.length; i++) {
										let index = indexes[i]
										// Get display name and typeId from collection if available
										var displayName = model.getData("name", index)
										var typeId = model.getData("typeId", index)
										var elementId = model.getData("id", index)
	
										// Build path as collectionId/typeId/entityId
										var linkPath = entityRefDialogLoader.entityTypeId
										if (typeId) {
											linkPath += "/" + typeId
										}
										linkPath += "/" + elementId

										arr.push({
													 entityType: entityRefDialogLoader.entityTypeId,
													 entityId: elementId,
													 displayName: displayName,
													 entityLinkPath: linkPath,
													 typeId: typeId
												 })
									}
									root.pendingEntityRefs = arr
									root.doUpdateModel()
								}
							}
						}
					}
				}
				
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
		
		// Separator between details and comments panel
		Rectangle {
			id: editSidebarSep
			visible: !root.isNewIssue
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.right: commentsPanel.left
			anchors.rightMargin: Style.marginXL
			width: 1
			color: Style.borderColor
		}
		
		// Right: comments panel
		Item {
			id: commentsPanel
			visible: !root.isNewIssue
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			width: parent.width * 0.55
			
			CustomScrollbar {
				id: commentsScrollV
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: commentsFlick.top
				anchors.bottom: commentsFlick.bottom
				secondSize: Style.marginM
				targetItem: commentsFlick
				visible: commentsPanel.visible
			}
			
			Flickable {
				id: commentsFlick
				anchors.top: parent.top
				anchors.bottom: addCommentSection.visible ? addCommentSection.top
							: lockNoticeRow.visible ? lockNoticeRow.top
							: parent.bottom
				anchors.bottomMargin: Style.marginM
				anchors.left: parent.left
				anchors.right: commentsScrollV.left
				anchors.rightMargin: Style.marginM
				contentHeight: commentsMainCol.height + Style.marginXL * 2
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				
				Column {
					id: commentsMainCol
					width: parent.width
					spacing: Style.marginM
					
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
							color: Style.textColor
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
								height: commentBubbleCol.height
								
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
											color: Style.baseColor
											
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
											color: Style.textColor
											anchors.verticalCenter: parent.verticalCenter
										}
									}
									
									// Comment body bubble
									Rectangle {
										width: commentBubbleCol.width
										height: bubbleContent.height + Style.paddingM * 2
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
											Repeater {
												model: commentDelegate.dataModel.m_attachments || []
												delegate: Row {
													height: Style.controlHeightS
													spacing: Style.spacingS
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
														width: bubbleContent.width
														text: model.item.m_fileName
														textFormat: Text.StyledText
														font.pixelSize: Style.fontSizeS
														color: Style.linkColor
														wrapMode: Text.Wrap
														font.underline: true
													}

													MouseArea {
														anchors.fill: parent
														hoverEnabled: true
														cursorShape: Qt.PointingHandCursor
														onClicked: {
															Qt.openUrlExternally(model.item.m_preview)
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
			}
			
			// --- Fixed bottom: "Add comment" input ---
			Column {
				id: addCommentSection
				visible: !root.ticketData || !root.ticketData.m_locked
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: commentsScrollV.left
				anchors.rightMargin: Style.marginM
				spacing: Style.spacingS
				
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}
				
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
						color: Style.baseColor
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
										  ? Style.imaginToolsAccentColor
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
									width: Math.min(pendingFileLabel.contentWidth + pendingRemoveBtn.width + Style.paddingM * 3, maxPillWidth)
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
				
									ToolButton {
										id: pendingRemoveBtn
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
										decorator: 	Component {
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
											// Delete the uploaded file from the server
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
									color: Style.textColor
								}
							}
						}
						
						Row {
							anchors.right: parent.right
							spacing: Style.spacingS
				
							ToolButton {
								id: attachButton
								tooltipText: qsTr("Attach file")
								iconSource: Style.getIconPath("Icons/Attachment", Icon.State.On, Icon.Mode.Normal)
								decorator: 	Component {
									ToolButtonDecorator {
										color: "transparent"
									}
								}
				
								onClicked: {
									attachImageDialog.open()
								}
							}
							
							Button {
								id: commentButton
								text: qsTr("Comment")
								enabled: commentInputField.text !== ""
								decorator: 	Component {
									ButtonDecorator {
										color: commentButton.enabled ? Style.imaginToolsAccentColor : Style.buttonColor
										textColor: commentButton.enabled ? Style.baseColor : Style.inactiveTextColor
										opacity: commentButton.hovered ? 0.85 : 1
										border.width: 0
									}
								}
				
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
			
			// Lock notice (fixed at bottom when ticket is locked)
			Row {
				id: lockNoticeRow
				visible: root.ticketData && root.ticketData.m_locked
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				width: parent.width
				spacing: Style.paddingS
				
				Text {
					text: "🔒"
					font.pixelSize: Style.fontSizeM
				}
				
				Text {
					text: qsTr("This conversation has been locked. Only collaborators can comment.")
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
					wrapMode: Text.Wrap
					width: parent.width - Style.fontSizeM - Style.paddingS
				}
			}
		}
	}
}

