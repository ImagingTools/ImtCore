import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthGroupsSdl 1.0
import imtdocgui 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

ViewBase {
	id: container;

	property GroupData groupData: model;
	property string productId;

	function updateGui(){
		generalGroup.updateGui();
		usersGroup.updateGui();
		rolesGroup.updateGui();
	}
	
	function updateModel(){
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
				
				SelectableCollectionEditor {
					id: groupSelectableCollectionEditor
					collectionId: "Groups"
					targetTitle: qsTr("Parent Groups")
					sourceTitle: qsTr("Adding Parent Group")
					onSelectionChanged: {
						container.doUpdateModel()
					}
					
					Component {
						id: fieldFilterComp
						FieldFilter {
							m_fieldId: "DocumentId"
							m_filterValueType: "String"
							m_filterValue: !container.groupData ? "" : container.groupData.m_id
							m_filterOperations: ["Not", "Equal"]
						}
					}
					
					function setSourceAdditionalFilters(collection){
						let fieldFilter = fieldFilterComp.createObject(collection.collectionFilter)
						collection.collectionFilter.addFieldFilter(fieldFilter)
					}
				}

				function updateGui(){
					nameInput.text = container.groupData.m_name;
					descriptionInput.text = container.groupData.m_description;
					groupSelectableCollectionEditor.selectedIds = container.groupData.m_parentGroups.slice()
				}
				
				function updateModel(){
					container.groupData.m_description = descriptionInput.text;
					container.groupData.m_name = nameInput.text;
					container.groupData.m_parentGroups = groupSelectableCollectionEditor.selectedIds.slice()
				}
			}
			
			GroupElementView {
				id: usersGroup;
				
				width: parent.width;
				
				SelectableCollectionEditor {
					id: userSelectableCollectionEditor
					collectionId: "Users"
					targetTitle: qsTr("Users")
					sourceTitle: qsTr("Adding Users")
					targetRequestedFields: ["id", "name"]
					onSelectionChanged: {
						container.doUpdateModel()
					}
				}
				
				function updateGui(){
					userSelectableCollectionEditor.selectedIds = container.groupData.m_users.slice()
				}
				
				function updateModel(){
					container.groupData.m_users = userSelectableCollectionEditor.selectedIds.slice()
				}
			}

			GroupElementView {
				id: rolesGroup;
				
				width: parent.width;
				
				SelectableCollectionEditor {
					id: roleSelectableCollectionEditor
					collectionId: "Roles"
					targetTitle: qsTr("Roles")
					sourceTitle: qsTr("Adding Roles")
					targetRequestedFields: ["roleName", "description"]
					onSelectionChanged: {
						container.doUpdateModel()
					}
				}

				function updateGui(){
					roleSelectableCollectionEditor.selectedIds = container.groupData.m_roles.slice()
				}
				
				function updateModel(){
					container.groupData.m_roles = roleSelectableCollectionEditor.selectedIds.slice()
				}
			}
		}
	}
}


