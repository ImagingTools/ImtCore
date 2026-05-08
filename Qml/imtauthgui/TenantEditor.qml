// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtauthTenantsSdl 1.0

DocumentViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property TenantData tenantData: model
	property var pendingMembers: []
	property bool isNewTenant: tenantData ? (!tenantData.m_id || tenantData.m_id === "") : true
	readonly property real memberRoleNameWidthRatio: 0.55
	readonly property real memberRoleComboWidthRatio: 0.45
	readonly property int memberRoleHorizontalMargin: Style.marginL
	readonly property int totalMemberRoleHorizontalMargin: memberRoleHorizontalMargin + memberRoleHorizontalMargin
	// Guard: set when members are modified locally, prevents updateGui from overwriting
	property bool __membersModifiedLocally: false

	onPendingMembersChanged: {
		memberRolesRepeater.model = __buildMemberRolesModel()
	}

	function updateGui(){
		__loadMembersFromModel();
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
		var role = rolesModel.get(index).item
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
		memberRolesRepeater.model = container.__buildMemberRolesModel()
	}

	function __updateMemberRole(userId, newRole) {
		var rolesMap = container.__memberRolesMap
		rolesMap[userId] = newRole
		container.__memberRolesMap = rolesMap
		memberRolesRepeater.model = container.__buildMemberRolesModel()
		container.doUpdateModel()
	}

	function __buildMemberRolesModel() {
		var result = []
		var members = container.pendingMembers
		for (var i = 0; i < members.length; i++) {
			var userId = members[i].id
			var userName = members[i].name || userId
			var role = container.__memberRolesMap[userId] || container.defaultRole
			result.push({ userId: userId, userName: userName, role: role })
		}
		return result
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
				addButtonText: qsTr("Add member")
				filterPlaceholder: qsTr("Type or choose a user")
				collectionId: "Users"
				emptyText: qsTr("No members")
				showCount: true

				onItemRemoved: {
					container.pendingMembers = membersSelector.items.slice()
					container.__membersModifiedLocally = true
					container.doUpdateModel()
				}

				onSelectionChanged: {
					container.pendingMembers = selectedItems
					container.__membersModifiedLocally = true
				}

				onPopupClosed: {
					if (container.__membersModifiedLocally) {
						container.doUpdateModel()
						container.__membersModifiedLocally = false
					}
				}
			}

			// --- Member Roles Section ---
			GroupHeaderView {
				width: parent.width
				title: qsTr("Member Roles")
				groupView: memberRolesGroup
				visible: memberRolesGroup.visible
			}

			GroupElementView {
				id: memberRolesGroup
				width: parent.width
				visible: !container.isNewTenant && container.pendingMembers.length > 0

				Column {
					width: parent.width - container.totalMemberRoleHorizontalMargin
					x: container.memberRoleHorizontalMargin
					spacing: Style.marginM

					Repeater {
						id: memberRolesRepeater
						model: []

						delegate: Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginS

							Row {
								anchors.fill: parent
								anchors.margins: Style.marginS
								spacing: Style.marginL

								BaseText {
									width: (parent.width - parent.spacing) * container.memberRoleNameWidthRatio
									anchors.verticalCenter: parent.verticalCenter
									text: modelData.userName
									elide: Text.ElideRight
								}

								ComboBox {
									id: roleCombo
									width: (parent.width - parent.spacing) * container.memberRoleComboWidthRatio
									anchors.verticalCenter: parent.verticalCenter
									model: container.__getAvailableRolesModel()
									nameId: "name"
									currentIndex: container.__findRoleIndex(modelData.role)

									onFinished: {
										if (!roleCombo.model || index < 0)
											return

										var selectedRole = container.__getRoleModelValue(roleCombo.model, index, "id")
										if (!selectedRole)
											return
										if (selectedRole !== modelData.role) {
											container.__updateMemberRole(modelData.userId, selectedRole)
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
