import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthGroupsSdl 1.0
import imtdocgui 1.0
import imtauthgui 1.0

ViewBase {
	id: container;
	contentColor: Style.baseColor
	property GroupData groupData: model;
	property string productId;

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
			multiPageView.addPage("ParentGroups", qsTr("Parent Groups"), parentGroupsPageComp, "Icons/Organization")
			multiPageView.addPage("Users", qsTr("Users"), usersPageComp, "Icons/MultipleUser")
			multiPageView.addPage("Roles", qsTr("Roles"), rolesPageComp, "Icons/Role")
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
				parentGroupsGroup.updateGui();
			}

			function updateModel(){
				parentGroupsGroup.updateModel();
			}

			Component.onCompleted: {
				parentGroupsPage.updateGui();
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
						title: qsTr("Parent Groups");
						groupView: parentGroupsGroup;
					}

					GroupElementView {
						id: parentGroupsGroup;
						width: parent.width;

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
							var arr = []
							for (var i = 0; i < groupSelectableCollectionEditor.items.length; i++)
								arr.push(groupSelectableCollectionEditor.items[i].id)
							container.groupData.m_parentGroups = arr
						}
					}
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
				usersGroup.updateGui();
			}

			function updateModel(){
				usersGroup.updateModel();
			}

			Component.onCompleted: {
				usersPage.updateGui();
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
						title: qsTr("Users");
						groupView: usersGroup;
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
				rolesGroup.updateGui();
			}

			function updateModel(){
				rolesGroup.updateModel();
			}

			Component.onCompleted: {
				rolesPage.updateGui();
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
						title: qsTr("Roles");
						groupView: rolesGroup;
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



