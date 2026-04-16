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
	contentColor: Style.baseColor

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
	// Modern card-based layout: LEFT (details + metadata) + RIGHT (chat)
	// Organized into 3 groups in a 2-column structure
	// ================================================================

	Item {
		id: editView
		anchors.fill: parent

		// Layout constants
		readonly property real contentMaxWidth: 800
		readonly property real cardPadding: 20
		readonly property real cardRadius: Style.radiusL
		readonly property string cardColor: Style.baseColor
		readonly property string cardBorderColor: Style.borderColor
		readonly property string pageBgColor: Style.backgroundColor2
		readonly property string accentColor: Style.imaginToolsAccentColor
		readonly property string labelColor: Style.textColor
		readonly property real avatarSize: 34
		readonly property real accentDividerHeight: 2
		readonly property real badgeHeight: 22
		readonly property string accentBgLight: "#DFECF9"
		readonly property string accentBorderLight: "#B4D3F2"
		readonly property string accentBadgeBg: "#E5F0FB"

		// Page background
		Rectangle {
			anchors.fill: parent
			color: editView.pageBgColor
		}

		// ==================== LEFT COLUMN ====================
		Item {
			id: leftColumn
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			width: root.isNewIssue ? parent.width - Style.marginXL * 2 : parent.width * 0.45 - Style.marginXL

			CustomScrollbar {
				id: editScrollV
				z: parent.z + 1
				anchors.right: parent.right
				anchors.rightMargin: Style.spacingS
				anchors.top: editFlick.top
				anchors.bottom: editFlick.bottom
				secondSize: Style.marginM
				targetItem: editFlick
				visible: editView.visible
			}

			Flickable {
				id: editFlick
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: editScrollV.left
				anchors.rightMargin: Style.spacingS
				contentHeight: leftStack.height + Style.spacingL
				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: leftStack
					width: Math.min(editFlick.width, editView.contentMaxWidth)
					anchors.horizontalCenter: parent.horizontalCenter
					spacing: Style.spacingL

					// ========================================
					// CARD 1 — Ticket Details
					// ========================================
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
							spacing: Style.spacingL

							// Header row: ticket number + badges
							Row {
								width: parent.width
								spacing: Style.spacingS

								Text {
									text: root.isNewIssue ? qsTr("New Ticket") : "#" + (root.ticketData ? root.ticketData.m_number : "")
									font.pixelSize: Style.fontSizeXL
									font.bold: true
									color: editView.accentColor
									anchors.verticalCenter: parent.verticalCenter
								}

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

							// Accent divider
							Rectangle { width: parent.width; height: editView.accentDividerHeight; color: editView.accentColor; opacity: 0.15 }

							// ---------- Title ----------
							Column {
								width: parent.width
								spacing: Style.spacingXS

								Text {
									text: qsTr("TITLE")
									font.pixelSize: Style.fontSizeS
									font.bold: true
									color: editView.labelColor
									opacity: 0.6
								}

								CustomTextField {
									id: editTitleInput
									width: parent.width
									height: Style.controlHeightM
									placeHolderText: qsTr("Enter ticket title...")
									onEditingFinished: root.doUpdateModel()
									KeyNavigation.tab: editDescriptionInput
								}
							}

							// ---------- Description ----------
							Column {
								width: parent.width
								spacing: Style.spacingXS

								Text {
									text: qsTr("DESCRIPTION")
									font.pixelSize: Style.fontSizeS
									font.bold: true
									color: editView.labelColor
									opacity: 0.6
								}

								Rectangle {
									width: parent.width
									height: editDescriptionInput.height + Style.paddingM * 2
									radius: Style.radiusM
									border.color: editDescriptionInput.activeFocus ? editView.accentColor : editView.cardBorderColor
									border.width: editDescriptionInput.activeFocus ? 2 : 1
									color: editView.cardColor

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
										onEditingFinished: root.doUpdateModel()
										KeyNavigation.tab: editTypeCB
										KeyNavigation.backtab: editTitleInput

										Text {
											anchors.fill: parent
											text: qsTr("Describe the issue in detail...")
											color: Style.inactiveTextColor
											font.pixelSize: Style.fontSizeM
											visible: editDescriptionInput.text.length === 0
										}
									}
								}
							}

							// ---------- Context / Entity References ----------
							Column {
								width: parent.width
								spacing: Style.spacingS

								Row {
									width: parent.width
									spacing: Style.spacingS

									Text {
										text: qsTr("CONTEXT")
										font.pixelSize: Style.fontSizeS
										font.bold: true
										color: editView.labelColor
										opacity: 0.6
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
											font.pixelSize: Style.fontSizeS - 1
											font.bold: true
											color: Style.baseColor
										}
									}
								}

								// Entity chips
								Flow {
									width: parent.width
									spacing: Style.spacingS
									visible: root.pendingEntityRefs.length > 0

									Repeater {
										model: root.pendingEntityRefs
										delegate: Rectangle {
											readonly property real maxRefWidth: 300
											width: Math.min(refLabelText.contentWidth + refRemoveBtn.width + Style.paddingS * 3, maxRefWidth)
											height: 30
											radius: 15
											color: editView.accentBgLight
											border.color: editView.accentBorderLight
											border.width: 1

											Text {
												id: refLabelText
												anchors.left: parent.left
												anchors.leftMargin: Style.paddingS + 2
												anchors.verticalCenter: parent.verticalCenter
												text: (modelData.entityType ? "[" + modelData.entityType + "] " : "") + (modelData.displayName || modelData.entityId || "")
												font.pixelSize: Style.fontSizeS
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
													root.doUpdateModel()
												}
											}
										}
									}
								}

								// Entity type picker + Link button
								Row {
									width: parent.width
									spacing: Style.spacingS

									Column {
										width: Math.min(200, parent.width * 0.4)
										spacing: Style.spacingXS

										Text {
											text: qsTr("Entity Type")
											font.pixelSize: Style.fontSizeS
											color: editView.labelColor
											opacity: 0.6
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
										radius: Style.radiusL
										anchors.bottom: parent.bottom
										color: addRefBtnMa.pressed
											   ? Qt.darker(editView.accentColor, 1.15)
											   : addRefBtnMa.containsMouse
												 ? Qt.lighter(editView.accentColor, 1.1)
												 : editView.accentColor

										Text {
											id: addRefBtnContent
											anchors.centerIn: parent
											text: "🔗 " + qsTr("Link entity")
											font.pixelSize: Style.fontSizeM
											font.family: Style.fontFamily
											color: Style.baseColor
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

								// Dialog helpers
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
												var mdl = collectionView.table.elements
												let indexes = collectionView.table.getSelectedIndexes()
												for (var i = 0; i < indexes.length; i++) {
													let idx = indexes[i]
													var displayName = mdl.getData("name", idx)
													var typeId = mdl.getData("typeId", idx)
													var elementId = mdl.getData("id", idx)
													var linkPath = entityRefDialogLoader.entityTypeId
													if (typeId) linkPath += "/" + typeId
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
						}
					}

					// ========================================
					// CARD 2 — Properties / Metadata
					// ========================================
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
								text: qsTr("PROPERTIES")
								font.pixelSize: Style.fontSizeS
								font.bold: true
								color: editView.labelColor
								opacity: 0.6
							}

							Rectangle { width: parent.width; height: editView.accentDividerHeight; color: editView.accentColor; opacity: 0.15 }

							// Row 1: Type + Priority
							Row {
								width: parent.width
								spacing: Style.spacingL

								Column {
									width: (parent.width - Style.spacingL) / 2
									spacing: Style.spacingXS

									Text {
										text: qsTr("Type")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									ComboBox {
										id: editTypeCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 1
										model: ticketTypeModel
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editPriorityCB
										KeyNavigation.backtab: editDescriptionInput
									}
								}

								Column {
									width: (parent.width - Style.spacingL) / 2
									spacing: Style.spacingXS

									Text {
										text: qsTr("Priority")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									ComboBox {
										id: editPriorityCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 1
										model: priorityModel
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editAssigneeCB
										KeyNavigation.backtab: editTypeCB
									}
								}
							}

							// Row 2: Assignees + Reporter
							Row {
								width: parent.width
								spacing: Style.spacingL

								Column {
									width: editReporterCB.visible
										   ? (parent.width - Style.spacingL) / 2
										   : parent.width
									spacing: Style.spacingXS

									Text {
										text: qsTr("Assignees")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									ComboBox {
										id: editAssigneeCB
										width: parent.width
										height: Style.buttonHeightM
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editReporterCB.visible ? editReporterCB : editStatusCB
										KeyNavigation.backtab: editPriorityCB
									}
								}

								Column {
									visible: !root.isNewIssue
									width: (parent.width - Style.spacingL) / 2
									spacing: Style.spacingXS

									Text {
										text: qsTr("Reporter")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									ComboBox {
										id: editReporterCB
										width: parent.width
										height: Style.buttonHeightM
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editStatusCB
										KeyNavigation.backtab: editAssigneeCB
									}
								}
							}

							// Row 3: Status + State Reason (existing tickets only)
							Row {
								visible: !root.isNewIssue
								width: parent.width
								spacing: Style.spacingL

								Column {
									width: (parent.width - Style.spacingL) / 2
									spacing: Style.spacingXS

									Text {
										text: qsTr("Status")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									ComboBox {
										id: editStatusCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 0
										model: statusModel
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editStateReasonCB
										KeyNavigation.backtab: editReporterCB.visible ? editReporterCB : editAssigneeCB
									}
								}

								Column {
									width: (parent.width - Style.spacingL) / 2
									spacing: Style.spacingXS

									Text {
										text: qsTr("State Reason")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									ComboBox {
										id: editStateReasonCB
										width: parent.width
										height: Style.buttonHeightM
										currentIndex: 0
										model: stateReasonModel
										onCurrentIndexChanged: root.doUpdateModel()
										KeyNavigation.tab: editLockedCB
										KeyNavigation.backtab: editStatusCB
									}
								}
							}

							// Lock section (existing tickets only)
							Column {
								visible: !root.isNewIssue
								width: parent.width
								spacing: Style.spacingS

								Rectangle { width: parent.width; height: 1; color: editView.cardBorderColor; opacity: 0.5 }

								CheckBox {
									id: editLockedCB
									text: qsTr("Lock issue")
									onCheckStateChanged: root.doUpdateModel()
									KeyNavigation.tab: editLockedCB.checkState === Qt.Checked ? editLockReasonInput : commentInputField
									KeyNavigation.backtab: editStateReasonCB
								}

								Column {
									width: parent.width
									spacing: Style.spacingXS
									visible: editLockedCB.checkState === Qt.Checked

									Text {
										text: qsTr("Lock Reason")
										font.pixelSize: Style.fontSizeS
										color: editView.labelColor
										opacity: 0.6
									}

									CustomTextField {
										id: editLockReasonInput
										width: parent.width
										height: Style.controlHeightM
										placeHolderText: qsTr("Reason for locking")
										onEditingFinished: root.doUpdateModel()
										KeyNavigation.tab: commentInputField
										KeyNavigation.backtab: editLockedCB
									}
								}
							}
						}
					}
				}
			}
		}

		// ==================== RIGHT COLUMN — Chat ====================
		Rectangle {
			id: commentsPanel
			visible: !root.isNewIssue
			anchors.top: parent.top
			anchors.topMargin: Style.marginXL
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Style.marginXL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			anchors.left: leftColumn.right
			anchors.leftMargin: Style.spacingL
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
				height: 52
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
						color: editView.accentBadgeBg
						anchors.verticalCenter: parent.verticalCenter

						Text {
							id: chatCountLabel
							anchors.centerIn: parent
							text: commentsThread.count
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: editView.accentColor
						}
					}
				}

				// Header bottom border
				Rectangle {
					anchors.bottom: parent.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					height: 1
					color: editView.cardBorderColor
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

			Flickable {
				id: commentsFlick
				anchors.top: chatHeader.bottom
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

				onContentHeightChanged: {
					var maxY = contentHeight - height
					if (maxY > 0 && (contentY >= maxY - 50 || contentY <= 0))
						contentY = maxY
				}

				Column {
					id: commentsMainCol
					width: parent.width
					spacing: Style.spacingL

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
									width: parent.width * 0.82
									anchors.right: commentDelegate.isMe ? parent.right : undefined
									anchors.left: commentDelegate.isMe ? undefined : parent.left
									spacing: Style.spacingXS

									// Avatar + Name + Timestamp
									Row {
										spacing: Style.spacingS
										anchors.right: commentDelegate.isMe ? parent.right : undefined
										anchors.left: commentDelegate.isMe ? undefined : parent.left
										layoutDirection: commentDelegate.isMe ? Qt.RightToLeft : Qt.LeftToRight

										Rectangle {
											width: editView.avatarSize
											height: editView.avatarSize
											radius: editView.avatarSize / 2
											color: commentDelegate.isMe
												   ? editView.accentColor
												   : Style.borderColor

											Text {
												anchors.centerIn: parent
												text: model.item.m_userName ? model.item.m_userName.charAt(0).toUpperCase() : "?"
												font.pixelSize: Style.fontSizeS
												font.bold: true
												color: commentDelegate.isMe ? Style.baseColor : Style.textColor
												font.family: Style.fontFamily
											}
										}

										Column {
											anchors.verticalCenter: parent.verticalCenter
											spacing: 1

											Text {
												text: commentDelegate.isMe ? qsTr("You") : (model.item.m_userName || qsTr("Unknown"))
												font.pixelSize: Style.fontSizeM
												font.bold: true
												color: Style.textColor
											}

											Text {
												text: root.formatTimestamp(model.item.m_timestamp)
												font.pixelSize: Style.fontSizeS - 1
												color: Style.inactiveTextColor
											}
										}
									}

									// Chat bubble
									Rectangle {
										width: commentBubbleCol.width
										height: bubbleContent.height + Style.paddingM * 2
										radius: Style.radiusL
										color: commentDelegate.isMe
											   ? editView.accentBgLight
											   : editView.pageBgColor
										border.color: commentDelegate.isMe
											   ? editView.accentBorderLight
											   : editView.cardBorderColor
										border.width: 1

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
												lineHeight: 1.4
											}

											// Attachment cards
											Repeater {
												model: commentDelegate.dataModel.m_attachments || []
												delegate: Rectangle {
													width: attRow.width + Style.paddingM * 2
													height: attRow.height + Style.paddingS * 2
													radius: Style.radiusM
													color: Style.baseColor
													border.color: editView.cardBorderColor
													border.width: 1

													Row {
														id: attRow
														x: Style.paddingM
														anchors.verticalCenter: parent.verticalCenter
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
															text: model.item.m_fileName
															font.pixelSize: Style.fontSizeS
															color: Style.linkColor
															font.underline: true
														}
													}

													MouseArea {
														anchors.fill: parent
														hoverEnabled: true
														cursorShape: Qt.PointingHandCursor
														onClicked: Qt.openUrlExternally(model.item.m_preview)
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

			// ---- Fixed bottom: Add comment input ----
			Rectangle {
				id: addCommentSection
				visible: !root.ticketData || !root.ticketData.m_locked
				anchors.bottom: parent.bottom
				anchors.bottomMargin: 1
				anchors.left: parent.left
				anchors.leftMargin: 1
				anchors.right: parent.right
				anchors.rightMargin: 1
				height: addCommentInnerCol.height + editView.cardPadding * 2
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
					anchors.right: parent.right
					height: 1
					color: editView.cardBorderColor
				}

				Column {
					id: addCommentInnerCol
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					anchors.margins: editView.cardPadding
					spacing: Style.spacingS

					// Comment input with rounded border
					Rectangle {
						width: parent.width
						height: commentInputField.height + Style.paddingM * 2
						radius: Style.radiusL
						border.color: commentInputField.activeFocus ? editView.accentColor : editView.cardBorderColor
						border.width: commentInputField.activeFocus ? 2 : 1
						color: editView.cardColor

						TextEdit {
							id: commentInputField
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.top: parent.top
							anchors.margins: Style.paddingM
							height: 50
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
							wrapMode: TextEdit.Wrap
							clip: true
							KeyNavigation.backtab: editLockReasonInput.visible ? editLockReasonInput : editLockedCB

							Text {
								anchors.fill: parent
								text: qsTr("Write a comment...")
								color: Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
								visible: commentInputField.text.length === 0
							}
						}
					}

					// Pending attachments
					Flow {
						width: parent.width
						spacing: Style.spacingS
						visible: root.pendingAttachments.length > 0 || root.uploadsInProgress > 0

						Repeater {
							model: root.pendingAttachments
							delegate: Rectangle {
								readonly property real maxPillWidth: 220
								width: Math.min(pendingFileLabel.contentWidth + pendingRemoveBtn.width + Style.paddingM * 3, maxPillWidth)
								height: 28
								radius: 14
								border.color: editView.cardBorderColor
								border.width: 1
								color: editView.pageBgColor

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
							height: 28
							radius: 14
							border.color: editView.cardBorderColor
							border.width: 1
							color: editView.pageBgColor
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

					// Action row: attach + send
					Row {
						anchors.right: parent.right
						spacing: Style.spacingS

						ToolButton {
							id: attachButton
							tooltipText: qsTr("Attach file")
							iconSource: Style.getIconPath("Icons/Attachment", Icon.State.On, Icon.Mode.Normal)
							decorator: Component {
								ToolButtonDecorator { color: "transparent" }
							}
							onClicked: attachImageDialog.open()
						}

						Rectangle {
							width: sendBtnText.contentWidth + Style.marginXL * 2
							height: Style.buttonHeightM
							radius: Style.radiusL
							color: commentButton.enabled
								   ? (sendBtnMa.pressed ? Qt.darker(editView.accentColor, 1.15)
									  : sendBtnMa.containsMouse ? Qt.lighter(editView.accentColor, 1.1)
									  : editView.accentColor)
								   : Style.buttonColor

							Text {
								id: sendBtnText
								anchors.centerIn: parent
								text: qsTr("Send")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: commentButton.enabled ? Style.baseColor : Style.inactiveTextColor
							}

							// Hidden functional Button for enabled state
							Button {
								id: commentButton
								visible: false
								enabled: commentInputField.text !== ""
							}

							MouseArea {
								id: sendBtnMa
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: commentButton.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
								onClicked: {
									if (!commentButton.enabled || root.uploadsInProgress > 0) return
									root.addComment(commentInputField.text.trim(), root.pendingAttachments.slice())
									commentInputField.text = ""
									root.pendingAttachments = []
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

			// Lock notice
			Row {
				id: lockNoticeRow
				visible: root.ticketData && root.ticketData.m_locked
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.paddingM
				anchors.left: parent.left
				anchors.leftMargin: editView.cardPadding
				width: parent.width - editView.cardPadding * 2
				spacing: Style.paddingS

				Text {
					text: "🔒"
					font.pixelSize: Style.fontSizeM
				}

				Text {
					text: qsTr("This conversation has been locked. Only collaborators can comment.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
					wrapMode: Text.Wrap
					width: parent.width - Style.fontSizeM - Style.paddingS
				}
			}
		}
	}
}

