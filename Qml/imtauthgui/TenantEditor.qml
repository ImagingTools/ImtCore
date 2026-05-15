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
	readonly property real memberRoleNameWidthRatio: 0.55
	readonly property real memberRoleComboWidthRatio: 0.45
	readonly property int memberRoleHorizontalMargin: Style.marginL
	readonly property int totalMemberRoleHorizontalMargin: container.memberRoleHorizontalMargin * 2
	readonly property int memberRoleRowMargin: Style.marginS
	readonly property int totalMemberRoleRowMargin: container.memberRoleRowMargin * 2
	readonly property int memberActionColumnWidth: Style.controlHeightM * 2
	// Guard: set when members are modified locally, prevents updateGui from overwriting
	property bool __membersModifiedLocally: false

	// Role-based access: determine current user's access level from ownerId and membership
	readonly property bool __isOwner: container.tenantData && container.tenantData.m_ownerId && container.tenantData.m_currentUserId
		? container.tenantData.m_currentUserId === container.tenantData.m_ownerId
		: false
	readonly property bool __isOwnerOrAdmin: container.__isOwner
		|| (container.__memberRolesMap && container.tenantData && container.tenantData.m_currentUserId
			? container.__memberRolesMap[container.tenantData.m_currentUserId] === "Admin"
			: false)
	readonly property bool __isReadOnly: !container.isNewTenant && !container.__isOwnerOrAdmin

	// Explicit member-roles list model used by the Repeater in Members page
	property var __memberRolesListModel: []

	function __rebuildMemberRolesListModel() {
		container.__memberRolesListModel = container.__buildMemberRolesModel()
	}

	onPendingMembersChanged: {
		container.__rebuildMemberRolesListModel()
	}

	onPendingInvitationsChanged: {
		container.__rebuildMemberRolesListModel()
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
					entry.m_role = container.__memberRolesMap[members[i].id] || ""
					container.tenantData.m_memberRoles.addElement(entry)
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
		container.__loadMemberRolesFromModel()
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
						role: invitation.m_role || "",
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

	// --- Member roles support ---
	// Roles loaded via RoleCollectionDataProvider from Puma
	RoleCollectionDataProvider {
		id: roleCollectionDataProvider
		productId: AuthorizationController.productId
		Component.onCompleted: {
			updateModel()
		}
	}

	function __getAvailableRolesModel() {
		return roleCollectionDataProvider.collectionModel
	}

	function __getRoleModelValue(rolesModel, index, key) {
		if (!rolesModel || index < 0 || index >= rolesModel.getItemsCount())
			return ""
		var val = rolesModel.getData("m_" + key, index)
		if (val !== undefined && val !== null)
			return val || ""
		val = rolesModel.getData(key, index)
		if (val !== undefined && val !== null)
			return val || ""
		return ""
	}

	function __findRoleIndex(roleId) {
		var roles = container.__getAvailableRolesModel()
		if (!roles)
			return -1

		var count = roles.getItemsCount()
		for (var i = 0; i < count; i++) {
			var id = container.__getRoleModelValue(roles, i, "id")
			var name = container.__getRoleModelValue(roles, i, "roleName")
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
					rolesMap[entry.m_userId] = entry.m_role || ""
				}
			}
		}
		container.__memberRolesMap = rolesMap
		container.__rebuildMemberRolesListModel()
	}

	function __updateMemberRole(userId, newRole) {
		var rolesMap = container.__memberRolesMap
		rolesMap[userId] = newRole
		container.__memberRolesMap = rolesMap
		container.__rebuildMemberRolesListModel()
		container.doUpdateModel()
	}

	function __buildMemberRolesModel() {
		var result = []
		var members = container.pendingMembers
		for (var i = 0; i < members.length; i++) {
			var userId = members[i].id
			var userName = members[i].name || userId
			var role = container.__memberRolesMap[userId] || ""
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

	function __isInvitationExpired(expiresAt) {
		if (!expiresAt)
			return false
		var expDate = new Date(expiresAt)
		if (isNaN(expDate.getTime()))
			return false
		return expDate.getTime() < Date.now()
	}

	function __removeMemberById(userId) {
		var arr = membersSelector.items.slice()
		for (var i = 0; i < arr.length; i++) {
			if (arr[i].id === userId) {
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

	// --- Tenant permissions ---
	property TreeItemModel permissionsModel: TreeItemModel {}

	// --- MultiPageView ---
	MultiPageView {
		id: multiPageView

		anchors.fill: parent

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			if (!container.isNewTenant) {
				multiPageView.addPage("Members", qsTr("Members"), membersPageComp, "Icons/MultipleUser")
				multiPageView.addPage("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Role")
			}
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	onIsNewTenantChanged: {
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
							readOnly: !container.__isOwner && !container.isNewTenant

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
							readOnly: !container.__isOwner && !container.isNewTenant

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
							readOnly: !container.__isOwner && !container.isNewTenant

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

					ItemSelectElementView {
						id: membersSelector
						width: parent.width
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
									model: container.__memberRolesListModel

									delegate: Item {
										id: memberDelegate
										width: parent.width
										height: memberDelegate.isPending
											? Style.controlHeightM * 1.5 + container.totalMemberRoleRowMargin
											: Style.controlHeightM + container.totalMemberRoleRowMargin

										readonly property bool isOwner: container.tenantData && modelData.userId === container.tenantData.m_ownerId
										readonly property bool isPending: modelData.isPending === true
										readonly property bool isCurrentUser: container.tenantData && container.tenantData.m_currentUserId && modelData.userId === container.tenantData.m_currentUserId
										readonly property int actionBtnWidth: container.memberActionColumnWidth
										readonly property real contentWidth: memberDelegate.width - container.totalMemberRoleRowMargin * 2 - memberDelegate.actionBtnWidth
											- (memberDelegate.isPending ? Style.iconSizeS + Style.marginL : 0) - Style.marginL

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
													width: memberDelegate.contentWidth * container.memberRoleNameWidthRatio
													anchors.verticalCenter: parent.verticalCenter
													text: modelData.userName
													elide: Text.ElideRight
													color: memberDelegate.isPending ? Style.inactiveTextColor : Style.textColor
												}

												BaseText {
													visible: !memberDelegate.isPending && memberDelegate.isOwner
													width: memberDelegate.contentWidth * container.memberRoleComboWidthRatio
													anchors.verticalCenter: parent.verticalCenter
													text: qsTr("Owner")
												}

												ComboBox {
													id: roleCombo
													visible: !memberDelegate.isPending && !memberDelegate.isOwner
													width: memberDelegate.contentWidth * container.memberRoleComboWidthRatio
													anchors.verticalCenter: parent.verticalCenter
													model: container.__getAvailableRolesModel()
													nameId: "roleName"
													currentIndex: container.__findRoleIndex(modelData.role)
													changeable: container.__isOwnerOrAdmin

													onCurrentIndexChanged: {
														if (roleCombo.currentIndex >= 0){
															var selectedRole = container.__getRoleModelValue(roleCombo.model, roleCombo.currentIndex, "id")
															if (!selectedRole)
																return
															var currentRole = container.__memberRolesMap[modelData.userId] || ""
															if (selectedRole !== currentRole) {
																container.__updateMemberRole(modelData.userId, selectedRole)
															}
														}

													}
												}

												BaseText {
													visible: memberDelegate.isPending
													width: memberDelegate.contentWidth * container.memberRoleComboWidthRatio
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
														container.__removeMemberById(modelData.userId)
													}
												}

												Button {
													id: leaveBtn
													visible: !memberDelegate.isPending && memberDelegate.isCurrentUser && !memberDelegate.isOwner && !chipRemoveBtn.visible
													anchors.verticalCenter: parent.verticalCenter
													width: container.memberActionColumnWidth
													text: qsTr("Leave")
													onClicked: {
														container.__removeMemberById(modelData.userId)
													}
												}

												ToolButton {
													id: transferOwnerBtn
													visible: container.__isOwner && !memberDelegate.isOwner && !memberDelegate.isPending
													anchors.verticalCenter: parent.verticalCenter
													iconSource: Style.getIconPath("Icons/Switch", Icon.State.On, Icon.Mode.Normal)

													decorator: Component {
														ToolButtonDecorator {
															color: "transparent"
															icon.width: Style.iconSizeXS
														}
													}
													onClicked: {
														container.__transferOwnershipTo(modelData.userId, modelData.userName)
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
		}
	}

	// ===== Page: Permissions =====
	Component {
		id: permissionsPageComp

		Item {
			id: permissionsPage

			function updateGui() {
				if (!container.tenantData)
					return

				// Populate permissions model from server data
				container.permissionsModel.clear()
				var allPermsModel = container.tenantData.m_allProductPermissions
				if (allPermsModel) {
					var allCount = allPermsModel.count || 0
					for (var pi = 0; pi < allCount; pi++) {
						var perm = allPermsModel.get(pi).item
						if (perm) {
							var permIndex = container.permissionsModel.insertNewItem()
							container.permissionsModel.setData("FeatureId", perm.m_id || "", permIndex)
							container.permissionsModel.setData("FeatureName", perm.m_name || "", permIndex)
						}
					}
				}
				container.permissionsModel.refresh()
				tenantPermissionHeaders.updateHeaders()

				// Restore checked state from selected permissions
				var selectedPermissionsIds = []
				var permissionsArray = container.tenantData.m_tenantPermissions
				if (permissionsArray) {
					var permCount = permissionsArray.length || 0
					for (var pi2 = 0; pi2 < permCount; pi2++) {
						selectedPermissionsIds.push(permissionsArray[pi2])
					}
				}

				tenantPermissionsTreeView.treeView.uncheckAll()

				var itemsList = tenantPermissionsTreeView.treeView.getItemsDataAsList()
				for (var i = 0; i < itemsList.length; i++) {
					var delegateItem = itemsList[i]
					if (!delegateItem.hasChild) {
						var itemData = delegateItem.getItemData()
						var id = itemData.FeatureId

						if (selectedPermissionsIds.includes(id)) {
							tenantPermissionsTreeView.treeView.checkItem(delegateItem)
						}
					}
				}
			}

			function updateModel() {
				if (!container.tenantData)
					return
				var selectedPermissionIds = []
				var itemsList = tenantPermissionsTreeView.treeView.getCheckedItems()
				for (var j = 0; j < itemsList.length; j++) {
					var delegate = itemsList[j]
					if (!delegate.hasChild) {
						var itemData = delegate.getItemData()
						var id = itemData.FeatureId
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

					GroupHeaderView {
						width: parent.width
						title: qsTr("Permissions")
						groupView: permissionsGroup
					}

					GroupElementView {
						id: permissionsGroup
						width: parent.width

						TreeViewElementView {
							id: tenantPermissionsTreeView

							Component.onCompleted: {
								tenantPermissionsTreeView.treeView.tristate = true
							}

							Connections {
								target: tenantPermissionsTreeView.treeView

								function onCheckedItemsChanged() {
									container.doUpdateModel()
								}
							}

							TreeItemModel {
								id: tenantPermissionHeaders

								function updateHeaders() {
									tenantPermissionHeaders.clear()

									var index = tenantPermissionHeaders.insertNewItem()
									tenantPermissionHeaders.setData("id", "FeatureName", index)
									tenantPermissionHeaders.setData("name", qsTr("Permission"), index)

									tenantPermissionHeaders.refresh()

									tenantPermissionsTreeView.treeView.columnModel = tenantPermissionHeaders
									tenantPermissionsTreeView.treeView.rowModel = container.permissionsModel
								}

								Component.onCompleted: {
									tenantPermissionHeaders.updateHeaders()
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
