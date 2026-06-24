import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthRolesSdl 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtguigql 1.0

ViewBase {
	id: container;
	
	anchors.fill: parent;
	contentColor: Style.baseColor
	
	property string productId: "";
	property string tenantId: "";
	property var permissionsProvider: null;
	property string __lastRequestedProductId: ""
	property string __lastRequestedTenantId: ""
	
	property RoleData roleData: model;

	Component.onCompleted: {
		container.__requestPermissions(true)
		container.__rebuildPermissionsTree()
	}

	onProductIdChanged: {
		container.__requestPermissions(true)
	}

	onTenantIdChanged: {
		container.__requestPermissions(true)
	}

	onPermissionsProviderChanged: {
		container.__lastRequestedProductId = ""
		container.__lastRequestedTenantId = ""
		container.__requestPermissions(true)
		container.__rebuildPermissionsTree()
	}

	Connections {
		target: container.permissionsProvider

		function onPermissionsReceived(permissions, sourceTenantId) {
			var expectedTenantId = container.tenantId || ""
			var actualTenantId = sourceTenantId || ""
			if (expectedTenantId !== actualTenantId)
				return
			container.__rebuildPermissionsTree()
		}
	}

	function __requestPermissions(force) {
		if (!container.permissionsProvider)
			return false

		if (container.productId === "")
			return false

		var requestTenantId = container.tenantId || ""
		if (!force
				&& container.__lastRequestedProductId === container.productId
				&& container.__lastRequestedTenantId === requestTenantId
				&& container.permissionsProvider.loading)
			return true

		container.permissionsProvider.productId = container.productId
		container.__lastRequestedProductId = container.productId
		container.__lastRequestedTenantId = requestTenantId
		container.permissionsProvider.requestPermissions(requestTenantId)
		return true
	}

	function __activePermissions() {
		if (!container.permissionsProvider)
			return []

		var requestTenantId = container.tenantId || ""
		if (requestTenantId !== "") {
			if (container.permissionsProvider.tenantPermissionsTenantId !== requestTenantId)
				return []
			return container.permissionsProvider.tenantPermissions || []
		}

		return container.permissionsProvider.allPermissions || []
	}

	function __rebuildPermissionsTree() {
		if (!permissionsTableElementView.bottomItem)
			return
		permissionsTableElementView.bottomItem.rebuildFromFlatArray(container.__activePermissions())
		if (container.roleData)
			container.doUpdateGuiPermissions()
	}

	function updateGui(){
		generalGroup.updateGui();
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
		
		generalGroup.updateModel();
		container.doUpdateModelPermissions()
		
		roleData.m_productId = container.productId;
	}

	DocumentHistoryPanel {
		id: historyPanel;
		documentId: container.roleData ? container.roleData.m_id : "";
		collectionId: "Roles";
		editorFlickable: flickable;
		
		function getHeaders(){
			return container.getHeaders();
		}
	}
	
	function getHeaders(){
		return {};
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
	
	CustomScrollbar{
		id: scrollHoriz;
		z: parent.z + 1;
		anchors.left: flickable.left;
		anchors.right: flickable.right;
		anchors.bottom: flickable.bottom;
		secondSize: Style.marginM;
		vertical: false;
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
		contentWidth: bodyColumn.width;
		contentHeight: Math.max(bodyColumn.height + 2 * Style.marginXL, historyPanel.contentHeight + 2 * Style.marginXL);
		
		boundsBehavior: Flickable.StopAtBounds;
		clip: true;
		
		Column {
			id: bodyColumn;
			width: Style.sizeHintXXL;
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
					KeyNavigation.backtab: permissionsGroupElement;
				}
				
				TextInputElementView {
					id: roleIdInput;
					
					readOnly: true;
					
					name: qsTr("Role-ID");
					
					KeyNavigation.tab: descriptionInput;
					KeyNavigation.backtab: roleNameInput;
				}
				
				TextInputElementView {
					id: descriptionInput;
					
					name: qsTr("Description");
					placeHolderText: qsTr("Enter the description");
					
					onEditingFinished: {
						let oldText = container.roleData.m_description;
						if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
							container.doUpdateModel();
						}
					}
					
					KeyNavigation.tab: roleSelectableCollectionEditor;
					KeyNavigation.backtab: roleIdInput;
				}
				
				GqlBasedItemSelectElementView {
					id: roleSelectableCollectionEditor
					collectionId: "Roles"
					label: qsTr("Parent Roles")
					addButtonText: qsTr("Add Parent Role")
					showCount: true
					onSelectionChanged: {
						container.doUpdateModel()
					}
				}

				function updateGui(){
					if (!container.roleData){
						return
					}

					roleIdInput.text = container.roleData.m_roleId;
					roleNameInput.text = container.roleData.m_name;
					descriptionInput.text = container.roleData.m_description;
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

					container.roleData.m_roleId = roleIdInput.text;
					container.roleData.m_name = roleNameInput.text;
					container.roleData.m_description = descriptionInput.text;
					var arr = []
					for (var i = 0; i < roleSelectableCollectionEditor.items.length; i++)
						arr.push(roleSelectableCollectionEditor.items[i].id)
					container.roleData.m_parentRoles = arr
				}
			}
			
			GroupHeaderView {
				width: parent.width;
				title: qsTr("Permissions");
			}
			
			GroupElementView {
				id: permissionsGroupElement
				width: parent.width
				
				ElementView {
					id: permissionsTableElementView
					width: parent.width
					bottomComp: Component {
						PermissionsTableView {
							id: permissionsTableView
							height: 500
							anchors.horizontalCenter: parent.horizontalCenter
							showControlPanel: true
							treeToScrollbarSpacing: 0
		
							onSelectionChanged: {
								container.doUpdateModel()
							}
						}
					}
				}
			}
		}
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

		if (permissionsTableElementView.bottomItem){
			permissionsTableElementView.bottomItem.applySelection(selectedPermissionsIds)
		}
	}
	
	function doUpdateModelPermissions() {
		if (!container.roleData){
			return
		}

		if (permissionsTableElementView.bottomItem){
			var selectedPermissionIds = permissionsTableElementView.bottomItem.getCheckedIds()
			container.roleData.m_permissions = selectedPermissionIds.join(';')
		}
	}
}
