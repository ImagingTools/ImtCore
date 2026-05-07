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
	// Guard: set when members are modified locally, prevents updateGui from overwriting
	property bool __membersModifiedLocally: false

	function updateGui(){
		generalGroup.updateGui();
		__loadMembersFromModel();
	}

	function updateModel(){
		generalGroup.updateModel();
		// Sync memberIds and memberRoles back to model
		if (container.tenantData) {
			container.tenantData.m_memberIds = __pendingMemberIds()
			container.tenantData.m_memberRoles = __buildMemberRolesForModel()
		}
	}

	function __pendingMemberIds() {
		var ids = []
		for (var i = 0; i < pendingMembers.length; ++i)
			ids.push(pendingMembers[i].id)
		return ids
	}

	function __loadMembersFromModel() {
		if (container.__membersModifiedLocally) {
			container.__membersModifiedLocally = false
			return
		}
		if (!container.tenantData)
			return
		var ids = container.tenantData.m_memberIds || []
		var members = []
		for (var i = 0; i < ids.length; i++) {
			members.push({ id: ids[i], name: ids[i] })
		}
		container.pendingMembers = members
		__loadMemberRolesFromModel()
	}

	// --- Member roles support ---
	// Static role options matching TenantMemberRole enum
	readonly property var roleOptions: ["Owner", "Admin", "Member", "Guest"]
	readonly property string defaultRole: "Member"

	// Map of userId -> role string (built from TenantData.memberRoles)
	property var __memberRolesMap: ({})

	function __loadMemberRolesFromModel() {
		var rolesMap = {}
		if (container.tenantData && container.tenantData.m_memberRoles) {
			var roleEntries = container.tenantData.m_memberRoles
			for (var i = 0; i < roleEntries.length; i++) {
				var entry = roleEntries[i]
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

	function __buildMemberRolesForModel() {
		var result = []
		var members = container.pendingMembers
		for (var i = 0; i < members.length; i++) {
			var userId = members[i].id
			var role = container.__memberRolesMap[userId] || container.defaultRole
			result.push({ m_userId: userId, m_role: role })
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
					width: parent.width
					spacing: Style.marginM

					Repeater {
						id: memberRolesRepeater
						model: []

						delegate: Row {
							width: parent.width
							spacing: Style.marginL

							BaseText {
								width: parent.width * 0.5
								anchors.verticalCenter: parent.verticalCenter
								text: modelData.userName
								elide: Text.ElideRight
							}

							ComboBox {
								width: parent.width * 0.4
								model: container.roleOptions
								currentIndex: container.roleOptions.indexOf(modelData.role)
								onActivated: {
									var selectedRole = container.roleOptions[index]
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
