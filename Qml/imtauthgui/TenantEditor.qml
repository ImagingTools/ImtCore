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
	property var __userNameCache: ({})
	property bool isNewTenant: tenantData ? (!tenantData.m_id || tenantData.m_id === "") : true

	function updateGui(){
		generalGroup.updateGui();
		__loadMembersFromModel();
	}

	function updateModel(){
		generalGroup.updateModel();
		// Sync memberIds back to model
		if (container.tenantData) {
			container.tenantData.m_memberIds = __pendingMemberIds()
		}
	}

	function __pendingMemberIds() {
		var ids = []
		for (var i = 0; i < pendingMembers.length; ++i)
			ids.push(pendingMembers[i].id)
		return ids
	}

	function __loadMembersFromModel() {
		if (!container.tenantData)
			return
		var ids = container.tenantData.m_memberIds || []
		var members = []
		for (var i = 0; i < ids.length; i++) {
			var uid = ids[i]
			members.push({ id: uid, name: container.__userNameCache[uid] || uid })
		}
		container.pendingMembers = members
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
				width: parent.width
				visible: !container.isNewTenant
				items: container.pendingMembers
				label: qsTr("Members")
				addButtonText: qsTr("Add member")
				filterPlaceholder: qsTr("Type or choose a user")
				collectionId: "Users"
				emptyText: qsTr("No members")

				onItemRemoved: {
					var arr = container.pendingMembers.slice()
					arr.splice(index, 1)
					container.pendingMembers = arr
					container.doUpdateModel()
				}

				onSelectionChanged: {
					for (var i = 0; i < selectedItems.length; i++) {
						var selName = selectedItems[i].name
						var selId = selectedItems[i].id
						if (selName !== selId) container.__userNameCache[selId] = selName
					}
					container.pendingMembers = selectedItems
					container.doUpdateModel()
				}
			}
		}
	}
}
