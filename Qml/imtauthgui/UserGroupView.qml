import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthGroupsSdl 1.0
import imtauthUsersSdl 1.0
import imtauthRolesSdl 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtauthgui 1.0

ViewBase {
	id: container;
	contentColor: Style.baseColor
	property GroupData groupData: model;
	property string productId;
	// Permission tree of the product [{groupId, groupName, entries: [{permissionId, displayName}]}], for display names.
	property var permissionGroups: []

	function updateGui(){
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		var parentGroupsPageInstance = multiPageView.getPageById("ParentGroups")
		if (parentGroupsPageInstance)
			parentGroupsPageInstance.updateGui()
		var usersPageInstance = multiPageView.getPageById("Users")
		if (usersPageInstance)
			usersPageInstance.updateGui()
		var rolesPageInstance = multiPageView.getPageById("Roles")
		if (rolesPageInstance)
			rolesPageInstance.updateGui()
		var permissionsPageInstance = multiPageView.getPageById("Permissions")
		if (permissionsPageInstance)
			permissionsPageInstance.updateGui()
	
		container.updateBadges()
	}
	
	function updateModel(){
		if (!container.groupData){
			return
		}

		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
		var parentGroupsPageInstance = multiPageView.getPageById("ParentGroups")
		if (parentGroupsPageInstance)
			parentGroupsPageInstance.updateModel()
		var usersPageInstance = multiPageView.getPageById("Users")
		if (usersPageInstance)
			usersPageInstance.updateModel()
		var rolesPageInstance = multiPageView.getPageById("Roles")
		if (rolesPageInstance)
			rolesPageInstance.updateModel()
		
		groupData.m_productId = productId;
	
		container.updateBadges()
	}
	
	// Emitted after a change of the direct assignments that other ones are inherited through.
	signal assignmentsChanged()

	// The inherited assignments are being recalculated.
	property bool assignmentsUpdating: false

	// Shows the recalculated assignment lists on the loaded pages.
	function updateAssignments(){
		container.setBlockingUpdateModel(true)
		var parentGroupsPageInstance = multiPageView.getPageById("ParentGroups")
		if (parentGroupsPageInstance)
			parentGroupsPageInstance.updateGui()
		var rolesPageInstance = multiPageView.getPageById("Roles")
		if (rolesPageInstance)
			rolesPageInstance.updateGui()
		var permissionsPageInstance = multiPageView.getPageById("Permissions")
		if (permissionsPageInstance)
			permissionsPageInstance.updateGui()
		container.updateBadges()
		container.setBlockingUpdateModel(false)
	}

	// Item counts next to the page names.
	function updateBadges(){
		if (!container.groupData){
			return
		}

		multiPageView.setPageBadge("ParentGroups", String(container.groupData.m_parentGroups ? container.groupData.m_parentGroups.count : 0))
		multiPageView.setPageBadge("Users", String(container.groupData.m_users ? container.groupData.m_users.count : 0))
		multiPageView.setPageBadge("Roles", String(container.groupData.m_roles ? container.groupData.m_roles.count : 0))
		multiPageView.setPageBadge("Permissions", String(container.groupData.m_permissions ? container.groupData.m_permissions.count : 0))
	}

	function getHeaders(){
		return {}
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			// Labelled by what they are *for this group*, so they don't read as the
			// identically-named top-level Users/Roles collections in
			// AdministrationView. Page ids stay untouched, they are the API.
			multiPageView.addPage("ParentGroups", qsTr("Parent Groups"), parentGroupsPageComp, "Icons/Organization")
			multiPageView.addPage("Users", qsTr("Members"), usersPageComp, "Icons/MultipleUser")
			multiPageView.addPage("Roles", qsTr("Group Roles"), rolesPageComp, "Icons/Role")
			multiPageView.addPage("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Key")
			if (PermissionsController.checkPermission("ViewRevisions")){
				multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
			}
			multiPageView.currentIndex = 0
			container.updateBadges()
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: generalPageComp

		Item {
			id: generalPage
			anchors.fill: parent

			function updateGui(){
				generalGroup.updateGui();
			}

			function updateModel(){
				generalGroup.updateModel();
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				
				secondSize: 10;
				targetItem: flickable;
			}
			
			Flickable {
				id: flickable;
				
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				
				contentHeight: bodyColumn.height + 2 * Style.marginXL;
				
				boundsBehavior: Flickable.StopAtBounds;
				
				clip: true;
				
				Column {
					id: bodyColumn;
					
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					
					spacing: Style.spacingXL;
					
					GroupHeaderView {
						width: parent.width;
						
						title: qsTr("General");
						groupView: generalGroup;
					}
					
					GroupElementView {
						id: generalGroup;
						
						width: parent.width;
						
						TextInputElementView {
							id: nameInput;

							// Test instrumentation: matches the AccountEditor/DeviceEditor/etc. convention of
							// an explicit per-field objectName on the ElementView usage site. Inert.
							objectName: "GroupNameInput";

							name: qsTr("Group Name");
							placeHolderText: qsTr("Enter the name");

							onEditingFinished: {
								container.doUpdateModel();
							}

							KeyNavigation.tab: descriptionInput;
						}

						TextInputElementView {
							id: descriptionInput;

							// Test instrumentation - see nameInput's comment above. Inert.
							objectName: "GroupDescriptionInput";

							name: qsTr("Description");
							placeHolderText: qsTr("Enter the description");

							onEditingFinished: {
								container.doUpdateModel();
							}
							
							KeyNavigation.backtab: nameInput;
						}

						function updateGui(){
							if (!container.groupData){
								return
							}
							nameInput.text = container.groupData.m_name;
							descriptionInput.text = container.groupData.m_description;
						}
						
						function updateModel(){
							if (!container.groupData){
								return
							}
							container.groupData.m_description = descriptionInput.text;
							container.groupData.m_name = nameInput.text;
						}
					}
				}
			}
		}
	}

	Component {
		id: parentGroupsPageComp

		Item {
			id: parentGroupsPage
			anchors.fill: parent

			function updateGui(){
				if (!container.groupData){
					return
				}

				parentGroupsTable.loadAssignments(container.groupData.m_parentGroups)
			}

			function updateModel(){
				if (!container.groupData){
					return
				}

				if (!container.groupData.hasParentGroups()){
					container.groupData.emplaceParentGroups()
				}

				var added = parentGroupsTable.syncAssignments(container.groupData.m_parentGroups)
				for (var i = 0; i < added.length; i++){
					var assignment = container.groupData.createParentGroupsArrayElement()
					assignment.m_id = added[i].id
					assignment.m_name = added[i].title
					assignment.m_direct = true
					container.groupData.m_parentGroups.addElement(assignment)
				}
			}

			Component.onCompleted: {
				parentGroupsPage.updateGui();
			}

			Column {
				id: parentGroupsHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (parentGroupsPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, parentGroupsPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Parent Groups") + " (" + parentGroupsTable.itemsCount + ")" + (container.assignmentsUpdating ? "   " + qsTr("Updating...") : "")
					controlComp: Component {
						Row {
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginL

							Text {
								objectName: "RemoveParentGroupLink"
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Remove") + " (" + parentGroupsTable.selectedCount + ")"
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: parentGroupsTable.selectedCount > 0 ? Style.linkColor : Style.inactiveTextColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									enabled: parentGroupsTable.selectedCount > 0
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										parentGroupsTable.removeSelected()
									}
								}
							}

							Text {
								id: parentGroupsTableAddLink
								objectName: "AddParentGroupLink"
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Add Parent Group")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										groupSelectableCollectionEditor.items = parentGroupsTable.directItems()
										groupSelectableCollectionEditor.openSelector(parentGroupsTableAddLink)
									}
								}
							}
						}
					}
				}

				CollectionItemSelectElementView {
					id: groupSelectableCollectionEditor
					width: parent.width
					commandId: ImtauthGroupsSdlCommandIds.s_groupsList
					fields: [GroupItemDataTypeMetaInfo.s_id, GroupItemDataTypeMetaInfo.s_name]
					titleField: GroupItemDataTypeMetaInfo.s_name
					textFilterFieldIds: [GroupItemDataTypeMetaInfo.s_name]
					sortByField: GroupItemDataTypeMetaInfo.s_name
					label: qsTr("Parent Groups")
					addButtonText: qsTr("Add Parent Group")
					// A group cannot be its own parent.
					excludeIds: container.groupData && container.groupData.m_id
						? [container.groupData.m_id]
						: []
					visible: false

					onSelectionChanged: {
						parentGroupsTable.setDirectItems(groupSelectableCollectionEditor.items)
						container.doUpdateModel()
						container.assignmentsChanged()
					}
				}
			}

			AssignmentsTable {
				id: parentGroupsTable
				anchors.top: parentGroupsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: parentGroupsHeader.x
				width: parentGroupsHeader.width
				editable: true
				navigationPath: "Administration/Groups/Group/"
				nameTitle: qsTr("Group")
				emptyText: qsTr("The group has no parent groups.")
				filterPlaceholder: qsTr("Filter groups...")


				onRemoved: {
					container.doUpdateModel()
					container.assignmentsChanged()
				}
			}
		}
	}

	Component {
		id: usersPageComp

		Item {
			id: usersPage
			anchors.fill: parent

			function updateGui(){
				if (!container.groupData){
					return
				}

				usersTable.loadAssignments(container.groupData.m_users)
			}

			function updateModel(){
				if (!container.groupData){
					return
				}

				if (!container.groupData.hasUsers()){
					container.groupData.emplaceUsers()
				}

				var added = usersTable.syncAssignments(container.groupData.m_users)
				for (var i = 0; i < added.length; i++){
					var assignment = container.groupData.createUsersArrayElement()
					assignment.m_id = added[i].id
					assignment.m_name = added[i].title
					assignment.m_direct = true
					container.groupData.m_users.addElement(assignment)
				}
			}

			Component.onCompleted: {
				usersPage.updateGui();
			}

			Column {
				id: usersHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (usersPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, usersPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Members") + " (" + usersTable.itemsCount + ")"
					controlComp: Component {
						Row {
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginL

							Text {
								objectName: "RemoveUserLink"
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Remove") + " (" + usersTable.selectedCount + ")"
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: usersTable.selectedCount > 0 ? Style.linkColor : Style.inactiveTextColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									enabled: usersTable.selectedCount > 0
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										usersTable.removeSelected()
									}
								}
							}

							Text {
								id: usersTableAddLink
								objectName: "AddUserLink"
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Add User")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										userSelectableCollectionEditor.items = usersTable.directItems()
										userSelectableCollectionEditor.openSelector(usersTableAddLink)
									}
								}
							}
						}
					}
				}

				CollectionItemSelectElementView {
					id: userSelectableCollectionEditor
					width: parent.width
					commandId: ImtauthUsersSdlCommandIds.s_usersList
					fields: [UserItemDataTypeMetaInfo.s_id, UserItemDataTypeMetaInfo.s_name]
					titleField: UserItemDataTypeMetaInfo.s_name
					textFilterFieldIds: [UserItemDataTypeMetaInfo.s_name]
					sortByField: UserItemDataTypeMetaInfo.s_name
					label: qsTr("Users")
					addButtonText: qsTr("Add User")
					visible: false

					onSelectionChanged: {
						usersTable.setDirectItems(userSelectableCollectionEditor.items)
						container.doUpdateModel()
					}
				}
			}

			AssignmentsTable {
				id: usersTable
				anchors.top: usersHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: usersHeader.x
				width: usersHeader.width
				editable: true
				navigationPath: "Administration/Users/User/"
				nameTitle: qsTr("User")
				emptyText: qsTr("The group has no members.")
				filterPlaceholder: qsTr("Filter users...")

				onRemoved: {
					container.doUpdateModel()
				}
			}
		}
	}

	Component {
		id: rolesPageComp

		Item {
			id: rolesPage
			anchors.fill: parent

			function updateGui(){
				if (!container.groupData){
					return
				}

				rolesTable.loadAssignments(container.groupData.m_roles)
			}

			function updateModel(){
				if (!container.groupData){
					return
				}

				if (!container.groupData.hasRoles()){
					container.groupData.emplaceRoles()
				}

				var added = rolesTable.syncAssignments(container.groupData.m_roles)
				for (var i = 0; i < added.length; i++){
					var assignment = container.groupData.createRolesArrayElement()
					assignment.m_id = added[i].id
					assignment.m_name = added[i].title
					assignment.m_direct = true
					container.groupData.m_roles.addElement(assignment)
				}
			}

			Component.onCompleted: {
				rolesPage.updateGui();
			}

			Column {
				id: rolesHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (rolesPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, rolesPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Roles") + " (" + rolesTable.itemsCount + ")" + (container.assignmentsUpdating ? "   " + qsTr("Updating...") : "")
					controlComp: Component {
						Row {
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginL

							Text {
								objectName: "RemoveRoleLink"
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Remove") + " (" + rolesTable.selectedCount + ")"
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: rolesTable.selectedCount > 0 ? Style.linkColor : Style.inactiveTextColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									enabled: rolesTable.selectedCount > 0
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										rolesTable.removeSelected()
									}
								}
							}

							Text {
								id: rolesTableAddLink
								objectName: "AddRoleLink"
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Add Role")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										roleSelectableCollectionEditor.items = rolesTable.directItems()
										roleSelectableCollectionEditor.openSelector(rolesTableAddLink)
									}
								}
							}
						}
					}
				}

				CollectionItemSelectElementView {
					id: roleSelectableCollectionEditor
					width: parent.width
					commandId: ImtauthRolesSdlCommandIds.s_rolesList
					fields: [RoleItemDataTypeMetaInfo.s_id, RoleItemDataTypeMetaInfo.s_roleName]
					titleField: RoleItemDataTypeMetaInfo.s_roleName
					textFilterFieldIds: [RoleItemDataTypeMetaInfo.s_roleName]
					sortByField: RoleItemDataTypeMetaInfo.s_roleName
					label: qsTr("Roles")
					addButtonText: qsTr("Add Role")
					visible: false

					// The role list is scoped by product, as a header and as an input field.
					function getHeaders(){
						let headers = {}
						headers["productId"] = container.productId
						return headers
					}

					function setCustomInputParams(inputParams){
						if (container.productId){
							inputParams.InsertField(RoleItemInputTypeMetaInfo.s_productId, container.productId)
						}
					}

					onSelectionChanged: {
						rolesTable.setDirectItems(roleSelectableCollectionEditor.items)
						container.doUpdateModel()
						container.assignmentsChanged()
					}
				}
			}

			AssignmentsTable {
				id: rolesTable
				anchors.top: rolesHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: rolesHeader.x
				width: rolesHeader.width
				editable: true
				navigationPath: "Administration/Roles/Role/"
				nameTitle: qsTr("Role")
				emptyText: qsTr("No roles are assigned.")
				filterPlaceholder: qsTr("Filter roles...")


				onRemoved: {
					container.doUpdateModel()
					container.assignmentsChanged()
				}
			}
		}
	}

	Component {
		id: permissionsPageComp

		PermissionSourcesPage {
			anchors.fill: parent
			permissions: container.groupData ? container.groupData.m_permissions : null
			warnings: container.groupData ? container.groupData.m_accessWarnings : null
			permissionGroups: container.permissionGroups
			updating: container.assignmentsUpdating
			emptyText: qsTr("The group grants no permissions to its members.")
		}
	}

	Component {
		id: historyPageComp

		Item {
			id: historyPage
			anchors.fill: parent

			Item {
				id: centeredContainer
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
			}

			GroupHeaderView {
				id: historyHeader
				anchors.left: centeredContainer.left
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.right: centeredContainer.right
				title: qsTr("History") + " (" + historyView.revisionsCount + ")"
			}

			DocumentHistoryView {
				id: historyView
				anchors.left: centeredContainer.left
				anchors.top: historyHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.right: centeredContainer.right
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				documentId: container.groupData ? container.groupData.m_id : "";
				collectionId: "Groups";

				function getHeaders(){
					return container.getHeaders()
				}
			}
		}
	}
}



