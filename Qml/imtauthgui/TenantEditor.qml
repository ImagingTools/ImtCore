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
	property bool isNewTenant: tenantData ? (!tenantData.m_id || tenantData.m_id === "") : true
	readonly property real memberRoleNameWidthRatio: 0.55
	readonly property real memberRoleComboWidthRatio: 0.45
	readonly property int memberRoleHorizontalMargin: Style.marginL
	readonly property int totalMemberRoleHorizontalMargin: memberRoleHorizontalMargin * 2
	readonly property int memberRoleRowMargin: Style.marginS
	readonly property int totalMemberRoleRowMargin: memberRoleRowMargin * 2
	// Guard: set when members are modified locally, prevents updateGui from overwriting
	property bool __membersModifiedLocally: false

	// Role-based access: determine current user's role in this tenant
	readonly property string __currentUserRole:
		!container.tenantData ? ""
		: !container.tenantData.m_currentUserId ? ""
		: container.tenantData.m_ownerId && container.tenantData.m_currentUserId === container.tenantData.m_ownerId ? "Owner"
		: container.__memberRolesMap && container.__memberRolesMap[container.tenantData.m_currentUserId] ? container.__memberRolesMap[container.tenantData.m_currentUserId]
		: container.__isCurrentUserInvited() ? "Invited"
		: ""
	readonly property bool __isOwnerOrAdmin: container.__currentUserRole === "Owner" || container.__currentUserRole === "Admin"
	readonly property bool __isOwner: container.__currentUserRole === "Owner"
	readonly property bool __isReadOnly: !container.isNewTenant && !container.__isOwnerOrAdmin

	onPendingMembersChanged: {
		// Trigger rebuild by reassigning __memberRolesMap (bottomComp binds to __buildMemberRolesModel)
		container.__memberRolesMap = container.__memberRolesMap
	}

	function updateGui(){
		__loadMembersFromModel();
		__loadInvitationsFromModel();
		generalGroup.updateGui();
	}

	function updateModel(){
		generalGroup.updateModel();
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

			// memberRoles: structured SDL list — use emplace/create/addElement API
			if (!container.tenantData.hasMemberRoles()) {
				container.tenantData.emplaceMemberRoles()
			}
			container.tenantData.m_memberRoles.clear()
			var members = container.pendingMembers
			for (var i = 0; i < members.length; i++) {
				var entry = container.tenantData.createMemberRolesArrayElement()
				if (entry) {
					entry.m_userId = members[i].id
					entry.m_role = container.__memberRolesMap[members[i].id] || container.defaultRole
					container.tenantData.m_memberRoles.addElement(entry)
				}
			}
		}
	}

	function __loadMembersFromModel() {
		if (container.__membersModifiedLocally) {
			container.__membersModifiedLocally = false
			return
		}
		if (!container.tenantData)
			return
		// m_members is an SDL-generated list — access via .count and .get(i).item
		var serverMembers = container.tenantData.m_members
		var members = []
		if (serverMembers) {
			var count = serverMembers.count || 0
			for (var i = 0; i < count; i++) {
				var m = serverMembers.get(i).item
				if (m) {
					// Keep id visible if server cannot resolve the display name; this avoids blank chips/role rows.
					members.push({ id: m.m_id || "", name: m.m_name || m.m_id || "" })
				}
			}
		}
		container.pendingMembers = members
		__loadMemberRolesFromModel()
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
						role: invitation.m_role || container.defaultRole,
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
				createInvitationInput.m_tenantId = container.tenantData.m_id || ""
				createInvitationInput.m_userId = selected.id
				createInvitationInput.m_role = "Member"
				createInvitationSender.send(createInvitationInput)
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

	// --- Member roles support ---
	// Available role options from server (TenantData.availableRoles)
	readonly property string defaultRole: "Member"

	function __getAvailableRolesModel() {
		if (!container.tenantData || !container.tenantData.m_availableRoles)
			return null
		return container.tenantData.m_availableRoles
	}

	function __getRoleModelValue(rolesModel, index, key) {
		if (!rolesModel || index < 0)
			return ""
		// Prefer SDL-generated m_* roles but keep plain-key lookup for regular QML models.
		if (rolesModel.getData)
			return rolesModel.getData("m_" + key, index) || rolesModel.getData(key, index) || ""
		var modelItem = rolesModel.get(index)
		var role = modelItem ? modelItem.item : null
		if (!role)
			return ""
		return role["m_" + key] || role[key] || ""
	}

	function __findRoleIndex(roleId) {
		var roles = container.__getAvailableRolesModel()
		if (!roles)
			return -1

		var count = roles.getItemsCount ? roles.getItemsCount() : (roles.count || 0)
		for (var i = 0; i < count; i++) {
			var id = container.__getRoleModelValue(roles, i, "id")
			var name = container.__getRoleModelValue(roles, i, "name")
			if (id === roleId || name === roleId)
				return i
		}

		return -1
	}

	// Map of userId -> role string (built from TenantData.memberRoles)
	property var __memberRolesMap: ({})

	function __loadMemberRolesFromModel() {
		var rolesMap = {}
		if (container.tenantData && container.tenantData.m_memberRoles) {
			var roleEntries = container.tenantData.m_memberRoles
			var count = roleEntries.count || 0
			for (var i = 0; i < count; i++) {
				var entry = roleEntries.get(i).item
				if (entry && entry.m_userId) {
					rolesMap[entry.m_userId] = entry.m_role || container.defaultRole
				}
			}
		}
		container.__memberRolesMap = rolesMap
	}

	function __updateMemberRole(userId, newRole) {
		var rolesMap = container.__memberRolesMap
		rolesMap[userId] = newRole
		container.__memberRolesMap = rolesMap
		container.doUpdateModel()
	}

	function __buildMemberRolesModel() {
		var result = []
		var members = container.pendingMembers
		for (var i = 0; i < members.length; i++) {
			var userId = members[i].id
			var userName = members[i].name || userId
			var role = container.__memberRolesMap[userId] || container.defaultRole
			result.push({ userId: userId, userName: userName, role: role, isPending: false })
		}
		// Merge pending invitations into the same list
		var invitations = container.pendingInvitations
		for (var j = 0; j < invitations.length; j++) {
			var inv = invitations[j]
			var isExpired = container.__isInvitationExpired(inv.expiresAt)
			result.push({
				userId: inv.userId,
				userName: inv.userName,
				role: inv.role,
				isPending: true,
				invitationId: inv.id,
				status: isExpired ? "Expired" : inv.status,
				isExpired: isExpired,
				invitedByName: inv.invitedByName || inv.invitedByUserId || "",
				createdAt: inv.createdAt || "",
				expiresAt: inv.expiresAt || ""
			})
		}
		return result
	}

	function __formatInvitationInfo(invitedByName, expiresAt) {
		var byPart = invitedByName ? qsTr("by %1").arg(invitedByName) : ""
		var expPart = expiresAt ? qsTr("expires %1").arg(container.__formatDateTime(expiresAt)) : ""
		return byPart && expPart ? byPart + " · " + expPart : byPart + expPart
	}

	function __isCurrentUserInvited() {
		if (!container.tenantData || !container.tenantData.m_currentUserId)
			return false
		var userId = container.tenantData.m_currentUserId
		for (var i = 0; i < container.pendingInvitations.length; i++) {
			if (container.pendingInvitations[i].userId === userId)
				return true
		}
		return false
	}

	function __isInvitationExpired(expiresAt) {
		if (!expiresAt)
			return false
		var expDate = new Date(expiresAt)
		if (isNaN(expDate.getTime()))
			return false
		return expDate.getTime() < Date.now()
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
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
		contentHeight: bodyColumn.height + 2 * Style.marginXL

		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			GroupHeaderView {
				width: parent.width
				title: qsTr("General")
				groupView: generalGroup
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

				TextInputElementView {
					id: nameInput

					name: qsTr("Tenant Name")
					placeHolderText: qsTr("Enter the tenant name")
					enabled: container.__isOwner || container.isNewTenant

					onEditingFinished: {
						let oldText = container.tenantData ? container.tenantData.m_name : ""
						if (oldText !== nameInput.text){
							container.doUpdateModel();
						}
					}

					KeyNavigation.tab: descriptionInput
					KeyNavigation.backtab: isActiveInput
				}

				TextInputElementView {
					id: descriptionInput

					name: qsTr("Description")
					placeHolderText: qsTr("Enter the description")
					enabled: container.__isOwner || container.isNewTenant

					onEditingFinished: {
						let oldText = container.tenantData ? container.tenantData.m_description : ""
						if (oldText !== descriptionInput.text){
							container.doUpdateModel();
						}
					}

					KeyNavigation.tab: isActiveInput
					KeyNavigation.backtab: nameInput
				}

				SwitchElementView {
					id: isActiveInput

					name: qsTr("Active")
					enabled: container.__isOwner || container.isNewTenant

					onCheckedChanged: {
						container.doUpdateModel();
					}
				}

				function updateGui(){
					if (!container.tenantData) return;
					nameInput.text = container.tenantData.m_name || ""
					descriptionInput.text = container.tenantData.m_description || ""
					isActiveInput.checked = container.tenantData.m_isActive !== undefined ? container.tenantData.m_isActive : true
				}

				function updateModel(){
					if (!container.tenantData) return;
					container.tenantData.m_name = nameInput.text
					container.tenantData.m_description = descriptionInput.text
					container.tenantData.m_isActive = isActiveInput.checked
				}
			}

			ItemSelectElementView {
				id: membersSelector
				width: parent.width
				visible: !container.isNewTenant
				items: container.pendingMembers
				label: qsTr("Members")
				addButtonText: qsTr("Create invitation")
				filterPlaceholder: qsTr("Type or choose a user")
				collectionId: "Users"
				emptyText: qsTr("No members")
				showCount: true
				editable: container.__isOwnerOrAdmin
				nonRemovableIds: container.tenantData && container.tenantData.m_ownerId ? [container.tenantData.m_ownerId] : []

				onItemRemoved: {
					container.pendingMembers = membersSelector.items.slice()
					container.__membersModifiedLocally = true
					container.doUpdateModel()
				}

				onSelectionChanged: {
					container.__inviteSelectedUsers(selectedItems)
					membersSelector.items = container.pendingMembers.slice()
				}

				onPopupClosed: {
					if (container.__membersModifiedLocally) {
						container.doUpdateModel()
						container.__membersModifiedLocally = false
					}
					membersSelector.items = container.pendingMembers.slice()
				}

				bottomComp: Component {
					Column {
						width: parent.width
						spacing: Style.spacingXS

						Text {
							visible: membersSelector.items.length === 0 && container.pendingInvitations.length === 0
							width: parent.width
							text: membersSelector.emptyText
							font.pixelSize: Style.fontSizeM
							color: Style.inactiveTextColor
							wrapMode: Text.WordWrap
						}

						Repeater {
							model: container.__buildMemberRolesModel()

							delegate: Item {
								id: memberDelegate
								width: parent.width
								height: memberDelegate.isPending
									? Style.controlHeightM * 1.5 + container.totalMemberRoleRowMargin
									: Style.controlHeightM + container.totalMemberRoleRowMargin

								readonly property bool isOwner: container.tenantData && modelData.userId === container.tenantData.m_ownerId
								readonly property bool isPending: modelData.isPending === true

								Column {
									anchors.fill: parent
									anchors.margins: container.memberRoleRowMargin
									spacing: Style.marginXS

									Row {
										width: parent.width
										spacing: Style.marginL

										// Pending indicator
										Text {
											visible: memberDelegate.isPending
											anchors.verticalCenter: parent.verticalCenter
											text: "⏳"
											font.pixelSize: Style.fontSizeM
											width: Style.iconSizeS
										}

										BaseText {
											width: memberDelegate.isPending
												? (parent.width - parent.spacing * 2 - Style.iconSizeS - chipRemoveBtn.width) * container.memberRoleNameWidthRatio
												: (parent.width - parent.spacing - chipRemoveBtn.width) * container.memberRoleNameWidthRatio
											anchors.verticalCenter: parent.verticalCenter
											text: modelData.userName
											elide: Text.ElideRight
											color: memberDelegate.isPending ? Style.inactiveTextColor : Style.textColor
										}

										ComboBox {
											id: roleCombo
											visible: !memberDelegate.isPending
											width: (parent.width - parent.spacing - chipRemoveBtn.width) * container.memberRoleComboWidthRatio
											anchors.verticalCenter: parent.verticalCenter
											model: container.__getAvailableRolesModel()
											nameId: "name"
											currentIndex: container.__findRoleIndex(modelData.role)
											enabled: !memberDelegate.isOwner && container.__isOwnerOrAdmin

											onFinished: {
												var selectedIndex = index
												if (!roleCombo.model || selectedIndex < 0)
													return

												var selectedRole = container.__getRoleModelValue(roleCombo.model, selectedIndex, "id")
												if (!selectedRole)
													return
												if (selectedRole !== modelData.role) {
													container.__updateMemberRole(modelData.userId, selectedRole)
												}
											}
										}

										BaseText {
											visible: memberDelegate.isPending
											width: memberDelegate.isPending
												? (parent.width - parent.spacing * 2 - Style.iconSizeS - chipRemoveBtn.width) * container.memberRoleComboWidthRatio
												: 0
											anchors.verticalCenter: parent.verticalCenter
											text: modelData.isExpired ? qsTr("Expired") : qsTr("%1 (%2)").arg(modelData.role).arg(modelData.status)
											color: modelData.isExpired ? "#DA3633" : Style.inactiveTextColor
											font.bold: modelData.isExpired
											elide: Text.ElideRight
										}

										ToolButton {
											id: chipRemoveBtn
											visible: !memberDelegate.isPending && membersSelector.editable && membersSelector.nonRemovableIds.indexOf(modelData.userId) < 0
											anchors.verticalCenter: parent.verticalCenter
											iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
											decorator: Component {
												ToolButtonDecorator {
													color: "transparent"
													icon.width: Style.iconSizeXS
												}
											}
											onClicked: {
												var arr = membersSelector.items.slice()
												for (var i = 0; i < arr.length; i++) {
													if (arr[i].id === modelData.userId) {
														arr.splice(i, 1)
														break
													}
												}
												membersSelector.__resolvingNames = true
												membersSelector.items = arr
												membersSelector.__resolvingNames = false
												container.pendingMembers = arr
												container.__membersModifiedLocally = true
												container.doUpdateModel()
											}
										}
									}

									// Second row for pending invitations: expiry info + revoke/resend
									Row {
										visible: memberDelegate.isPending
										width: parent.width
										spacing: Style.marginM

										BaseText {
											width: parent.width - revokeBtn.width - resendBtn.width - parent.spacing * 2
											text: container.__formatInvitationInfo(modelData.invitedByName, modelData.expiresAt)
											color: modelData.isExpired ? "#DA3633" : Style.inactiveTextColor
											font.pixelSize: Style.fontSizeS
											elide: Text.ElideRight
										}

										Button {
											id: resendBtn
											visible: container.__isOwnerOrAdmin
											text: qsTr("Resend")
											onClicked: {
												container.resendInvitationInput.m_invitationId = modelData.invitationId
												container.resendInvitationSender.send(container.resendInvitationInput)
											}
										}

										Button {
											id: revokeBtn
											visible: container.__isOwnerOrAdmin
											text: qsTr("Revoke")
											onClicked: {
												container.revokeInvitationInput.m_invitationId = modelData.invitationId
												container.revokeInvitationSender.send(container.revokeInvitationInput)
												container.__removePendingInvitation(modelData.invitationId)
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

	property CreateTenantInvitationInput createInvitationInput: CreateTenantInvitationInput {}
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
}
