// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
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

	// Environment Roles: Creator > Owner > Admin > Member
	readonly property bool isCreator: container.tenantData && container.tenantData.m_creatorId && container.tenantData.m_currentUserId
		? container.tenantData.m_currentUserId === container.tenantData.m_creatorId
		: false
	readonly property bool isOwner: container.tenantData && container.tenantData.m_ownerId && container.tenantData.m_currentUserId
		? container.tenantData.m_currentUserId === container.tenantData.m_ownerId
		: false
	readonly property bool isAdmin: container.__getUserRole(container.tenantData ? container.tenantData.m_currentUserId : "") === "Admin"
	readonly property bool canManageMembers: container.isCreator || container.isOwner || container.isAdmin
	readonly property bool __isReadOnly: !container.isNewTenant && !container.canManageMembers

	function __getUserRole(userId) {
		if (!container.tenantData || !userId)
			return "Member"
		var roles = container.tenantData.m_memberRoles
		if (!roles) return "Member"
		var count = roles.count || roles.length || 0
		for (var i = 0; i < count; i++) {
			var entry = roles.get ? roles.get(i).item : roles[i]
			if (entry && entry.m_userId === userId)
				return entry.m_role || "Member"
		}
		return "Member"
	}


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
				if (container.canManageMembers) {
					multiPageView.addPage("Roles", qsTr("Roles"), rolesPageComp, "Icons/Role")
					multiPageView.addPage("Groups", qsTr("Groups"), groupsPageComp, "Icons/MultipleUser")
				}
				if (container.isCreator) {
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

	onIsCreatorChanged: {
		if (!container.isNewTenant)
			multiPageView.updatePages()
	}

	onCanManageMembersChanged: {
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

					// Header (matches Members page style)
					Column {
						width: parent.width
						spacing: Style.marginXS

						BaseText {
							text: qsTr("General")
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							text: qsTr("Basic workspace settings and configuration.")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}

					Rectangle {
						width: parent.width
						height: 1
						color: Style.borderColor
					}

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

						Text {
							id: inviteMemberBtn
							visible: container.canManageMembers
							anchors.verticalCenter: parent.verticalCenter
							text: "+ " + qsTr("Create invitation")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.linkColor

							MouseArea {
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									var ids = []
									for (var i = 0; i < container.pendingMembers.length; i++)
										ids.push(container.pendingMembers[i].id)
									var point = inviteMemberBtn.mapToItem(null, 0, inviteMemberBtn.height)
									ModalDialogManager.openDialog(membersSelectPopupComp, {
										"x": point.x,
										"y": point.y,
										"preselectedIds": ids
									})
								}
							}
						}

						Button {
							id: leaveWorkspaceBtn
							visible: !container.isOwner && !container.isCreator && container.tenantData && container.tenantData.m_currentUserId
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
								readonly property bool isMemberCreator: container.tenantData && container.tenantData.m_creatorId && modelData.id === container.tenantData.m_creatorId
								readonly property bool isCurrentUser: container.tenantData && container.tenantData.m_currentUserId && modelData.id === container.tenantData.m_currentUserId
								readonly property string memberRole: activeMemberDelegate.isMemberCreator ? "Creator" : activeMemberDelegate.isMemberOwner ? "Owner" : container.__getUserRole(modelData.id)

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
										color: (activeMemberDelegate.isMemberOwner || activeMemberDelegate.isMemberCreator) ? Style.selectedColor : Style.borderColor

										BaseText {
											anchors.centerIn: parent
											text: (modelData.name || modelData.id) ? (modelData.name || modelData.id).charAt(0).toUpperCase() : "?"
											font.pixelSize: Style.fontSizeM
											font.bold: true
											color: Style.textColor
										}

										// Crown icon for owner
										Image {
											visible: activeMemberDelegate.isMemberOwner || activeMemberDelegate.isMemberCreator
											anchors.top: parent.top
											anchors.right: parent.right
											anchors.topMargin: -Style.marginXS
											anchors.rightMargin: -Style.marginXS
											width: Style.fontSizeS
											height: Style.fontSizeS
											source: "qrc:/" + Style.getIconPath("Icons/Crown", Icon.State.On, Icon.Mode.Normal)
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
												font.bold: activeMemberDelegate.isMemberOwner || activeMemberDelegate.isMemberCreator
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
												color: Style.selectedColor

												BaseText {
													id: youText
													anchors.centerIn: parent
													text: qsTr("You")
													font.pixelSize: Style.fontSizeXS
													color: Style.linkColor
												}
											}
										}
									}

									// Status / Role badge
									Row {
										id: activeMemberStatusBadge
										anchors.verticalCenter: parent.verticalCenter
										spacing: Style.marginXS

										Rectangle {
											width: roleBadgeText.implicitWidth + Style.marginL
											height: roleBadgeText.implicitHeight + Style.marginS
											radius: Style.radiusS
											color: (activeMemberDelegate.isMemberOwner || activeMemberDelegate.isMemberCreator) ? Style.selectedColor : Style.baseColor
											border.color: (activeMemberDelegate.isMemberOwner || activeMemberDelegate.isMemberCreator) ? Style.secondColor : Style.borderColor
											border.width: 1

											BaseText {
												id: roleBadgeText
												anchors.centerIn: parent
												text: activeMemberDelegate.memberRole
												font.pixelSize: Style.fontSizeS
												color: (activeMemberDelegate.isMemberOwner || activeMemberDelegate.isMemberCreator) ? Style.secondColor : Style.textColor
											}
										}

										Rectangle {
											width: activeStatusText.implicitWidth + Style.marginL
											height: activeStatusText.implicitHeight + Style.marginS
											radius: Style.radiusS
											color: Style.selectedColor

											BaseText {
												id: activeStatusText
												anchors.centerIn: parent
												text: qsTr("Active")
												font.pixelSize: Style.fontSizeS
												color: Style.textColor
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
											visible: container.canManageMembers || activeMemberDelegate.isCurrentUser
											anchors.centerIn: parent
											tooltipText: qsTr("Actions")
											iconSource: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
											decorator: Component {
												ToolButtonDecorator {
													color: "transparent"
													icon.width: Style.iconSizeXS
												}
											}
											onClicked: {
												var menuItems = []
												if (container.canManageMembers) {
													if (!activeMemberDelegate.isMemberOwner && !activeMemberDelegate.isMemberCreator) {
														menuItems.push({ text: qsTr("Change Role"), action: "changeRole" })
														menuItems.push({ text: qsTr("Remove Member"), action: "remove" })
													}
													if (container.isOwner && !activeMemberDelegate.isMemberOwner) {
														menuItems.push({ text: qsTr("Transfer Ownership"), action: "transfer" })
													}
												} else {
													if (activeMemberDelegate.isCurrentUser && !activeMemberDelegate.isMemberOwner) {
														menuItems.push({ text: qsTr("Leave Workspace"), action: "leave" })
													}
												}
												membersPage.__showActionsMenu(menuItems, modelData.id, modelData.name || modelData.id, activeMemberDelegate.isMemberOwner, activeMemberDelegate.isCurrentUser)
											}
										}
									}
								}
							}
						}
					}

					// ===== Separator between sections =====
					Rectangle {
						width: parent.width
						height: 1
						color: Style.borderColor
						visible: container.canManageMembers || (container.pendingInvitations && container.pendingInvitations.length > 0)
					}

					// ===== Pending Invitations Section =====
					Column {
						id: pendingInvitationsSection
						width: parent.width
						spacing: Style.marginS
						visible: container.canManageMembers || (container.pendingInvitations && container.pendingInvitations.length > 0)

						BaseText {
							text: qsTr("Pending Invitations")
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
						}

						// Empty state
						BaseText {
							visible: !container.pendingInvitations || container.pendingInvitations.length === 0
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

								readonly property bool isExpired: container.__isInvitationExpired(modelData.expiresAt)
								readonly property bool isRevoked: modelData.status === "revoked" || modelData.status === "Revoked"
								readonly property string effectiveStatus: inviteDelegate.isExpired ? "Expired" : inviteDelegate.isRevoked ? "Revoked" : "Pending"

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
											color: Style.textColor
											elide: Text.ElideRight
											width: parent.width
										}

										BaseText {
											text: container.__formatInvitationInfo(modelData.invitedByName || modelData.invitedByUserId || "", modelData.expiresAt || "")
											font.pixelSize: Style.fontSizeS
											color: inviteDelegate.isExpired ? Style.errorTextColor : Style.inactiveTextColor
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
											color: Style.backgroundColor2

											BaseText {
												id: inviteStatusText
												anchors.centerIn: parent
												text: inviteDelegate.effectiveStatus === "Expired" ? qsTr("Expired") : inviteDelegate.effectiveStatus === "Revoked" ? qsTr("Revoked") : qsTr("Pending")
												font.pixelSize: Style.fontSizeXS
												color: inviteDelegate.effectiveStatus === "Expired" ? Style.errorTextColor : Style.textColor
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
											visible: container.canManageMembers && inviteDelegate.effectiveStatus === "Pending"
											anchors.centerIn: parent
											tooltipText: qsTr("Actions")
											iconSource: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
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
			function __showActionsMenu(menuItems, userId, userName, isOwnerTarget, isCurrentUserTarget) {
				if (menuItems.length === 0)
					return

				if (menuItems.length === 1) {
					membersPage.__executeAction(menuItems[0].action, userId, userName)
					return
				}

				membersPage.__pendingMenuUserId = userId
				membersPage.__pendingMenuUserName = userName
				membersPage.__pendingMenuIsOwner = isOwnerTarget
				membersPage.__pendingMenuIsCurrentUser = isCurrentUserTarget
				memberActionsMenu.popup()
			}

			property string __pendingMenuUserId: ""
			property string __pendingMenuUserName: ""
			property bool __pendingMenuIsOwner: false
			property bool __pendingMenuIsCurrentUser: false

			function __showInviteActionsMenu(menuItems, invitationId, userName) {
				if (menuItems.length === 1) {
					membersPage.__executeInviteAction(menuItems[0].action, invitationId, userName)
					return
				}
				membersPage.__pendingMenuUserId = invitationId
				membersPage.__pendingMenuUserName = userName
				inviteActionsMenu.popup()
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
				} else if (action === "changeRole") {
					membersPage.__pendingMenuUserId = userId
					membersPage.__pendingMenuUserName = userName
					roleAssignMenu.popup()
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

			// --- Standard QML Menu for member actions ---
			Menu {
				id: memberActionsMenu
				font.pixelSize: Style.fontSizeM
				font.family: Style.fontFamily

				MenuItem {
					text: qsTr("Change Role")
					visible: container.canManageMembers && !membersPage.__pendingMenuIsOwner
					height: visible ? implicitHeight : 0
					onTriggered: roleAssignMenu.popup()
				}
				MenuItem {
					text: qsTr("Remove Member")
					visible: container.canManageMembers && !membersPage.__pendingMenuIsOwner
					height: visible ? implicitHeight : 0
					onTriggered: membersPage.__executeAction("remove", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
				}
				MenuItem {
					text: qsTr("Transfer Ownership")
					visible: container.isOwner
					height: visible ? implicitHeight : 0
					onTriggered: membersPage.__executeAction("transfer", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
				}
				MenuItem {
					text: qsTr("Leave Workspace")
					visible: !container.canManageMembers && membersPage.__pendingMenuIsCurrentUser
					height: visible ? implicitHeight : 0
					onTriggered: membersPage.__executeAction("leave", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
				}
			}

			// --- Role assignment menu (shown from Change Role action) ---
			Menu {
				id: roleAssignMenu
				title: qsTr("Assign Role")
				font.pixelSize: Style.fontSizeM
				font.family: Style.fontFamily

				MenuItem {
					text: qsTr("Admin")
					onTriggered: {
						// TODO: call server to change role to Admin for membersPage.__pendingMenuUserId
					}
				}
				MenuItem {
					text: qsTr("Member")
					onTriggered: {
						// TODO: call server to change role to Member for membersPage.__pendingMenuUserId
					}
				}
			}

			// --- Standard QML Menu for invite actions ---
			Menu {
				id: inviteActionsMenu
				font.pixelSize: Style.fontSizeM
				font.family: Style.fontFamily

				MenuItem {
					text: qsTr("Resend Invitation")
					onTriggered: membersPage.__executeInviteAction("resend", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
				}
				MenuItem {
					text: qsTr("Revoke Invitation")
					onTriggered: membersPage.__executeInviteAction("revoke", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
				}
			}

			// --- FilterableSelectPopup for user invitation ---
			Component {
				id: membersSelectPopupComp

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
							arr.push({id: selId, name: selName})
						}
						container.__inviteSelectedUsers(arr)
					}

					Component.onDestruction: {
						if (container.__membersModifiedLocally) {
							container.doUpdateModel()
							container.__membersModifiedLocally = false
						}
					}
				}
			}
		}
	}

	// ===== Page: Roles =====
	Component {
		id: rolesPageComp

		Item {
			id: rolesPage

			StackView {
				id: rolesStackView
				anchors.fill: parent
				initialItem: rolesListView
			}

			// --- Roles List View ---
			Component {
				id: rolesListView

				Item {
					// Header (fixed)
					Column {
						id: rolesListHeader
						anchors.top: parent.top
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginM

						Row {
							width: parent.width
							spacing: Style.marginM

							Column {
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXS

								BaseText {
									text: qsTr("Roles")
									font.pixelSize: Style.fontSizeXL
									font.bold: true
									color: Style.textColor
								}

								BaseText {
									text: qsTr("Define custom roles for your workspace members.")
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}
							}

							Item {
								width: parent.width
									- parent.children[0].width
									- (createRoleBtn.visible ? createRoleBtn.width : 0)
									- parent.spacing * 2
								height: 1
							}

							Text {
								id: createRoleBtn
								visible: container.canManageMembers
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Create Role")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: rolesStackView.push(roleEditorView)
								}
							}
						}

						// Filter
						SearchTextInput {
							id: rolesFilter
							width: parent.width
							placeholderText: qsTr("Filter roles...")
						}

						Rectangle {
							width: parent.width
							height: 1
							color: Style.borderColor
						}
					}

					// Scrollable roles list
					CustomScrollbar {
						id: rolesListScrollbar
						z: parent.z + 1
						anchors.right: parent.right
						anchors.top: rolesListContent.top
						anchors.bottom: rolesListContent.bottom
						secondSize: Style.marginM
						targetItem: rolesListContent
					}

					Flickable {
						id: rolesListContent
						anchors.top: rolesListHeader.bottom
						anchors.topMargin: Style.marginM
						anchors.bottom: parent.bottom
						anchors.bottomMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: rolesListScrollbar.left
						anchors.rightMargin: Style.marginXL
						contentWidth: rolesListColumn.width
						contentHeight: rolesListColumn.height
						boundsBehavior: Flickable.StopAtBounds
						clip: true

						Column {
							id: rolesListColumn
							width: rolesListContent.width
							spacing: 0

							// Built-in roles
							Repeater {
								model: {
									var allRoles = [
										{ name: "Creator", description: qsTr("Full control including Permissions. Permanently assigned to tenant creator."), builtIn: true },
										{ name: "Owner", description: qsTr("Superuser within the tenant. Can manage all aspects except Permissions."), builtIn: true },
										{ name: "Admin", description: qsTr("Can invite/remove members and manage roles and groups."), builtIn: true },
										{ name: "Member", description: qsTr("Default role after accepting invitation. Read-only access."), builtIn: true }
									]
									if (!rolesFilter.text)
										return allRoles
									var filter = rolesFilter.text.toLowerCase()
									return allRoles.filter(function(r) { return r.name.toLowerCase().indexOf(filter) >= 0 || r.description.toLowerCase().indexOf(filter) >= 0 })
								}

								delegate: Rectangle {
									width: rolesListColumn.width
									height: roleItemRow.implicitHeight + Style.marginM * 2
									color: roleItemMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

									MouseArea {
										id: roleItemMouseArea
										anchors.fill: parent
										hoverEnabled: true
										acceptedButtons: Qt.NoButton
									}

									Row {
										id: roleItemRow
										anchors.left: parent.left
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										anchors.margins: Style.marginM
										spacing: Style.marginM

										// Role icon
										Rectangle {
											width: Style.iconSizeL
											height: Style.iconSizeL
											radius: Style.radiusS
											anchors.verticalCenter: parent.verticalCenter
											color: Style.selectedColor

											BaseText {
												anchors.centerIn: parent
												text: modelData.name.charAt(0)
												font.pixelSize: Style.fontSizeM
												font.bold: true
												color: Style.secondColor
											}
										}

										// Role info
										Column {
											anchors.verticalCenter: parent.verticalCenter
											spacing: Style.marginXS
											width: parent.width - Style.iconSizeL - builtInBadge.width - parent.spacing * 2

											BaseText {
												text: modelData.name
												font.pixelSize: Style.fontSizeM
												font.bold: true
												color: Style.textColor
											}

											BaseText {
												text: modelData.description
												font.pixelSize: Style.fontSizeS
												color: Style.inactiveTextColor
												elide: Text.ElideRight
												width: parent.width
											}
										}

										// Built-in badge
										Rectangle {
											id: builtInBadge
											visible: modelData.builtIn === true
											anchors.verticalCenter: parent.verticalCenter
											width: builtInText.implicitWidth + Style.marginM
											height: builtInText.implicitHeight + Style.marginXS
											radius: Style.radiusS
											color: Style.backgroundColor2

											BaseText {
												id: builtInText
												anchors.centerIn: parent
												text: qsTr("Built-in")
												font.pixelSize: Style.fontSizeXS
												color: Style.inactiveTextColor
											}
										}
									}

									// Separator
									Rectangle {
										anchors.bottom: parent.bottom
										width: parent.width
										height: 1
										color: Style.borderColor
										opacity: 0.5
									}
								}
							}
						}
					}
				}
			}

			// --- Role Editor View (pushed on stack) ---
			Component {
				id: roleEditorView

				Item {
					// Header with back button
					Row {
						id: roleEditorHeader
						anchors.top: parent.top
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginM

						Text {
							anchors.verticalCenter: parent.verticalCenter
							text: "← " + qsTr("Back")
							font.pixelSize: Style.fontSizeM
							color: Style.linkColor

							MouseArea {
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: rolesStackView.pop()
							}
						}

						BaseText {
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Create Role")
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}
					}

					// Editor content
					Column {
						anchors.top: roleEditorHeader.bottom
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginXL

						GroupElementView {
							width: parent.width

							TextInputElementView {
								id: roleNameInput
								name: qsTr("Role Name")
								placeHolderText: qsTr("Enter the role name")
							}

							TextInputElementView {
								id: roleDescInput
								name: qsTr("Description")
								placeHolderText: qsTr("Describe what this role can do")
							}
						}

						Row {
							spacing: Style.marginM

							Button {
								text: qsTr("Create")
								onClicked: {
									// TODO: save role via GQL
									rolesStackView.pop()
								}
							}

							Button {
								text: qsTr("Cancel")
								onClicked: rolesStackView.pop()
							}
						}
					}
				}
			}
		}
	}

	// ===== Page: Groups =====
	Component {
		id: groupsPageComp

		Item {
			id: groupsPage

			StackView {
				id: groupsStackView
				anchors.fill: parent
				initialItem: groupsListView
			}

			// --- Groups List View ---
			Component {
				id: groupsListView

				Item {
					// Header (fixed)
					Column {
						id: groupsListHeader
						anchors.top: parent.top
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginM

						Row {
							width: parent.width
							spacing: Style.marginM

							Column {
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXS

								BaseText {
									text: qsTr("Groups")
									font.pixelSize: Style.fontSizeXL
									font.bold: true
									color: Style.textColor
								}

								BaseText {
									text: qsTr("Organize members into groups for easier permission management.")
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}
							}

							Item {
								width: parent.width
									- parent.children[0].width
									- (createGroupBtn.visible ? createGroupBtn.width : 0)
									- parent.spacing * 2
								height: 1
							}

							Text {
								id: createGroupBtn
								visible: container.canManageMembers
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Create Group")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: groupsStackView.push(groupEditorView)
								}
							}
						}

						// Filter
						SearchTextInput {
							id: groupsFilter
							width: parent.width
							placeholderText: qsTr("Filter groups...")
						}

						Rectangle {
							width: parent.width
							height: 1
							color: Style.borderColor
						}
					}

					// Empty state or list
					Item {
						anchors.top: groupsListHeader.bottom
						anchors.topMargin: Style.marginM
						anchors.bottom: parent.bottom
						anchors.bottomMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL

						BaseText {
							anchors.centerIn: parent
							text: qsTr("No groups created yet. Create a group to organize members.")
							font.pixelSize: Style.fontSizeM
							color: Style.inactiveTextColor
							wrapMode: Text.WordWrap
							width: parent.width
							horizontalAlignment: Text.AlignHCenter
						}
					}
				}
			}

			// --- Group Editor View (pushed on stack) ---
			Component {
				id: groupEditorView

				Item {
					// Header with back button
					Row {
						id: groupEditorHeader
						anchors.top: parent.top
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginM

						Text {
							anchors.verticalCenter: parent.verticalCenter
							text: "← " + qsTr("Back")
							font.pixelSize: Style.fontSizeM
							color: Style.linkColor

							MouseArea {
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: groupsStackView.pop()
							}
						}

						BaseText {
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Create Group")
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}
					}

					// Editor content
					Column {
						anchors.top: groupEditorHeader.bottom
						anchors.topMargin: Style.marginXL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL
						spacing: Style.marginXL

						GroupElementView {
							width: parent.width

							TextInputElementView {
								id: groupNameInput
								name: qsTr("Group Name")
								placeHolderText: qsTr("Enter the group name")
							}

							TextInputElementView {
								id: groupDescInput
								name: qsTr("Description")
								placeHolderText: qsTr("Describe this group's purpose")
							}
						}

						Row {
							spacing: Style.marginM

							Button {
								text: qsTr("Create")
								onClicked: {
									// TODO: save group via GQL
									groupsStackView.pop()
								}
							}

							Button {
								text: qsTr("Cancel")
								onClicked: groupsStackView.pop()
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

			// Fixed header area
			Column {
				id: permissionsHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL
				spacing: Style.marginM

				Row {
					width: parent.width
					spacing: Style.marginM

					Column {
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginXS

						BaseText {
							text: qsTr("Permissions")
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							text: qsTr("Select which product features are available for this tenant.")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}

					Item {
						width: parent.width
							- parent.children[0].width
							- permExpandBtn.width
							- permCollapseBtn.width
							- parent.spacing * 3
						height: 1
					}

					Text {
						id: permExpandBtn
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("Expand All")
						font.pixelSize: Style.fontSizeM
						color: Style.linkColor

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								if (tenantPermissionsTreeView.treeView)
									tenantPermissionsTreeView.treeView.expandAll()
							}
						}
					}

					Text {
						id: permCollapseBtn
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("Collapse All")
						font.pixelSize: Style.fontSizeM
						color: Style.linkColor

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								if (tenantPermissionsTreeView.treeView)
									tenantPermissionsTreeView.treeView.collapseAll()
							}
						}
					}
				}

				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
				}
			}

			// Scrollable tree area
			TreeViewElementView {
				id: tenantPermissionsTreeView
				anchors.top: permissionsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				Connections {
					target: tenantPermissionsTreeView.treeView

					function onCheckedItemsChanged() {
						container.doUpdateModel()
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
