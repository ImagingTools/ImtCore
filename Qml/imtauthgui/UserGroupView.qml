import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthGroupsSdl 1.0
import imtdocgui 1.0

ViewBase {
	id: container;
	contentColor: Style.baseColor
	property GroupData groupData: model;
	property string productId;

	function updateGui(){
		generalGroup.updateGui();
		usersGroup.updateGui();
		rolesGroup.updateGui();
	}
	
	function updateModel(){
		if (!container.groupData){
			return
		}

		generalGroup.updateModel();
		usersGroup.updateModel();
		rolesGroup.updateModel();
		
		groupData.m_productId = productId;
	}
	
	function getHeaders(){
		return {}
	}
	
	DocumentHistoryPanel {
		id: historyPanel;
		documentId: container.groupData ? container.groupData.m_id : "";
		collectionId: "Groups";
		editorFlickable: flickable;
		
		function getHeaders(){
			return container.getHeaders()
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
	
	CustomScrollbar{
		id: scrollHoriz;
		
		z: parent.z + 1;
		
		anchors.left: flickable.left;
		anchors.right: flickable.right;
		anchors.bottom: flickable.bottom;
		
		secondSize: 10;
		
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
					
					name: qsTr("Group Name");
					placeHolderText: qsTr("Enter the name");
					
					onEditingFinished: {
						container.doUpdateModel();
					}
					
					KeyNavigation.tab: descriptionInput;
					KeyNavigation.backtab: roleSelectableCollectionEditor;
				}
				
				TextInputElementView {
					id: descriptionInput;
					
					name: qsTr("Description");
					placeHolderText: qsTr("Enter the description");
					
					onEditingFinished: {
						container.doUpdateModel();
					}
					
					KeyNavigation.tab: groupSelectableCollectionEditor;
					KeyNavigation.backtab: nameInput;
				}
				
				GqlBasedItemSelectElementView {
					id: groupSelectableCollectionEditor
					collectionId: "Groups"
						label: qsTr("Parent Groups")
						addButtonText: qsTr("Add Parent Group")
						showCount: true
						onSelectionChanged: {
							container.doUpdateModel()
						}
					}

				function updateGui(){
					if (!container.groupData){
						return
					}
					nameInput.text = container.groupData.m_name;
					descriptionInput.text = container.groupData.m_description;
					var ids = container.groupData.m_parentGroups ? container.groupData.m_parentGroups.slice() : []
					var arr = []
					for (var i = 0; i < ids.length; i++)
						arr.push({id: ids[i], name: ids[i]})
					groupSelectableCollectionEditor.items = arr
				}
				
				function updateModel(){
					if (!container.groupData){
						return
					}
					container.groupData.m_description = descriptionInput.text;
					container.groupData.m_name = nameInput.text;
					var arr = []
					for (var i = 0; i < groupSelectableCollectionEditor.items.length; i++)
						arr.push(groupSelectableCollectionEditor.items[i].id)
					container.groupData.m_parentGroups = arr
				}
			}
			
			GroupElementView {
				id: usersGroup;
				
				width: parent.width;
				
				GqlBasedItemSelectElementView {
					id: userSelectableCollectionEditor
					collectionId: "Users"
						label: qsTr("Users")
						addButtonText: qsTr("Add User")
						showCount: true
						onSelectionChanged: {
							container.doUpdateModel()
						}
					}
				
				function updateGui(){
					if (!container.groupData){
						return
					}
					var ids = container.groupData.m_users ? container.groupData.m_users.slice() : []
					var arr = []
					for (var i = 0; i < ids.length; i++)
						arr.push({id: ids[i], name: ids[i]})
					userSelectableCollectionEditor.items = arr
				}
				
				function updateModel(){
					if (!container.groupData){
						return
					}
					var arr = []
					for (var i = 0; i < userSelectableCollectionEditor.items.length; i++)
						arr.push(userSelectableCollectionEditor.items[i].id)
					container.groupData.m_users = arr
				}
			}

			GroupElementView {
				id: rolesGroup;
				
				width: parent.width;
				
				GqlBasedItemSelectElementView {
					id: roleSelectableCollectionEditor
					collectionId: "Roles"
						label: qsTr("Roles")
						addButtonText: qsTr("Add Role")
						showCount: true
						onSelectionChanged: {
							container.doUpdateModel()
						}
					}

				function updateGui(){
					if (!container.groupData){
						return
					}
					var ids = container.groupData.m_roles ? container.groupData.m_roles.slice() : []
					var arr = []
					for (var i = 0; i < ids.length; i++)
						arr.push({id: ids[i], name: ids[i]})
					roleSelectableCollectionEditor.items = arr
				}
				
				function updateModel(){
					if (!container.groupData){
						return
					}
					var arr = []
					for (var i = 0; i < roleSelectableCollectionEditor.items.length; i++)
						arr.push(roleSelectableCollectionEditor.items[i].id)
					container.groupData.m_roles = arr
				}
			}
		}
	}
}


