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

	property bool __permissionsRequested: false
	property var __receivedPermissions: null
	property bool __completed: false

	property RoleData roleData: model;

	onRoleDataChanged: {
		if (!container.roleData)
			return
		if (container.productId !== "")
			container.roleData.m_productId = container.productId;
		generalGroup.applyModelValues();
		container.doUpdateGuiPermissions();
	}

	Component.onCompleted: {
		container.__completed = true
		container.__requestPermissionsOnce()
	}

	onProductIdChanged: {
		if (container.roleData && container.productId !== "")
			container.roleData.m_productId = container.productId;
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

	Connections {
		target: permissionsTableElementView

		function onBottomItemChanged() {
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
		if (!permissionsTableElementView.bottomItem)
			return
		var perms = container.__receivedPermissions
		if (!perms)
			return
		permissionsTableElementView.bottomItem.rebuildFromFlatArray(perms)
		if (container.roleData)
			container.doUpdateGuiPermissions()
	}

	function updateGui(){
		generalGroup.applyModelValues();
		container.doUpdateGuiPermissions()
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

					// Test instrumentation: matches the AccountEditor/DeviceEditor/etc. convention of
					// an explicit per-field objectName on the ElementView usage site (the shared
					// TextInputElementView/CustomTextField components only ever carry the generic
					// "TextField"/"TextInput"). Inert - no runtime/visual effect.
					objectName: "RoleNameInput";

					name: qsTr("Role Name");
					placeHolderText: qsTr("Enter the role name");
					
					text: container.roleData ? container.roleData.m_name : "";
					onEditingFinished: {
						if (!container.roleData){
							return
						}

						if (container.roleData.m_name !== roleNameInput.text){
							container.roleData.m_name = roleNameInput.text;
							container.roleData.m_roleId = roleNameInput.text.replace(/\s+/g, '');
						}
					}
					
					KeyNavigation.tab: roleIdInput;
					KeyNavigation.backtab: permissionsGroupElement;
				}
				
				TextInputElementView {
					id: roleIdInput;

					// Test instrumentation - see roleNameInput's comment above. Inert.
					objectName: "RoleIdInput";

					readOnly: true;

					name: qsTr("Role-ID");
					
					text: container.roleData ? container.roleData.m_roleId : "";
					
					KeyNavigation.tab: descriptionInput;
					KeyNavigation.backtab: roleNameInput;
				}
				
				TextInputElementView {
					id: descriptionInput;

					// Test instrumentation - see roleNameInput's comment above. Inert.
					objectName: "RoleDescriptionInput";

					name: qsTr("Description");
					placeHolderText: qsTr("Enter the description");

					text: container.roleData ? container.roleData.m_description : "";
					onEditingFinished: {
						if (container.roleData && container.roleData.m_description !== descriptionInput.text){
							container.roleData.m_description = descriptionInput.text;
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
						generalGroup.writeParentRoles()
					}
				}

				function applyModelValues(){
					if (!container.roleData){
						return
					}

					var ids = container.roleData.m_parentRoles ? container.roleData.m_parentRoles.slice() : []
					var arr = []
					for (var i = 0; i < ids.length; i++)
						arr.push({id: ids[i], name: ids[i]})
					roleSelectableCollectionEditor.items = arr
				}
				
				function writeParentRoles(){
					if (!container.roleData){
						return
					}

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
								container.doUpdateModelPermissions()
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
			// Only leaf permission IDs must be stored (groups/parents are excluded even
			// when tristate check selected the whole subtree).
			var selectedPermissionIds = permissionsTableElementView.bottomItem.getCheckedIds()
			container.roleData.m_permissions = selectedPermissionIds.join(';')
		}
	}
}
