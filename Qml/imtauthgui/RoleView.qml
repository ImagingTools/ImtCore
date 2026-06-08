import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthRolesSdl 1.0
import imtdocgui 1.0
import imtguigql 1.0

ViewBase {
	id: container;
	
	anchors.fill: parent;
	contentColor: Style.baseColor
	
	property TreeItemModel permissionsModel: TreeItemModel {};
	onPermissionsModelChanged: {
		permissionsGroup.buildPermissionsModel()
		// After the permissions tree is rebuilt (e.g. fetchPermissions completes
		// asynchronously after the role has been loaded), re-apply the checked
		// state from the current roleData; otherwise the tree appears empty and
		// a subsequent Save would clear all permissions.
		if (container.roleData){
			container.doUpdateGui()
		}
	}
	
	property string productId: "";
	
	property RoleData roleData: model;
	
	function updateGui(){
		generalGroup.updateGui();
		permissionsGroup.updateGui()
	}
	
	function updateModel(){
		if (container.productId === ""){
			console.error("Unable to update a role model. Product-ID is empty");
			return;
		}
		
		generalGroup.updateModel();
		permissionsGroup.updateModel()
		
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
						let oldText = container.roleData.m_name;
						if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
							roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
							container.doUpdateModel();
						}
					}
					
					KeyNavigation.tab: roleIdInput;
					KeyNavigation.backtab: permissionsGroup;
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
				groupView: group;
			}
			
			GroupElementView {
				id: group;
				
				width: parent.width;
				
				TreeViewElementView {
					id: permissionsGroup;
					
					KeyNavigation.tab: roleNameInput;
					KeyNavigation.backtab: roleSelectableCollectionEditor;
					
					Component.onCompleted: {
						permissionsGroup.treeView.tristate = true;
					}
					
					function buildPermissionsModel() {
						if (!container.permissionsModel)
							return;
						
						var nodes = TreeModelBuilder.fromTreeItemModel(
							container.permissionsModel,
							function(wrapper, index) {
								return {
									key: wrapper.data("FeatureId", ""),
									text: wrapper.data("FeatureName", ""),
									checkable: true,
									expanded: true,
									data: {
										FeatureId: wrapper.data("FeatureId", ""),
										FeatureName: wrapper.data("FeatureName", "")
									}
								};
							},
							function(wrapper, index) {
								return wrapper.childModel("ChildModel");
							}
						);
						
						permissionsGroup.treeView.model = nodes;
					}
					
					function updateGui(){
						let selectedPermissionsIds = [];
						let selectedPermissions = container.roleData.m_permissions;
						if (selectedPermissions !== ""){
							selectedPermissionsIds = selectedPermissions.split(';');
						}
						
						selectedPermissionsIds.sort();
						
						permissionsGroup.treeView.uncheckAll();
						
						let allNodesList = permissionsGroup.treeView.allNodes();
						for (let i = 0; i < allNodesList.length; i++){
							let nodeIdx = allNodesList[i];
							let nodeChildren = nodeIdx.item && nodeIdx.item.children ? nodeIdx.item.children : [];
							if (nodeChildren.length === 0){
								let nodeData = nodeIdx.data || {};
								let id = nodeData.FeatureId;
								
								if (selectedPermissionsIds.includes(id)){
									permissionsGroup.treeView.checkItem(nodeIdx.key);
								}
							}
						}
					}
					
					function updateModel(){
						let selectedPermissionIds = [];
						let checkedNodes = permissionsGroup.treeView.getCheckedNodes();
						for (let j = 0; j < checkedNodes.length; j++){
							let nodeIdx = checkedNodes[j];
							let nodeChildren = nodeIdx.item && nodeIdx.item.children ? nodeIdx.item.children : [];
							if (nodeChildren.length === 0){
								let nodeData = nodeIdx.data || {};
								let id = nodeData.FeatureId;
								selectedPermissionIds.push(id);
							}
						}
						
						selectedPermissionIds.sort();
						
						container.roleData.m_permissions = selectedPermissionIds.join(';')
					}
					
					Connections {
						target: permissionsGroup.treeView;
						
						function onCheckedItemsChanged(){
							container.doUpdateModel();
						}
					}
				}
			}
		}
	}
}
