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
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			// Labelled by what they are *for this user*, so they don't read as the
			// identically-named top-level Roles/Groups collections in
			// AdministrationView. Page ids stay untouched, they are the API.
			multiPageView.addPage("Roles", qsTr("Assigned Roles"), rolesPageComp, "Icons/Role")
			multiPageView.addPage("Groups", qsTr("Group Membership"), groupsPageComp, "Icons/Organization")
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
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Assigned Roles");
						groupView: rolesGroup;
					}

					GroupElementView {
						id: rolesGroup;

						width: parent.width;

						CollectionItemSelectElementView {
							id: roleSelectableCollectionEditor
							commandId: ImtauthRolesSdlCommandIds.s_rolesList
							fields: [RoleItemDataTypeMetaInfo.s_id, RoleItemDataTypeMetaInfo.s_roleName]
							titleField: RoleItemDataTypeMetaInfo.s_roleName
							textFilterFieldIds: [RoleItemDataTypeMetaInfo.s_roleName]
							sortByField: RoleItemDataTypeMetaInfo.s_roleName
							label: qsTr("Roles")
							addButtonText: qsTr("Add Role")
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
							if (!container.userData){
								return
							}

							var ids = container.userData.m_roles ? container.userData.m_roles.slice() : []
							var arr = []
							for (var i = 0; i < ids.length; i++)
								arr.push({id: ids[i], name: ids[i]})
							roleSelectableCollectionEditor.items = arr
						}
						
						function updateModel(){
							if (!container.userData){
								return
							}

							var arr = []
							for (var i = 0; i < roleSelectableCollectionEditor.items.length; i++)
								arr.push(roleSelectableCollectionEditor.items[i].id)
							container.userData.m_roles = arr
						}
					}
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
				groupsBlock.updateGui();
			}

			function updateModel(){
				groupsBlock.updateModel();
			}

			Component.onCompleted: {
				groupsPage.updateGui();
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
						width: parent.width;
						
						title: qsTr("Group Membership");
						groupView: groupsBlock;
					}
					
					GroupElementView {
						id: groupsBlock;
						
						width: parent.width;

						CollectionItemSelectElementView {
							id: groupSelectableCollectionEditor
							commandId: ImtauthGroupsSdlCommandIds.s_groupsList
							fields: [GroupItemDataTypeMetaInfo.s_id, GroupItemDataTypeMetaInfo.s_name]
							titleField: GroupItemDataTypeMetaInfo.s_name
							textFilterFieldIds: [GroupItemDataTypeMetaInfo.s_name]
							sortByField: GroupItemDataTypeMetaInfo.s_name
							label: qsTr("Groups")
							addButtonText: qsTr("Add Group")
							showCount: true

							onSelectionChanged: {
								container.doUpdateModel()
							}
						}

						function updateGui(){
							if (!container.userData){
								return
							}

							var ids = container.userData.m_groups ? container.userData.m_groups.slice() : []
							var arr = []
							for (var i = 0; i < ids.length; i++)
								arr.push({id: ids[i], name: ids[i]})
							groupSelectableCollectionEditor.items = arr
						}
						
						function updateModel(){
							if (!container.userData){
								return
							}

							var arr = []
							for (var i = 0; i < groupSelectableCollectionEditor.items.length; i++)
								arr.push(groupSelectableCollectionEditor.items[i].id)
							container.userData.m_groups = arr
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
				documentId: container.userData ? container.userData.m_id : "";
				collectionId: "Users";

				function getHeaders(){
					return container.getHeaders()
				}
			}
		}
	}
}

