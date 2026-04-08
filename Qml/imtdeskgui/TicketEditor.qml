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

ViewBase {
	id: root

	anchors.fill: parent

	property TicketData ticketData: model
	property bool isNewIssue: ticketData ? (ticketData.m_number === 0) : true

	// Conversation messages — populated externally from the linked conversation
	property var commentMessages: []

	// User actions from ticket (via SDL activityItems, only Action type)
	property var ticketActions: ticketData ? (ticketData.m_activityItems || []) : []

	// Unified activity timeline: merge conversation messages + user actions, sorted by timestamp
	property var activityItems: {
		var items = []

		// Add conversation messages as Comment items
		var msgs = root.commentMessages || []
		for (var i = 0; i < msgs.length; i++) {
			var msg = msgs[i]
			items.push({
				itemType: "Comment",
				userId: msg.senderId || "",
				userName: msg.senderName || "",
				timestamp: msg.timestamp || "",
				content: msg.content || "",
				reactions: msg.reactions || []
			})
		}

		// Add user actions from ticket
		var actions = root.ticketActions || []
		for (var j = 0; j < actions.length; j++) {
			items.push(actions[j])
		}

		// Sort by timestamp (chronological)
		items.sort(function(a, b) {
			var ta = a.timestamp || ""
			var tb = b.timestamp || ""
			return ta < tb ? -1 : (ta > tb ? 1 : 0)
		})

		return items
	}

	signal commentSubmitted(string commentText)

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
	}

	function updateModel() {
		console.log("TicketEditor updateModel")
		if (root.isNewIssue) {
			ticketData.m_title = newTitleInput.text
			ticketData.m_description = newDescriptionInput.text
			ticketData.m_reporterId = AuthorizationController.getUserId()

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
		}
		else {
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
				ticketData.m_reporterId = ""
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

					// Status badges
					Row {
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

					// ── Activity / Conversation thread (GitHub Issues-like) ──
					Rectangle {
						width: parent.width
						height: 1
						color: Style.borderColor
					}

					Column {
						width: parent.width
						spacing: Style.spacingS

						Row {
							width: parent.width
							spacing: Style.spacingS

							Text {
								text: qsTr("Activity")
								font.pixelSize: Style.fontSizeL
								font.bold: true
								color: Style.textColor
								anchors.verticalCenter: parent.verticalCenter
							}

							Text {
								text: activityThread.count > 0
									? "(" + activityThread.count + ")"
									: ""
								font.pixelSize: Style.fontSizeS
								color: Style.textSecondaryColor
								anchors.verticalCenter: parent.verticalCenter
							}
						}

						// Unified activity timeline (comments + actions)
						Column {
							id: activityListCol
							width: parent.width
							spacing: Style.spacingS

							Repeater {
								id: activityThread
								model: root.activityItems

								// Each item = comment bubble OR action notice
								Column {
									width: activityListCol.width
									spacing: Style.spacingXS

									// --- Action item (e.g., "User closed ticket") ---
									Row {
										visible: modelData.itemType === "Action"
										width: parent.width
										spacing: Style.paddingS

										// Action icon circle
										Rectangle {
											width: 32
											height: 32
											radius: 16
											color: Style.textSecondaryColor

											Text {
												anchors.centerIn: parent
												text: "⚡"
												font.pixelSize: Style.fontSizeXS
											}
										}

										Text {
											width: parent.width - 32 - Style.paddingS
											text: {
												let who = modelData.userName || qsTr("Someone")
												let action = modelData.actionDescription || modelData.actionType || ""
												let when = modelData.timestamp || ""
												return who + " " + action + " " + when
											}
											font.pixelSize: Style.fontSizeS
											color: Style.textSecondaryColor
											wrapMode: Text.Wrap
											anchors.verticalCenter: parent.verticalCenter
										}
									}

									// --- Comment item ---
									Row {
										visible: modelData.itemType !== "Action"
										width: parent.width
										spacing: Style.paddingS

										// Avatar circle
										Rectangle {
											width: 32
											height: 32
											radius: 16
											color: Style.accentColor

											Text {
												anchors.centerIn: parent
												text: {
													let n = modelData.userName || ""
													return n.length > 0 ? n.charAt(0).toUpperCase() : "?"
												}
												font.pixelSize: Style.fontSizeXS
												color: "white"
												font.bold: true
											}
										}

										Column {
											width: parent.width - 32 - Style.paddingS
											spacing: Style.spacingXS

											// Header: sender name + timestamp
											Row {
												spacing: Style.paddingS

												Text {
													text: modelData.userName || qsTr("Unknown")
													font.pixelSize: Style.fontSizeS
													font.bold: true
													color: Style.textColor
												}

												Text {
													text: qsTr("commented") + " " + (modelData.timestamp || "")
													font.pixelSize: Style.fontSizeXS
													color: Style.textSecondaryColor
												}
											}

											// Comment body
											Rectangle {
												width: parent.width
												height: activityBodyText.height + Style.paddingM * 2
												radius: Style.radiusS
												color: Style.surfaceColor
												border.color: Style.borderColor
												border.width: 1

												Text {
													id: activityBodyText
													anchors.left: parent.left
													anchors.right: parent.right
													anchors.top: parent.top
													anchors.margins: Style.paddingM
													text: modelData.content || ""
													font.pixelSize: Style.fontSizeS
													color: Style.textColor
													wrapMode: Text.Wrap
												}
											}

											// Reactions row
											Row {
												spacing: Style.paddingXS
												visible: modelData.reactions ? modelData.reactions.length > 0 : false

												Repeater {
													model: modelData.reactions || []

													Rectangle {
														height: 22
														width: reactionEmojiText.width + Style.paddingS * 2
														radius: height / 2
														color: Style.surfaceColor
														border.color: Style.borderColor

														Text {
															id: reactionEmojiText
															anchors.centerIn: parent
															text: modelData
															font.pixelSize: Style.fontSizeXS
														}
													}
												}
											}
										}
									}
								}
							}
						}

						// "Write" input area (GitHub-like comment box)
						Rectangle {
							width: parent.width
							height: 1
							color: Style.borderColor
							visible: !ticketData || !ticketData.m_locked
						}

						Column {
							width: parent.width
							spacing: Style.spacingS
							visible: !ticketData || !ticketData.m_locked

							Text {
								text: qsTr("Add a comment")
								font.pixelSize: Style.fontSizeS
								font.bold: true
								color: Style.textColor
							}

							Row {
								width: parent.width
								spacing: Style.paddingS

								// Avatar for current user
								Rectangle {
									width: 32
									height: 32
									radius: 16
									color: Style.accentColor
									anchors.top: parent.top

									Text {
										anchors.centerIn: parent
										text: qsTr("You")
										font.pixelSize: Style.fontSizeXS
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
										color: Style.backgroundColor

										TextEdit {
											id: commentInputField
											anchors.left: parent.left
											anchors.right: parent.right
											anchors.top: parent.top
											anchors.margins: Style.paddingM
											height: 60
											font.pixelSize: Style.fontSizeS
											color: Style.textColor
											wrapMode: TextEdit.Wrap
											clip: true

											Text {
												anchors.fill: parent
												text: qsTr("Leave a comment")
												color: Style.textPlaceholderColor
												font.pixelSize: Style.fontSizeS
												visible: commentInputField.text.length === 0
											}
										}
									}

									Row {
										anchors.right: parent.right
										spacing: Style.spacingS

										Rectangle {
											width: commentBtnText.width + Style.paddingM * 2
											height: Style.buttonHeightM
											radius: Style.radiusS
											color: commentInputField.text.trim().length > 0
												? "#1a7f37" : Style.disabledColor

											Text {
												id: commentBtnText
												anchors.centerIn: parent
												text: qsTr("Comment")
												font.pixelSize: Style.fontSizeS
												font.bold: true
												color: "white"
											}

											MouseArea {
												anchors.fill: parent
												enabled: commentInputField.text.trim().length > 0
												onClicked: {
													root.commentSubmitted(commentInputField.text.trim())
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
							visible: ticketData && ticketData.m_locked
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

				// Separator
				Rectangle {
					width: 1
					height: editMainCol.height
					color: Style.borderColor
				}

				// Right: full sidebar
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

					Rectangle { width: parent.width; height: 1; color: Style.borderColor }

					// Reporter
					Column {
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
						spacing: Style.spacingS
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
							height: Style.controlHeightM
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

					// State Reason
					Column {
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
	}
}
