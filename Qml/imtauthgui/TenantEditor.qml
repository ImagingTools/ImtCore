// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtauthTenantsSdl 1.0
import imtauthTenantMembershipsSdl 1.0

DocumentViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property TenantData tenantData: model
	property var pendingMembers: []
	property var pendingInvitations: []
	property bool isNewTenant: container.tenantData ? (!container.tenantData.m_id || container.tenantData.m_id === "") : true
	// Guard: set when members are modified locally, prevents updateGui from overwriting
	property bool __membersModifiedLocally: false

	// Access: only Owner can edit, all others are readOnly (Leave button only)
	readonly property bool isOwner: container.tenantData && container.tenantData.m_ownerId && container.tenantData.m_currentUserId
		? container.tenantData.m_currentUserId === container.tenantData.m_ownerId
		: false
	readonly property bool __isReadOnly: !container.isNewTenant && !container.isOwner

	// Trigger UI updates when members/invitations data changes
	onPendingMembersChanged: {
		// UI is directly bound to pendingMembers array
	}

	onPendingInvitationsChanged: {
		// UI is directly bound to pendingInvitations array
	}

	function updateGui(){
		container.__loadMembersFromModel()
		container.__loadInvitationsFromModel()
		if (multiPageView.getPageByIndex(0))
			multiPageView.getPageByIndex(0).updateGui()
		// Update permissions page if loaded
		var permissionsPage = multiPageView.getPageById("Permissions")
		if (permissionsPage)
			permissionsPage.updateGui()
	}

	function updateModel(){
		if (multiPageView.getPageByIndex(0))
			multiPageView.getPageByIndex(0).updateModel()
		// Sync members and memberRoles back to model
		if (container.tenantData) {
			if (!container.tenantData.hasMembers()) {
				container.tenantData.emplaceMembers()
			}
			container.tenantData.m_members.clear()
			var pendingMembers = container.pendingMembers
			for (var memberIndex = 0; memberIndex < pendingMembers.length; memberIndex++) {
				var memberEntry = container.tenantData.createMembersArrayElement()
				if (memberEntry) {
					memberEntry.m_id = pendingMembers[memberIndex].id
					memberEntry.m_name = pendingMembers[memberIndex].name || pendingMembers[memberIndex].id
					container.tenantData.m_members.addElement(memberEntry)
				}
			}

			// tenantPermissions: sync from permissions page
			var permissionsPage = multiPageView.getPageById("Permissions")
			if (permissionsPage)
				permissionsPage.updateModel()
		}
	}

	function __loadMembersFromModel() {
		if (container.__membersModifiedLocally) {
			container.__membersModifiedLocally = false
			return
		}
		if (!container.tenantData)
			return
		var serverMembers = container.tenantData.m_members
		var members = []
		if (serverMembers) {
			var count = serverMembers.count || 0
			for (var i = 0; i < count; i++) {
				var m = serverMembers.get(i).item
				if (m) {
					members.push({ id: m.m_id || "", name: m.m_name || m.m_id || "" })
				}
			}
		}
		container.pendingMembers = members
	}

	function __loadInvitationsFromModel() {
		if (!container.tenantData)
			return
		var invitationsModel = container.tenantData.m_pendingInvitations
		var invitations = []
		if (invitationsModel) {
			var count = invitationsModel.count || 0
			for (var i = 0; i < count; i++) {
				var invitation = invitationsModel.get(i).item
				if (invitation) {
					invitations.push({
						id: invitation.m_id || "",
						userId: invitation.m_userId || "",
						userName: invitation.m_userName || invitation.m_userId || "",
						status: invitation.m_status || "Pending",
						invitedByUserId: invitation.m_invitedByUserId || "",
						invitedByName: invitation.m_invitedByName || invitation.m_invitedByUserId || "",
						createdAt: invitation.m_createdAt || "",
						expiresAt: invitation.m_expiresAt || ""
					})
				}
			}
		}
		container.pendingInvitations = invitations
	}

	function __inviteSelectedUsers(selectedItems) {
		var activeIds = {}
		for (var i = 0; i < container.pendingMembers.length; i++)
			activeIds[container.pendingMembers[i].id] = true
		for (var j = 0; j < container.pendingInvitations.length; j++)
			activeIds[container.pendingInvitations[j].userId] = true
		for (var k = 0; k < selectedItems.length; k++) {
			var selected = selectedItems[k]
			if (selected && selected.id && !activeIds[selected.id]) {
				container.createInvitationInput.m_tenantId = container.tenantData.m_id || ""
				container.createInvitationInput.m_userId = selected.id
				container.createInvitationInput.m_role = "Member"
				container.createInvitationSender.send(container.createInvitationInput)
			}
		}
	}

	function __removePendingInvitation(invitationId) {
		var invitations = []
		for (var i = 0; i < container.pendingInvitations.length; i++) {
			if (container.pendingInvitations[i].id !== invitationId)
				invitations.push(container.pendingInvitations[i])
		}
		container.pendingInvitations = invitations
	}

	function __formatDateTime(value) {
		if (!value)
			return ""
		var date = new Date(value)
		if (isNaN(date.getTime()))
			return value
		return Qt.formatDateTime(date, Qt.DefaultLocaleShortDate)
	}

	function __displayNameOrId(name, id) {
		return name ? name : id
	}

	function __formatInvitationInfo(invitedByName, expiresAt) {
		var byPart = invitedByName ? qsTr("by %1").arg(invitedByName) : ""
		var expPart = expiresAt ? qsTr("expires %1").arg(container.__formatDateTime(expiresAt)) : ""
		return byPart && expPart ? byPart + " · " + expPart : byPart + expPart
	}

	function __isInvitationExpired(expiresAt) {
		if (!expiresAt)
			return false
		var expDate = new Date(expiresAt)
		if (isNaN(expDate.getTime()))
			return false
		return expDate.getTime() < Date.now()
	}

	// --- MultiPageView ---
	MultiPageView {
		id: multiPageView

		anchors.fill: parent

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			if (!container.isNewTenant) {
				multiPageView.addPage("Members", qsTr("Members"), membersPageComp, "Icons/MultipleUser")
				if (container.isOwner) {
					multiPageView.addPage("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Role")
				}
			}
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	onIsNewTenantChanged: {
		multiPageView.updatePages()
		if (!container.isNewTenant) {
			container.__loadMembersFromModel()
			container.__loadInvitationsFromModel()
		}
	}

	onIsOwnerChanged: {
		if (!container.isNewTenant)
			multiPageView.updatePages()
	}

	// ===== Page: General =====
	Component {
		id: generalPageComp

		Item {
			id: generalPage

			function updateGui() {
				if (!container.tenantData) return
				nameInput.text = container.tenantData.m_name || ""
				descriptionInput.text = container.tenantData.m_description || ""
				isActiveInput.checked = container.tenantData.m_isActive !== undefined ? container.tenantData.m_isActive : true
			}

			function updateModel() {
				if (!container.tenantData) return
				container.tenantData.m_name = nameInput.text
				container.tenantData.m_description = descriptionInput.text
				container.tenantData.m_isActive = isActiveInput.checked
			}

			CustomScrollbar {
				id: generalScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: generalFlickable.top
				anchors.bottom: generalFlickable.bottom
				secondSize: Style.marginM
				targetItem: generalFlickable
			}

			Flickable {
				id: generalFlickable
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: generalScrollbar.left
				anchors.rightMargin: Style.marginXL
				contentWidth: generalColumn.width
				contentHeight: generalColumn.height + 2 * Style.marginXL

				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: generalColumn
					width: Style.sizeHintXXL
					spacing: Style.marginXL

					GroupElementView {
						id: generalGroup
						width: parent.width

						TextInputElementView {
							id: nameInput

							name: qsTr("Tenant Name")
							placeHolderText: qsTr("Enter the tenant name")
							readOnly: !container.isOwner && !container.isNewTenant

							onEditingFinished: {
								let oldText = container.tenantData ? container.tenantData.m_name : ""
								if (oldText !== nameInput.text){
									container.doUpdateModel()
								}
							}

							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: isActiveInput
						}

						TextInputElementView {
							id: descriptionInput

							name: qsTr("Description")
							placeHolderText: qsTr("Enter the description")
							readOnly: !container.isOwner && !container.isNewTenant

							onEditingFinished: {
								let oldText = container.tenantData ? container.tenantData.m_description : ""
								if (oldText !== descriptionInput.text){
									container.doUpdateModel()
								}
							}

							KeyNavigation.tab: isActiveInput
							KeyNavigation.backtab: nameInput
						}

						SwitchElementView {
							id: isActiveInput

							name: qsTr("Active")
							readOnly: !container.isOwner && !container.isNewTenant

							onCheckedChanged: {
								container.doUpdateModel()
							}
						}
					}
				}
			}
		}
	}

	function __transferOwnershipTo(newOwnerId, newOwnerName) {
		container.transferOwnershipInput.m_tenantId = container.tenantData.m_id || ""
		container.transferOwnershipInput.m_newOwnerId = newOwnerId
		container.transferOwnershipSender.send(container.transferOwnershipInput)
	}

	// ===== Page: Members =====
	Component {
		id: membersPageComp

		Item {
			id: membersPage

			function __removeMemberById(userId) {
				var arr = container.pendingMembers.slice()
				for (var i = 0; i < arr.length; i++) {
					if (arr[i].id === userId) {
						arr.splice(i, 1)
						break
					}
				}
				container.pendingMembers = arr
				container.__membersModifiedLocally = true
				container.doUpdateModel()
			}

			// --- Confirmation state properties ---
			property string __confirmRemoveUserId: ""
			property string __confirmRemoveUserName: ""
			property string __confirmTransferUserId: ""
			property string __confirmTransferUserName: ""
			property string __confirmRevokeInvitationId: ""
			property string __confirmRevokeUserName: ""

			CustomScrollbar {
				id: membersScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: membersFlickable.top
				anchors.bottom: membersFlickable.bottom
				secondSize: Style.marginM
				targetItem: membersFlickable
			}

			Flickable {
				id: membersFlickable
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: membersScrollbar.left
				anchors.rightMargin: Style.marginXL
				contentWidth: membersColumn.width
				contentHeight: membersColumn.height + 2 * Style.marginXL

				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: membersColumn
					width: Style.sizeHintXXL
					spacing: Style.marginXL

					// ===== Header Area =====
					Row {
						id: membersHeader
						width: parent.width
						spacing: Style.marginM

						Column {
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginXS

							BaseText {
								text: qsTr("Members")
								font.pixelSize: Style.fontSizeXL
								font.bold: true
								color: Style.textColor
							}

							BaseText {
								text: container.pendingInvitations.length > 0
									? qsTr("%1 member(s) · %2 pending invite(s)").arg(container.pendingMembers.length).arg(container.pendingInvitations.length)
									: qsTr("%1 member(s)").arg(container.pendingMembers.length)
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}

						Item {
							width: parent.width
								- parent.children[0].width
								- (inviteMemberBtn.visible ? inviteMemberBtn.width : 0)
								- (leaveWorkspaceBtn.visible ? leaveWorkspaceBtn.width : 0)
								- parent.spacing * 2
							height: 1
						}

						Button {
							id: inviteMemberBtn
							visible: container.isOwner
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Invite Member")
							tooltipText: qsTr("Invite a new member to this workspace")
							onClicked: {
								ModalDialogManager.openDialog(inviteMemberDialogComp, {})
							}
						}

						Button {
							id: leaveWorkspaceBtn
							visible: !container.isOwner && container.tenantData && container.tenantData.m_currentUserId
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Leave Workspace")
							tooltipText: qsTr("Leave this workspace")
							onClicked: {
								ModalDialogManager.showConfirmationDialog(
									qsTr("Leave Workspace"),
									qsTr("Are you sure you want to leave this workspace? You will lose access to all workspace resources."),
									function(result) {
										if (result === true)
											membersPage.__removeMemberById(container.tenantData.m_currentUserId)
									}
								)
							}
						}
					}

					// ===== Active Members Section =====
					Column {
						id: activeMembersSection
						width: parent.width
						spacing: Style.marginS

						BaseText {
							text: qsTr("Active Members")
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
						}

						// Empty state
						BaseText {
							visible: container.pendingMembers.length === 0
							text: qsTr("No members found")
							font.pixelSize: Style.fontSizeM
							color: Style.inactiveTextColor
							topPadding: Style.marginM
							bottomPadding: Style.marginM
						}

						Repeater {
							model: container.pendingMembers

							delegate: Rectangle {
								id: activeMemberDelegate
								width: activeMembersSection.width
								height: activeMemberRow.implicitHeight + Style.marginM * 2
								radius: Style.radiusS
								color: activeMemberMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

								readonly property bool isMemberOwner: container.tenantData && modelData.id === container.tenantData.m_ownerId
								readonly property bool isCurrentUser: container.tenantData && container.tenantData.m_currentUserId && modelData.id === container.tenantData.m_currentUserId

								MouseArea {
									id: activeMemberMouseArea
									anchors.fill: parent
									hoverEnabled: true
									acceptedButtons: Qt.NoButton
								}

								Row {
									id: activeMemberRow
									anchors.left: parent.left
									anchors.right: parent.right
									anchors.verticalCenter: parent.verticalCenter
									anchors.margins: Style.marginM
									spacing: Style.marginM

									// Avatar placeholder
									Rectangle {
										id: activeMemberAvatar
										width: Style.iconSizeL
										height: Style.iconSizeL
										radius: width / 2
										anchors.verticalCenter: parent.verticalCenter
										color: activeMemberDelegate.isMemberOwner ? Style.selectedColor : Style.borderColor

										BaseText {
											anchors.centerIn: parent
											text: {
												var name = modelData.name || modelData.id || ""
												return name.length > 0 ? name.charAt(0).toUpperCase() : "?"
											}
											font.pixelSize: Style.fontSizeM
											font.bold: true
											color: Style.textColor
										}

										// Crown icon for owner
										BaseText {
											visible: activeMemberDelegate.isMemberOwner
											anchors.top: parent.top
											anchors.right: parent.right
											anchors.topMargin: -Style.marginXS
											anchors.rightMargin: -Style.marginXS
											text: "♛"
											font.pixelSize: Style.fontSizeS
											color: "#E3A008"
										}
									}

									// User info column
									Column {
										anchors.verticalCenter: parent.verticalCenter
										spacing: 2
										width: parent.width
											- activeMemberAvatar.width
											- activeMemberStatusBadge.width
											- activeMemberActions.width
											- parent.spacing * 3

										Row {
											spacing: Style.marginS

											BaseText {
												text: modelData.name || modelData.id || ""
												font.bold: activeMemberDelegate.isMemberOwner
												font.pixelSize: Style.fontSizeM
												color: Style.textColor
												elide: Text.ElideRight
											}

											// "You" indicator
											Rectangle {
												visible: activeMemberDelegate.isCurrentUser
												anchors.verticalCenter: parent.verticalCenter
												width: youText.implicitWidth + Style.marginS
												height: youText.implicitHeight + 2
												radius: Style.radiusS
												color: "#E8F0FE"

												BaseText {
													id: youText
													anchors.centerIn: parent
													text: qsTr("You")
													font.pixelSize: Style.fontSizeXS
													color: "#1967D2"
												}
											}
										}

										BaseText {
											text: modelData.id || ""
											font.pixelSize: Style.fontSizeS
											color: Style.inactiveTextColor
											elide: Text.ElideRight
											width: parent.width
										}
									}

									// Status / Role badge
									Column {
										id: activeMemberStatusBadge
										anchors.verticalCenter: parent.verticalCenter
										spacing: 2

										Rectangle {
											width: roleBadgeText.implicitWidth + Style.marginM
											height: roleBadgeText.implicitHeight + Style.marginXS
											radius: Style.radiusS
											color: activeMemberDelegate.isMemberOwner ? "#FEF3C7" : Style.baseColor
											border.color: activeMemberDelegate.isMemberOwner ? "#F59E0B" : Style.borderColor
											border.width: 1

											BaseText {
												id: roleBadgeText
												anchors.centerIn: parent
												text: activeMemberDelegate.isMemberOwner ? qsTr("Owner") : qsTr("Member")
												font.pixelSize: Style.fontSizeXS
												color: activeMemberDelegate.isMemberOwner ? "#92400E" : Style.textColor
											}
										}

										Rectangle {
											width: activeStatusText.implicitWidth + Style.marginS
											height: activeStatusText.implicitHeight + 2
											radius: Style.radiusS
											color: "#DCFCE7"

											BaseText {
												id: activeStatusText
												anchors.centerIn: parent
												text: qsTr("Active")
												font.pixelSize: Style.fontSizeXS
												color: "#166534"
											}
										}
									}

									// Actions column (fixed width to prevent jumping)
									Item {
										id: activeMemberActions
										width: Style.iconSizeL
										height: parent.height
										anchors.verticalCenter: parent.verticalCenter

										ToolButton {
											id: memberActionsBtn
											visible: {
												if (container.isOwner) {
													// Owner can act on everyone except remove self
													return true
												}
												// Regular member can only leave (self)
												return activeMemberDelegate.isCurrentUser
											}
											anchors.centerIn: parent
											tooltipText: qsTr("Actions")
											iconSource: Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
											decorator: Component {
												ToolButtonDecorator {
													color: "transparent"
													icon.width: Style.iconSizeXS
												}
											}
											onClicked: {
												var menuItems = []
												if (container.isOwner) {
													if (activeMemberDelegate.isMemberOwner) {
														// Owner → Self: only Transfer Ownership
														menuItems.push({ text: qsTr("Transfer Ownership"), action: "transfer" })
													} else {
														// Owner → Regular Member
														menuItems.push({ text: qsTr("Remove Member"), action: "remove" })
														menuItems.push({ text: qsTr("Transfer Ownership"), action: "transfer" })
													}
												} else {
													if (activeMemberDelegate.isCurrentUser && !activeMemberDelegate.isMemberOwner) {
														menuItems.push({ text: qsTr("Leave Workspace"), action: "leave" })
													}
												}
												membersPage.__showActionsMenu(menuItems, modelData.id, modelData.name || modelData.id)
											}
										}
									}
								}
							}
						}
					}

					// ===== Pending Invitations Section =====
					Column {
						id: pendingInvitationsSection
						width: parent.width
						spacing: Style.marginS
						visible: container.isOwner || container.pendingInvitations.length > 0

						BaseText {
							text: qsTr("Pending Invitations")
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
							topPadding: Style.marginL
						}

						// Empty state
						BaseText {
							visible: container.pendingInvitations.length === 0
							text: qsTr("No pending invitations")
							font.pixelSize: Style.fontSizeM
							color: Style.inactiveTextColor
							topPadding: Style.marginM
							bottomPadding: Style.marginM
						}

						Repeater {
							model: container.pendingInvitations

							delegate: Rectangle {
								id: inviteDelegate
								width: pendingInvitationsSection.width
								height: inviteRow.implicitHeight + Style.marginM * 2
								radius: Style.radiusS
								color: inviteMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"
								border.color: Style.borderColor
								border.width: 1
								opacity: 0.85

								readonly property bool isExpired: container.__isInvitationExpired(modelData.expiresAt)
								readonly property string effectiveStatus: {
									if (inviteDelegate.isExpired) return "Expired"
									if (modelData.status === "revoked" || modelData.status === "Revoked") return "Revoked"
									return "Pending"
								}

								MouseArea {
									id: inviteMouseArea
									anchors.fill: parent
									hoverEnabled: true
									acceptedButtons: Qt.NoButton
								}

								Row {
									id: inviteRow
									anchors.left: parent.left
									anchors.right: parent.right
									anchors.verticalCenter: parent.verticalCenter
									anchors.margins: Style.marginM
									spacing: Style.marginM

									// Mail/invitation avatar placeholder (dashed)
									Rectangle {
										width: Style.iconSizeL
										height: Style.iconSizeL
										radius: width / 2
										anchors.verticalCenter: parent.verticalCenter
										color: "transparent"
										border.color: Style.inactiveTextColor
										border.width: 1

										BaseText {
											anchors.centerIn: parent
											text: "✉"
											font.pixelSize: Style.fontSizeM
											color: Style.inactiveTextColor
										}
									}

									// Invite info column
									Column {
										anchors.verticalCenter: parent.verticalCenter
										spacing: 2
										width: parent.width
											- Style.iconSizeL
											- inviteStatusBadge.width
											- inviteActions.width
											- parent.spacing * 3

										BaseText {
											text: modelData.userName || modelData.userId || ""
											font.pixelSize: Style.fontSizeM
											color: Style.inactiveTextColor
											elide: Text.ElideRight
											width: parent.width
										}

										BaseText {
											text: container.__formatInvitationInfo(modelData.invitedByName || modelData.invitedByUserId || "", modelData.expiresAt || "")
											font.pixelSize: Style.fontSizeS
											color: inviteDelegate.isExpired ? "#DA3633" : Style.inactiveTextColor
											elide: Text.ElideRight
											width: parent.width
										}
									}

									// Status badge
									Item {
										id: inviteStatusBadge
										width: inviteStatusRect.width
										height: inviteStatusRect.height
										anchors.verticalCenter: parent.verticalCenter

										Rectangle {
											id: inviteStatusRect
											width: inviteStatusText.implicitWidth + Style.marginM
											height: inviteStatusText.implicitHeight + Style.marginXS
											radius: Style.radiusS
											color: {
												if (inviteDelegate.effectiveStatus === "Expired") return "#FFDCE0"
												if (inviteDelegate.effectiveStatus === "Revoked") return "#F3E8FF"
												return "#FFF3CD"
											}

											BaseText {
												id: inviteStatusText
												anchors.centerIn: parent
												text: {
													if (inviteDelegate.effectiveStatus === "Expired") return qsTr("Expired")
													if (inviteDelegate.effectiveStatus === "Revoked") return qsTr("Revoked")
													return qsTr("Pending")
												}
												font.pixelSize: Style.fontSizeXS
												color: {
													if (inviteDelegate.effectiveStatus === "Expired") return "#DA3633"
													if (inviteDelegate.effectiveStatus === "Revoked") return "#7C3AED"
													return "#856404"
												}
											}
										}
									}

									// Actions column (fixed width)
									Item {
										id: inviteActions
										width: Style.iconSizeL
										height: parent.height
										anchors.verticalCenter: parent.verticalCenter

										ToolButton {
											visible: container.isOwner && inviteDelegate.effectiveStatus === "Pending"
											anchors.centerIn: parent
											tooltipText: qsTr("Actions")
											iconSource: Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
											decorator: Component {
												ToolButtonDecorator {
													color: "transparent"
													icon.width: Style.iconSizeXS
												}
											}
											onClicked: {
												var menuItems = [
													{ text: qsTr("Resend Invitation"), action: "resend" },
													{ text: qsTr("Revoke Invitation"), action: "revoke" }
												]
												membersPage.__showInviteActionsMenu(menuItems, modelData.id, modelData.userName || modelData.userId || "")
											}
										}
									}
								}
							}
						}
					}
				}
			}

			// --- Actions Menu handling ---
			function __showActionsMenu(menuItems, userId, userName) {
				if (menuItems.length === 0)
					return

				// Single action: execute directly with confirmation
				if (menuItems.length === 1) {
					membersPage.__executeAction(menuItems[0].action, userId, userName)
					return
				}

				// Multiple actions: show context menu via ModalDialogManager
				var actions = []
				for (var i = 0; i < menuItems.length; i++) {
					actions.push(menuItems[i])
				}
				membersPage.__pendingMenuUserId = userId
				membersPage.__pendingMenuUserName = userName
				membersPage.__pendingMenuActions = actions
				ModalDialogManager.openDialog(actionsPopupMenuComp, {
					"menuActions": actions,
					"targetUserId": userId,
					"targetUserName": userName
				})
			}

			property string __pendingMenuUserId: ""
			property string __pendingMenuUserName: ""
			property var __pendingMenuActions: []

			function __showInviteActionsMenu(menuItems, invitationId, userName) {
				if (menuItems.length === 1) {
					membersPage.__executeInviteAction(menuItems[0].action, invitationId, userName)
					return
				}
				membersPage.__pendingMenuUserId = invitationId
				membersPage.__pendingMenuUserName = userName
				ModalDialogManager.openDialog(inviteActionsPopupMenuComp, {
					"menuActions": menuItems,
					"targetInvitationId": invitationId,
					"targetUserName": userName
				})
			}

			function __executeAction(action, userId, userName) {
				if (action === "remove") {
					membersPage.__confirmRemoveUserId = userId
					membersPage.__confirmRemoveUserName = userName
					ModalDialogManager.showConfirmationDialog(
						qsTr("Remove Member"),
						qsTr("Are you sure you want to remove %1? They will lose access to this workspace.").arg(userName),
						function(result) {
							if (result === true)
								membersPage.__removeMemberById(membersPage.__confirmRemoveUserId)
						}
					)
				} else if (action === "transfer") {
					membersPage.__confirmTransferUserId = userId
					membersPage.__confirmTransferUserName = userName
					ModalDialogManager.showConfirmationDialog(
						qsTr("Transfer Ownership"),
						qsTr("Are you sure you want to transfer ownership to %1? You will become a regular member and %1 will become the new owner.").arg(userName),
						function(result) {
							if (result === true)
								container.__transferOwnershipTo(membersPage.__confirmTransferUserId, membersPage.__confirmTransferUserName)
						}
					)
				} else if (action === "leave") {
					ModalDialogManager.showConfirmationDialog(
						qsTr("Leave Workspace"),
						qsTr("Are you sure you want to leave this workspace? You will lose access to all workspace resources."),
						function(result) {
							if (result === true && container.tenantData)
								membersPage.__removeMemberById(container.tenantData.m_currentUserId)
						}
					)
				}
			}

			function __executeInviteAction(action, invitationId, userName) {
				if (action === "resend") {
					container.resendInvitationInput.m_invitationId = invitationId
					container.resendInvitationSender.send(container.resendInvitationInput)
				} else if (action === "revoke") {
					membersPage.__confirmRevokeInvitationId = invitationId
					membersPage.__confirmRevokeUserName = userName
					ModalDialogManager.showConfirmationDialog(
						qsTr("Revoke Invitation"),
						qsTr("Are you sure you want to revoke the invitation for %1? They will no longer be able to join this workspace.").arg(userName),
						function(result) {
							if (result === true) {
								container.revokeInvitationInput.m_invitationId = membersPage.__confirmRevokeInvitationId
								container.revokeInvitationSender.send(container.revokeInvitationInput)
								container.__removePendingInvitation(membersPage.__confirmRevokeInvitationId)
							}
						}
					)
				}
			}

			// --- Actions popup menu component for members ---
			Component {
				id: actionsPopupMenuComp

				Item {
					id: actionsPopup
					anchors.fill: parent

					property var menuActions: []
					property string targetUserId: ""
					property string targetUserName: ""

					signal finished(int buttonId)

					MouseArea {
						anchors.fill: parent
						onClicked: actionsPopup.finished(Enums.cancel)
					}

					Rectangle {
						anchors.centerIn: parent
						width: Style.sizeHintXS
						height: actionsMenuColumn.implicitHeight + Style.marginM * 2
						radius: Style.radiusM
						color: Style.baseColor
						border.color: Style.borderColor
						border.width: 1

						Column {
							id: actionsMenuColumn
							anchors.fill: parent
							anchors.margins: Style.marginS

							Repeater {
								model: actionsPopup.menuActions

								delegate: Rectangle {
									width: parent.width
									height: Style.buttonHeightM
									radius: Style.radiusS
									color: actionItemMouse.containsMouse ? Style.buttonHoverColor : "transparent"

									MouseArea {
										id: actionItemMouse
										anchors.fill: parent
										hoverEnabled: true
										onClicked: {
											actionsPopup.finished(Enums.yes)
											membersPage.__executeAction(modelData.action, actionsPopup.targetUserId, actionsPopup.targetUserName)
										}
									}

									BaseText {
										anchors.verticalCenter: parent.verticalCenter
										anchors.left: parent.left
										anchors.leftMargin: Style.marginM
										text: modelData.text
										font.pixelSize: Style.fontSizeM
										color: modelData.action === "remove" ? "#DA3633" : Style.textColor
									}
								}
							}
						}
					}
				}
			}

			// --- Actions popup menu component for invites ---
			Component {
				id: inviteActionsPopupMenuComp

				Item {
					id: inviteActionsPopup
					anchors.fill: parent

					property var menuActions: []
					property string targetInvitationId: ""
					property string targetUserName: ""

					signal finished(int buttonId)

					MouseArea {
						anchors.fill: parent
						onClicked: inviteActionsPopup.finished(Enums.cancel)
					}

					Rectangle {
						anchors.centerIn: parent
						width: Style.sizeHintXS
						height: inviteActionsMenuColumn.implicitHeight + Style.marginM * 2
						radius: Style.radiusM
						color: Style.baseColor
						border.color: Style.borderColor
						border.width: 1

						Column {
							id: inviteActionsMenuColumn
							anchors.fill: parent
							anchors.margins: Style.marginS

							Repeater {
								model: inviteActionsPopup.menuActions

								delegate: Rectangle {
									width: parent.width
									height: Style.buttonHeightM
									radius: Style.radiusS
									color: inviteActionItemMouse.containsMouse ? Style.buttonHoverColor : "transparent"

									MouseArea {
										id: inviteActionItemMouse
										anchors.fill: parent
										hoverEnabled: true
										onClicked: {
											inviteActionsPopup.finished(Enums.yes)
											membersPage.__executeInviteAction(modelData.action, inviteActionsPopup.targetInvitationId, inviteActionsPopup.targetUserName)
										}
									}

									BaseText {
										anchors.verticalCenter: parent.verticalCenter
										anchors.left: parent.left
										anchors.leftMargin: Style.marginM
										text: modelData.text
										font.pixelSize: Style.fontSizeM
										color: modelData.action === "revoke" ? "#DA3633" : Style.textColor
									}
								}
							}
						}
					}
				}
			}

			// --- Invite Member Dialog ---
			Component {
				id: inviteMemberDialogComp

				Item {
					id: inviteMemberDialog
					anchors.fill: parent

					signal finished(int buttonId)

					MouseArea {
						anchors.fill: parent
						onClicked: inviteMemberDialog.finished(Enums.cancel)
					}

					function __submitInvitation() {
						var email = inviteEmailInput.text.trim()
						if (email !== "") {
							container.__inviteSelectedUsers([{id: email}])
							inviteMemberDialog.finished(Enums.yes)
						}
					}

					Rectangle {
						anchors.centerIn: parent
						width: Style.sizeHintL
						height: inviteDialogColumn.implicitHeight + Style.marginXL * 2
						radius: Style.radiusM
						color: Style.baseColor
						border.color: Style.borderColor
						border.width: 1

						Column {
							id: inviteDialogColumn
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.top: parent.top
							anchors.margins: Style.marginXL
							spacing: Style.marginL

							BaseText {
								text: qsTr("Invite Member")
								font.pixelSize: Style.fontSizeXL
								font.bold: true
								color: Style.textColor
							}

							BaseText {
								text: qsTr("Enter the email or login of the user you want to invite:")
								font.pixelSize: Style.fontSizeM
								color: Style.inactiveTextColor
								wrapMode: Text.WordWrap
								width: parent.width
							}

							TextInput {
								id: inviteEmailInput
								width: parent.width
								font.pixelSize: Style.fontSizeM
								color: Style.textColor

								Rectangle {
									anchors.fill: parent
									anchors.margins: -Style.marginS
									z: -1
									radius: Style.radiusS
									color: "transparent"
									border.color: Style.borderColor
									border.width: 1
								}

								Keys.onReturnPressed: {
									inviteMemberDialog.__submitInvitation()
								}
							}

							Row {
								spacing: Style.marginM
								anchors.right: parent.right

								Button {
									text: qsTr("Cancel")
									onClicked: inviteMemberDialog.finished(Enums.cancel)
								}

								Button {
									text: qsTr("Send Invitation")
									enabled: inviteEmailInput.text.trim() !== ""
									onClicked: {
										inviteMemberDialog.__submitInvitation()
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// ===== Page: Permissions =====
	Component {
		id: permissionsPageComp

		Item {
			id: permissionsPage

			function updateGui() {
				if (!container.tenantData || !tenantPermissionsTreeView.treeView)
					return

				// Build tree directly from server model (no intermediate TreeItemModel)
				var allPermsModel = container.tenantData.m_allProductPermissions
				var nodes = TreeModelBuilder.fromListModel(
					allPermsModel,
					function(item, index) {
						var perm = item && item.item ? item.item : item
						return {
							key: perm ? (perm.m_id || "") : "",
							text: perm ? (perm.m_name || "") : "",
							checkable: true,
							expanded: true,
							data: { FeatureId: perm ? (perm.m_id || "") : "" }
						}
					},
					function(item, index) {
						var perm = item && item.item ? item.item : item
						var children = perm ? perm.m_children : null
						return (children && (children.count || 0) > 0) ? children : null
					}
				)
				tenantPermissionsTreeView.treeView.model = nodes

				// Restore checked state from selected permissions (leaf nodes only)
				var selectedPermissionsIds = []
				var permissionsArray = container.tenantData.m_tenantPermissions
				if (permissionsArray) {
					var permCount = permissionsArray.length || 0
					for (var pi2 = 0; pi2 < permCount; pi2++) {
						selectedPermissionsIds.push(permissionsArray[pi2])
					}
				}

				tenantPermissionsTreeView.treeView.uncheckAll()

				var allNodesList = tenantPermissionsTreeView.treeView.allNodes()
				for (var i = 0; i < allNodesList.length; i++) {
					var nodeObj = allNodesList[i]
					var nodeChildren = nodeObj.children || []
					if (nodeChildren.length === 0) {
						var nodeData = nodeObj.data || {}
						var id = nodeData.FeatureId

						if (selectedPermissionsIds.includes(id)) {
							tenantPermissionsTreeView.treeView.checkItem(nodeObj.key)
						}
					}
				}
			}

			function updateModel() {
				if (!container.tenantData)
					return
				var selectedPermissionIds = []
				var checkedNodes = tenantPermissionsTreeView.treeView.getCheckedNodes()
				for (var j = 0; j < checkedNodes.length; j++) {
					var nodeObj = checkedNodes[j]
					var nodeChildren = nodeObj.children || []
					if (nodeChildren.length === 0) {
						var nodeData = nodeObj.data || {}
						var id = nodeData.FeatureId
						selectedPermissionIds.push(id)
					}
				}

				selectedPermissionIds.sort()

				if (!container.tenantData.hasTenantPermissions())
					container.tenantData.emplaceTenantPermissions()
				container.tenantData.m_tenantPermissions = []
				for (var k = 0; k < selectedPermissionIds.length; k++) {
					container.tenantData.m_tenantPermissions.push(selectedPermissionIds[k])
				}
			}

			CustomScrollbar {
				id: permissionsScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: permissionsFlickable.top
				anchors.bottom: permissionsFlickable.bottom
				secondSize: Style.marginM
				targetItem: permissionsFlickable
			}

			Flickable {
				id: permissionsFlickable
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: permissionsScrollbar.left
				anchors.rightMargin: Style.marginXL
				contentWidth: permissionsColumn.width
				contentHeight: permissionsColumn.height + 2 * Style.marginXL

				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: permissionsColumn
					width: Style.sizeHintXXL
					spacing: Style.marginXL

					GroupElementView {
						id: permissionsGroup
						width: parent.width

						TreeViewElementView {
							id: tenantPermissionsTreeView

							Connections {
								target: tenantPermissionsTreeView.treeView

								function onCheckedItemsChanged() {
									container.doUpdateModel()
								}
							}
						}
					}
				}
			}
		}
	}

	// --- GQL Request Senders ---
	property CreateTenantInvitationInput createInvitationInput: CreateTenantInvitationInput {}

	// --- Subscription for real-time membership notifications ---
	TenantMembershipSubscriptionClient {
		id: membershipSubscription

		onInvitationAccepted: {
			if (!container.tenantData || container.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				// Member accepted -> refresh the editor to show updated members
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}

		onInvitationRejected:{
			if (!container.tenantData || container.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				// Member rejected -> remove the pending invitation locally and refresh
				container.__removePendingInvitation(notification.membershipId)
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}

		onOwnershipTransferred: {
			if (!container.tenantData || container.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				// Ownership transferred -> refresh the editor to reflect new owner
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
	}
	property GqlSdlRequestSender createInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_createTenantInvitation

		sdlObjectComp: Component {
			CreateTenantInvitationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "")
						ModalDialogManager.showInfoDialog(m_errorMessage)
					// Refresh to show the new pending invitation
					else if (container.representationController)
						container.representationController.updateRepresentationFromDocument()
				}
			}
		}
	}

	property RevokeTenantInvitationInput revokeInvitationInput: RevokeTenantInvitationInput {}
	property GqlSdlRequestSender revokeInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_revokeTenantInvitation
	}

	property ResendTenantInvitationInput resendInvitationInput: ResendTenantInvitationInput {}
	property GqlSdlRequestSender resendInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_resendTenantInvitation
	}

	property TransferTenantOwnershipInput transferOwnershipInput: TransferTenantOwnershipInput {}
	property GqlSdlRequestSender transferOwnershipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_transferTenantOwnership

		sdlObjectComp: Component {
			TransferTenantOwnershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "")
						ModalDialogManager.showInfoDialog(m_errorMessage)
					else if (container.representationController)
						container.representationController.updateRepresentationFromDocument()
				}
			}
		}
	}
}
