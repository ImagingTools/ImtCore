import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0
import imtdocgui 1.0
import imtguigql 1.0

DocumentViewBase {
	id: container;
	
	anchors.fill: parent;
	contentColor: Style.baseColor

	property UserData userData: model;
	property string productId;
	
	property alias passwordInput: userGeneralEditor.passwordInput;
	property alias passwordInputConfirm: userGeneralEditor.confirmPasswordInput;
	
	property bool isNew: true
	readonly property bool hasValidUserId: container.userData && container.userData.m_id && container.userData.m_id !== ""
	readonly property string contextEntityDisplayName: container.userData
		? (container.userData.m_name || container.userData.m_username || container.userData.m_id)
		: ""
	
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
		
		userData.m_productId = container.productId;
		
		let ok = false
		if (systemInfoTable.table && userData.hasSystemInfos()){
			let systemInfosModel = userData.m_systemInfos;
			systemInfoTable.table.elements = systemInfosModel;
			ok = systemInfoTable.table.elementsCount > 1
		}
		
		if (!ok){
			headerSystemInfoGroup.visible = false;
			systemInfoGroup.visible = false;
		}
		
		systemInfoGroup.applyModelSelection();
		rolesGroup.applyModelSelection();
		groupsBlock.applyModelSelection();
		
		checkChangePasswordLogic();
		
		checkSystemId();
	}
	
	function writeSystemInfoSelection(){
		if (!userData){
			return;
		}
		
		let indexes = systemInfoTable.table.getCheckedItems();
		
		if (userData.m_systemInfos){
			for (let i = 0; i < userData.m_systemInfos.count; i++){
				userData.m_systemInfos.get(i).item.m_enabled = indexes.includes(i)
			}
		}
	}
	
	function checkChangePasswordLogic(){
		if (!userData){
			return;
		}

		userGeneralEditor.passwordInput.visible = isNew;
		userGeneralEditor.changePasswordButton.visible = !isNew;
	}
	
	function checkSystemId(){
		if (!userData){
			console.error("Unable to check system ID for the user. Error: UserData is invalid");
			return;
		}
		
		if (!userData.hasSystemInfos()){
			return;
		}
		
		userGeneralEditor.usernameInput.readOnly = false;
		userGeneralEditor.passwordInput.readOnly = false;

		for (let i = 0; i < userData.m_systemInfos.count; i++){
			let systemId = userData.m_systemInfos.get(i).item.m_id;
			if (systemId !== ""){
				userGeneralEditor.changePasswordButton.visible = false;
				userGeneralEditor.usernameInput.readOnly = true;
			}
			
			let enabled = userData.m_systemInfos.get(i).item.m_enabled;
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
	
	DocumentHistoryPanel {
		id: historyPanel;
		documentId: container.userData ? container.userData.m_id : "";
		collectionId: "Users";
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
							
							container.writeSystemInfoSelection();
							container.checkSystemId();
						}
					}
				}
				
				property bool block: false;
				
				function applyModelSelection(){
					if (!container.userData){
						return;
					}
					
					if (!container.userData.hasSystemInfos()){
						return;
					}
					
					if (systemInfoTable.table){
						systemInfoGroup.block = true;
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
						systemInfoGroup.block = false;
					}
				}
			}
			
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
							rolesGroup.writeModelSelection()
						}
					}

				function applyModelSelection(){
					if (!container.userData){
						return
					}

					var ids = container.userData.m_roles ? container.userData.m_roles.slice() : []
					var arr = []
					for (var i = 0; i < ids.length; i++)
						arr.push({id: ids[i], name: ids[i]})
					roleSelectableCollectionEditor.items = arr
				}
				
				function writeModelSelection(){
					if (!container.userData){
						return
					}

					var arr = []
					for (var i = 0; i < roleSelectableCollectionEditor.items.length; i++)
						arr.push(roleSelectableCollectionEditor.items[i].id)
					container.userData.m_roles = arr
				}
			}
			
			GroupHeaderView {
				width: parent.width;
				
				title: qsTr("Groups");
				groupView: groupsBlock;
			}
			
			GroupElementView {
				id: groupsBlock;
				
				width: parent.width;

				GqlBasedItemSelectElementView {
					id: groupSelectableCollectionEditor
					collectionId: "Groups"
						label: qsTr("Groups")
						addButtonText: qsTr("Add Group")
						showCount: true
						onSelectionChanged: {
							groupsBlock.writeModelSelection()
						}
					}

				function applyModelSelection(){
					if (!container.userData){
						return
					}

					var ids = container.userData.m_groups ? container.userData.m_groups.slice() : []
					var arr = []
					for (var i = 0; i < ids.length; i++)
						arr.push({id: ids[i], name: ids[i]})
					groupSelectableCollectionEditor.items = arr
				}
				
				function writeModelSelection(){
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
