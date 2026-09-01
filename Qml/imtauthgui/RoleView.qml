import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthRolesSdl 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtguigql 1.0

ViewBase {
	id: container;
	
	anchors.fill: parent;
	contentColor: Style.baseColor
	
	property string productId: "";
	property string tenantId: "";
	// Part of the product this view works in; scopes its permission checks.
	property string permissionPath: "";
	property var permissionsProvider: null;

	property bool __permissionsRequested: false
	property var __receivedPermissions: null
	property bool __completed: false

	property RoleData roleData: model;

	Component.onCompleted: {
		container.__completed = true
		container.__requestPermissionsOnce()
	}

	onProductIdChanged: {
		if (!container.__completed)
			return
		container.__permissionsRequested = false
		container.__receivedPermissions = null
		container.__requestPermissionsOnce()
	}

	onTenantIdChanged: {
		if (!container.__completed)
			return
		container.__permissionsRequested = false
		container.__receivedPermissions = null
		container.__requestPermissionsOnce()
	}

	onPermissionsProviderChanged: {
		if (!container.__completed)
			return
		container.__permissionsRequested = false
		container.__receivedPermissions = null
		container.__requestPermissionsOnce()
	}

	Connections {
		target: container.permissionsProvider

		function onPermissionsReceived(permissions, sourceTenantId) {
			var expectedTenantId = container.tenantId || ""
			var actualTenantId = sourceTenantId || ""
			if (expectedTenantId !== actualTenantId)
				return
			container.__receivedPermissions = permissions
			container.__populatePermissionsTree()
		}
	}

	function __requestPermissionsOnce() {
		if (!container.permissionsProvider)
			return
		if (container.productId === "")
			return
		if (container.__permissionsRequested)
			return
		container.__permissionsRequested = true
		container.permissionsProvider.productId = container.productId
		var requestTenantId = container.tenantId || ""
		container.permissionsProvider.requestPermissions(requestTenantId)
	}

	function __populatePermissionsTree() {
		var permissionPageInstance = multiPageView.getPageById("Permission")
		if (!permissionPageInstance || !permissionPageInstance.bottomItem)
			return
		var perms = container.__receivedPermissions
		if (!perms)
			return
		permissionPageInstance.bottomItem.rebuildFromFlatArray(perms)
		if (container.roleData)
			container.doUpdateGuiPermissions()
	}

	function updateGui(){
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		var parentRolesPageInstance = multiPageView.getPageById("ParentRoles")
		if (parentRolesPageInstance)
			parentRolesPageInstance.updateGui()
		container.doUpdateGuiPermissions()
	}
	
	function updateModel(){
		if (!container.roleData){
			return
		}

		if (container.productId === ""){
			console.error("Unable to update a role model. Product-ID is empty");
			return;
		}
		
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
		var parentRolesPageInstance = multiPageView.getPageById("ParentRoles")
		if (parentRolesPageInstance)
			parentRolesPageInstance.updateModel()
		container.doUpdateModelPermissions()
		
		roleData.m_productId = container.productId;
	}
	
	function getHeaders(){
		return {};
	}

	function doUpdateGuiPermissions() {
		if (!container.roleData){
			return
		}

		var selectedPermissionsIds = [];
		var selectedPermissions = container.roleData.m_permissions;
		if (selectedPermissions !== ""){
			selectedPermissionsIds = selectedPermissions.split(';');
		}

		var permissionPageInstance = multiPageView.getPageById("Permission")
		if (permissionPageInstance && permissionPageInstance.bottomItem){
			permissionPageInstance.bottomItem.applySelection(selectedPermissionsIds)
		}
	}
	
	function doUpdateModelPermissions() {
		if (!container.roleData){
			return
		}

		var permissionPageInstance = multiPageView.getPageById("Permission")
		if (permissionPageInstance && permissionPageInstance.bottomItem){
			// Only leaf permission IDs must be stored (groups/parents are excluded even
			// when tristate check selected the whole subtree).
			var selectedPermissionIds = permissionPageInstance.bottomItem.getCheckedIds()
			container.roleData.m_permissions = selectedPermissionIds.join(';')
		}
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage("ParentRoles", qsTr("Parent Roles"), parentRolesPageComp, "Icons/Role")
			multiPageView.addPage("Permission", qsTr("Permissions"), permissionPageComp, "Icons/Key")
			if (PermissionsController.checkPermission("ViewRevisions")){
				multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
			}
			multiPageView.currentIndex = 0
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
				secondSize: Style.marginM;
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
						width: parent.width;
						title: qsTr("General");
						groupView: generalGroup;
					}
					
					GroupElementView {
						id: generalGroup;
						width: parent.width;
						
						TextInputElementView {
							id: roleNameInput;

							// Test instrumentation: matches the AccountEditor/DeviceEditor/etc. convention of
							// an explicit per-field objectName on the ElementView usage site (the shared
							// TextInputElementView/CustomTextField components only ever carry the generic
							// "TextField"/"TextInput"). Inert - no runtime/visual effect.
							objectName: "RoleNameInput";

							name: qsTr("Role Name");
							placeHolderText: qsTr("Enter the role name");
							
							onEditingFinished: {
								if (!container.roleData){
									return
								}

								let oldText = container.roleData.m_name;
								if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
									roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
									container.doUpdateModel();
								}
							}
							
							KeyNavigation.tab: roleIdInput;
						}
						
						TextInputElementView {
							id: roleIdInput;

							// Test instrumentation - see roleNameInput's comment above. Inert.
							objectName: "RoleIdInput";

							readOnly: true;

							name: qsTr("Role-ID");
							
							KeyNavigation.tab: descriptionInput;
							KeyNavigation.backtab: roleNameInput;
						}
						
						TextInputElementView {
							id: descriptionInput;

							// Test instrumentation - see roleNameInput's comment above. Inert.
							objectName: "RoleDescriptionInput";

							name: qsTr("Description");
							placeHolderText: qsTr("Enter the description");

							onEditingFinished: {
								let oldText = container.roleData.m_description;
								if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
									container.doUpdateModel();
								}
							}
							
							KeyNavigation.backtab: roleIdInput;
						}

						function updateGui(){
							if (!container.roleData){
								return
							}

							roleIdInput.text = container.roleData.m_roleId;
							roleNameInput.text = container.roleData.m_name;
							descriptionInput.text = container.roleData.m_description;
						}
						
						function updateModel(){
							if (!container.roleData){
								return
							}

							container.roleData.m_roleId = roleIdInput.text;
							container.roleData.m_name = roleNameInput.text;
							container.roleData.m_description = descriptionInput.text;
						}
					}
				}
			}
		}
	}

	Component {
		id: parentRolesPageComp

		Item {
			id: parentRolesPage
			anchors.fill: parent

			function updateGui(){
				parentRolesGroup.updateGui();
			}

			function updateModel(){
				parentRolesGroup.updateModel();
			}

			Component.onCompleted: {
				parentRolesPage.updateGui();
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
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
						width: parent.width;
						title: qsTr("Parent Roles");
						groupView: parentRolesGroup;
					}

					GroupElementView {
						id: parentRolesGroup;
						width: parent.width;

						CollectionItemSelectElementView {
							id: roleSelectableCollectionEditor
							commandId: ImtauthRolesSdlCommandIds.s_rolesList
							fields: [RoleItemDataTypeMetaInfo.s_id, RoleItemDataTypeMetaInfo.s_roleName]
							titleField: RoleItemDataTypeMetaInfo.s_roleName
							textFilterFieldIds: [RoleItemDataTypeMetaInfo.s_roleName]
							sortByField: RoleItemDataTypeMetaInfo.s_roleName
							label: qsTr("Parent Roles")
							addButtonText: qsTr("Add Parent Role")
							// A role cannot be its own parent, so it is not offered at all.
							// Deeper cycles are rejected by the server on save.
							excludeIds: container.roleData && container.roleData.m_id
								? [container.roleData.m_id]
								: []
							showCount: true

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
								container.doUpdateModel()
							}
						}

						function updateGui(){
							if (!container.roleData){
								return
							}

							var ids = container.roleData.m_parentRoles ? container.roleData.m_parentRoles.slice() : []
							var arr = []
							for (var i = 0; i < ids.length; i++)
								arr.push({id: ids[i], name: ids[i]})
							roleSelectableCollectionEditor.items = arr
						}

						function updateModel(){
							if (!container.roleData){
								return
							}

							var arr = []
							for (var i = 0; i < roleSelectableCollectionEditor.items.length; i++)
								arr.push(roleSelectableCollectionEditor.items[i].id)
							container.roleData.m_parentRoles = arr
						}
					}
				}
			}
		}
	}

	Component {
		id: permissionPageComp

		Item {
			id: permissionPage
			anchors.fill: parent

			property alias bottomItem: permissionsTableElementView.bottomItem

			// Deferred: multiPageView.getPageById() needs the Loader to have
			// published itemAt().item, which isn't true yet during this page's own completion.
			Timer {
				id: populatePermissionsTimer
				interval: 0
				repeat: false
				onTriggered: container.__populatePermissionsTree()
			}

			Connections {
				target: permissionsTableElementView

				function onBottomItemChanged() {
					populatePermissionsTimer.restart()
				}
			}

			Component.onCompleted: {
				populatePermissionsTimer.restart();
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
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
						width: parent.width;
						title: qsTr("Permissions");
						groupView: permissionsGroupElement;
					}

					GroupElementView {
						id: permissionsGroupElement
						width: parent.width
						
						ElementView {
							id: permissionsTableElementView
							width: parent.width
							border.width: 0
							color: "transparent"
							radius: 0
							contentMargin: Style.marginL
							contentSpacing: 0
							height: contentHeight
							bottomComp: Component {
								PermissionsTableView {
									id: permissionsTableView
									width: parent.width
									height: preferredHeight
									showControlPanel: true
									treeToScrollbarSpacing: 0
									controlPanelTopMargin: Style.marginL
									treeTopMargin: Style.marginL
									treeBottomMargin: Style.marginL
				
									onSelectionChanged: {
										container.doUpdateModel()
									}
								}
							}
						}
					}
				}
			}
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
				documentId: container.roleData ? container.roleData.m_id : "";
				collectionId: "Roles";

				function getHeaders(){
					return container.getHeaders();
				}
			}
		}
	}
}
