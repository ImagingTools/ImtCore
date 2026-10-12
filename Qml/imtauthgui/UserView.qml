import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0
import imtauthRolesSdl 1.0
import imtauthGroupsSdl 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtguigql 1.0

DocumentViewBase {
	id: container;
	
	anchors.fill: parent;
	contentColor: Style.baseColor

	property UserData userData: model;
	property string productId;

	// Permission tree of the product [{groupId, groupName, entries: [{permissionId, displayName}]}], for display names.
	property var permissionGroups: []

	// PasswordPolicyController instance, injected by the owning api client.
	property var passwordPolicy: null;
	
	property var passwordInput: multiPageView.getPageByIndex(0) ? multiPageView.getPageByIndex(0).passwordInput : null;
	property var passwordInputConfirm: multiPageView.getPageByIndex(0) ? multiPageView.getPageByIndex(0).passwordInputConfirm : null;
	
	property bool isNew: true
	readonly property bool hasValidUserId: container.userData && container.userData.m_id && container.userData.m_id !== ""
	readonly property string contextEntityDisplayName: container.userData
		? (container.userData.m_name || container.userData.m_username || container.userData.m_id)
		: ""
	
	function updateGui(){
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		var rolesPageInstance = multiPageView.getPageById("Roles")
		if (rolesPageInstance)
			rolesPageInstance.updateGui()
		var groupsPageInstance = multiPageView.getPageById("Groups")
		if (groupsPageInstance)
			groupsPageInstance.updateGui()
		var permissionsPageInstance = multiPageView.getPageById("Permissions")
		if (permissionsPageInstance)
			permissionsPageInstance.updateGui()
	
		container.updateBadges()
	}
	
	function updateModel(){
		if (!container.userData){
			return
		}

		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
		var rolesPageInstance = multiPageView.getPageById("Roles")
		if (rolesPageInstance)
			rolesPageInstance.updateModel()
		var groupsPageInstance = multiPageView.getPageById("Groups")
		if (groupsPageInstance)
			groupsPageInstance.updateModel()
		userData.m_productId = container.productId;
	
		container.updateBadges()
	}
	
	// Emitted after a change of the direct assignments that other ones are inherited through.
	signal assignmentsChanged()

	// The inherited assignments are being recalculated.
	property bool assignmentsUpdating: false

	// Shows the recalculated assignment lists on the loaded pages.
	function updateAssignments(){
		container.setBlockingUpdateModel(true)
		var rolesPageInstance = multiPageView.getPageById("Roles")
		if (rolesPageInstance)
			rolesPageInstance.updateGui()
		var groupsPageInstance = multiPageView.getPageById("Groups")
		if (groupsPageInstance)
			groupsPageInstance.updateGui()
		var permissionsPageInstance = multiPageView.getPageById("Permissions")
		if (permissionsPageInstance)
			permissionsPageInstance.updateGui()
		container.updateBadges()
		container.setBlockingUpdateModel(false)
	}

	// Item counts next to the page names.
	function updateBadges(){
		if (!container.userData){
			return
		}

		multiPageView.setPageBadge("Roles", String(container.userData.m_roles ? container.userData.m_roles.count : 0))
		multiPageView.setPageBadge("Groups", String(container.userData.m_groups ? container.userData.m_groups.count : 0))
		multiPageView.setPageBadge("Permissions", String(container.userData.m_permissions ? container.userData.m_permissions.count : 0))
	}

	function getHeaders(){
		return {}
	}
	
	onIsNewChanged: {
		checkChangePasswordLogic();
	}
	
	onUserDataChanged: {
		if (!userData){
			return;
		}

		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.handleUserDataChanged()
	}
	
	function checkChangePasswordLogic(){
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.checkChangePasswordLogic()
	}
	
	function checkSystemId(){
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.checkSystemId()
	}

	MultiPageView {
		id: multiPageView
		objectName: "UserEditorPages"
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			// Labelled by what they are *for this user*, so they don't read as the
			// identically-named top-level Roles/Groups collections in
			// AdministrationView. Page ids stay untouched, they are the API.
			multiPageView.addPage("Roles", qsTr("User Roles"), rolesPageComp, "Icons/Role")
			multiPageView.addPage("Groups", qsTr("Group Membership"), groupsPageComp, "Icons/Organization")
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

			property alias passwordInput: userGeneralEditor.passwordInput;
			property alias passwordInputConfirm: userGeneralEditor.confirmPasswordInput;

			function updateGui(){
				userGeneralEditor.updateGui();
				systemInfoGroup.updateGui();
			}

			function updateModel(){
				userGeneralEditor.updateModel();
				systemInfoGroup.updateModel();
			}

			function handleUserDataChanged(){
				container.setBlockingUpdateModel(true);

				let ok = false
				if (systemInfoTable.table && container.userData.hasSystemInfos()){
					let systemInfosModel = container.userData.m_systemInfos;
					systemInfoTable.table.elements = systemInfosModel;
					ok = systemInfoTable.table.elementsCount > 1
				}

				if (!ok){
					headerSystemInfoGroup.visible = false;
					systemInfoGroup.visible = false;
				}

				generalPage.checkChangePasswordLogic();

				generalPage.checkSystemId();

				container.setBlockingUpdateModel(false);
			}

			function checkChangePasswordLogic(){
				if (!container.userData){
					return;
				}

				userGeneralEditor.passwordInput.visible = container.isNew;
				userGeneralEditor.changePasswordButton.visible = !container.isNew;
			}

			function checkSystemId(){
				if (!container.userData){
					console.error("Unable to check system ID for the user. Error: UserData is invalid");
					return;
				}

				if (!container.userData.hasSystemInfos()){
					return;
				}

				userGeneralEditor.usernameInput.readOnly = false;
				userGeneralEditor.passwordInput.readOnly = false;

				for (let i = 0; i < container.userData.m_systemInfos.count; i++){
					let systemId = container.userData.m_systemInfos.get(i).item.m_id;
					if (systemId !== ""){
						userGeneralEditor.changePasswordButton.visible = false;
						userGeneralEditor.usernameInput.readOnly = true;
					}
					
					let enabled = container.userData.m_systemInfos.get(i).item.m_enabled;
					if (enabled && systemId !== ""){
						userGeneralEditor.usernameInput.readOnly = true;
						userGeneralEditor.passwordInput.readOnly = true;
						userGeneralEditor.changePasswordButton.visible = false;
					}
					else if (enabled && systemId === ""){
						userGeneralEditor.passwordInput.readOnly = false;
					}
				}
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
					
					spacing: Style.marginXL;
					
					GroupHeaderView {
						id: headerGeneralGroup;
						width: parent.width;
						title: qsTr("General");
					}

					UserGeneralEditor {
						id: userGeneralEditor;
						width: parent.width;
						userData: container.userData;
						showAccountEnabled: true;
						passwordPolicy: container.passwordPolicy;
						
						onEmitUpdateModel: {
							container.doUpdateModel();
						}
						
						onEmitUpdateGui: {
							container.doUpdateGui();
						}
					}
					
					GroupHeaderView {
						id: headerSystemInfoGroup;
						width: parent.width;
						
						title: qsTr("System Information");
						groupView: systemInfoGroup;
					}
					
					GroupElementView {
						id: systemInfoGroup;
						width: parent.width;
						
						TableElementView {
							id: systemInfoTable;
							TreeItemModel {
								id: headersModel2;
								
								Component.onCompleted: {
									updateModel();
								}
								
								function updateModel(){
									headersModel2.clear();
									
									let index = headersModel2.insertNewItem();
									headersModel2.setData("id", "name", index)
									headersModel2.setData("name", qsTr("System Name"), index)
									
									if (systemInfoTable.table){
										systemInfoTable.table.headers = headersModel2;
									}
								}
							}
							
							onTableChanged: {
								if (table){
									table.checkable = true;
									table.isMultiCheckable = false;
								}
							}
							
							Connections {
								id: systemInfoTableConn;
								target: systemInfoTable.table;
								
								function onCheckedItemsChanged(){
									if (systemInfoGroup.block){
										return;
									}
									
									let indexes = systemInfoTable.table.getCheckedItems();
									if (indexes.length === 0){
										systemInfoGroup.block = true;
										systemInfoTable.table.checkItem(0);
										systemInfoGroup.block = false;
									}
									
									container.doUpdateModel();
									container.checkSystemId();
								}
							}
						}
						
						property bool block: false;
						
						function updateGui(){
							if (!container.userData){
								return;
							}
							
							if (!container.userData.hasSystemInfos()){
								return;
							}
							
							if (systemInfoTable.table){
								systemInfoTable.table.uncheckAll();
								let systemInfosModel = container.userData.m_systemInfos;
								if (systemInfosModel){
									for (let i = 0; i < systemInfosModel.count; i++){
										let enabled = systemInfosModel.get(i).item.m_enabled;
										if (enabled){
											systemInfoTable.table.checkItem(i);
										}
									}
								}
							}
						}
						
						function updateModel(){
							if (!container.userData){
								return;
							}
							
							let indexes = systemInfoTable.table.getCheckedItems();
							
							if (container.userData.m_systemInfos){
								for (let i = 0; i < container.userData.m_systemInfos.count; i++){
									container.userData.m_systemInfos.get(i).item.m_enabled = indexes.includes(i)
								}
							}
						}
					}
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
				if (!container.userData){
					return
				}

				rolesTable.loadAssignments(container.userData.m_roles)
			}

			function updateModel(){
				if (!container.userData){
					return
				}

				if (!container.userData.hasRoles()){
					container.userData.emplaceRoles()
				}

				var added = rolesTable.syncAssignments(container.userData.m_roles)
				for (var i = 0; i < added.length; i++){
					var assignment = container.userData.createRolesArrayElement()
					assignment.m_id = added[i].id
					assignment.m_name = added[i].title
					assignment.m_direct = true
					container.userData.m_roles.addElement(assignment)
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
		id: groupsPageComp

		Item {
			id: groupsPage
			anchors.fill: parent

			function updateGui(){
				if (!container.userData){
					return
				}

				groupsTable.loadAssignments(container.userData.m_groups)
			}

			function updateModel(){
				if (!container.userData){
					return
				}

				if (!container.userData.hasGroups()){
					container.userData.emplaceGroups()
				}

				var added = groupsTable.syncAssignments(container.userData.m_groups)
				for (var i = 0; i < added.length; i++){
					var assignment = container.userData.createGroupsArrayElement()
					assignment.m_id = added[i].id
					assignment.m_name = added[i].title
					assignment.m_direct = true
					container.userData.m_groups.addElement(assignment)
				}
			}

			Component.onCompleted: {
				groupsPage.updateGui();
			}

			Column {
				id: groupsHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (groupsPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, groupsPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Groups") + " (" + groupsTable.itemsCount + ")" + (container.assignmentsUpdating ? "   " + qsTr("Updating...") : "")
					controlComp: Component {
						Row {
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginL

							Text {
								objectName: "RemoveGroupLink"
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Remove") + " (" + groupsTable.selectedCount + ")"
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: groupsTable.selectedCount > 0 ? Style.linkColor : Style.inactiveTextColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									enabled: groupsTable.selectedCount > 0
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										groupsTable.removeSelected()
									}
								}
							}

							Text {
								id: groupsTableAddLink
								objectName: "AddGroupLink"
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Add Group")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										groupSelectableCollectionEditor.items = groupsTable.directItems()
										groupSelectableCollectionEditor.openSelector(groupsTableAddLink)
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
					label: qsTr("Groups")
					addButtonText: qsTr("Add Group")
					visible: false

					onSelectionChanged: {
						groupsTable.setDirectItems(groupSelectableCollectionEditor.items)
						container.doUpdateModel()
						container.assignmentsChanged()
					}
				}
			}

			AssignmentsTable {
				id: groupsTable
				anchors.top: groupsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: groupsHeader.x
				width: groupsHeader.width
				editable: true
				navigationPath: "Administration/Groups/Group/"
				nameTitle: qsTr("Group")
				emptyText: qsTr("The user is not a member of any group.")
				filterPlaceholder: qsTr("Filter groups...")


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
			permissions: container.userData ? container.userData.m_permissions : null
			warnings: container.userData ? container.userData.m_accessWarnings : null
			permissionGroups: container.permissionGroups
			updating: container.assignmentsUpdating
			emptyText: qsTr("The user gets no permissions from roles or groups.")
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
				documentId: container.userData ? container.userData.m_id : "";
				collectionId: "Users";

				function getHeaders(){
					return container.getHeaders()
				}
			}
		}
	}
}

