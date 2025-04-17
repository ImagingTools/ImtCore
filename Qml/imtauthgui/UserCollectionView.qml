import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthUsersSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: userCollectionViewContainer;
	
	collectionId: "Users";
	visibleMetaInfo: true;

	additionalFieldIds: [UserItemTypeMetaInfo.s_systemId]
	filterMenu.decorator: filterDecoratorComp;
	
	property string productId;
	property var documentManager: null;
	property TreeItemModel rolesModel;
	property TreeItemModel groupsModel;
	
	signal saved();
	
	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: userCollectionViewContainer;
			documentManager: userCollectionViewContainer.documentManager;
			
			documentTypeIds: ["User"];
			documentViewTypeIds: ["UserEditor"];
			documentViewsComp: [userDocumentComp];
			documentDataControllersComp: [documentDataControllerComp];
			documentValidatorsComp: [documentValidator];
			
			function updateStateBaseCommands(selection, commandsController, elementsModel){
				let isEnabled = selection.length > 0;
				if(commandsController){
					commandsController.setCommandIsEnabled("Remove", isEnabled);
					commandsController.setCommandIsEnabled("Edit", selection.length === 1);
					commandsController.setCommandIsEnabled("Export", selection.length === 1);
					commandsController.setCommandIsEnabled("Revision", selection.length === 1);
				}
			}
		}
	}
	
	function handleSubscription(dataModel){
		userCollectionViewContainer.doUpdateGui();
	}
	
	Component {
		id: filterDecoratorComp;
		
		DecoratorBase {
			id: decoratorBase;
			
			width: baseElement ? baseElement.width: 0;
			height: 40;
			
			property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;
			
			Row {
				id: content;
				anchors.left: parent.left;
				anchors.verticalCenter: parent.verticalCenter;
				spacing: Style.sizeMainMargin;
				
				TreeItemModel {
					id: systemInfoModel;
					
					Component.onCompleted: {
						updateModel();
					}
					
					function updateModel(){
						systemInfoModel.clear();
						
						let index = systemInfoModel.insertNewItem();
						systemInfoModel.setData("id", "All", index)
						systemInfoModel.setData("name", qsTr("All systems"), index)
						
						index = systemInfoModel.insertNewItem();
						systemInfoModel.setData("id", "Internal", index)
						systemInfoModel.setData("name", qsTr("Internal"), index)
						
						index = systemInfoModel.insertNewItem();
						systemInfoModel.setData("id", "Ldap", index)
						systemInfoModel.setData("name", qsTr("LDAP"), index)
						
						systemComboBox.model = systemInfoModel;
					}
				}
				
				FieldFilter {
					id: systemFilter
					m_fieldId: "SystemId"
					m_filterValue: ""
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
				}
				
				ComboBox {
					id: systemComboBox;
					
					width: 250;
					height: filtermenu.height;
					currentIndex: 0;
					onCurrentIndexChanged: {
						userCollectionViewContainer.collectionFilter.removeFieldFilter(systemFilter)
						
						if (currentIndex == 1){
							systemFilter.m_filterOperations = ["Equal"]
							userCollectionViewContainer.collectionFilter.addFieldFilter(systemFilter)
						}
						else if (currentIndex == 2){
							systemFilter.m_filterOperations = ["Not","Equal"]
							userCollectionViewContainer.collectionFilter.addFieldFilter(systemFilter)
						}

						userCollectionViewContainer.collectionFilter.filterChanged()
					}
				}
			}
			
			FilterPanelDecorator {
				id: filtermenu
				anchors.verticalCenter: parent.verticalCenter;
				anchors.right: parent.right;
				baseElement: decoratorBase.baseElement;
				width: 325;
				complexFilter: decoratorBase.complexFilter;
			}
		}
	}
	
	Component.onCompleted: {
		table.setSortingInfo(UserItemTypeMetaInfo.s_name, "ASC");
		table.nonSortableColumns = [UserItemTypeMetaInfo.s_roles, UserItemTypeMetaInfo.s_groups]
		table.rowDelegate = tableRowDelegateBaseComp;
	}
	
	Component {
		id: tableRowDelegateBaseComp;
		
		TableRowDelegate {
			tableItem: userCollectionViewContainer.table;
			width: userCollectionViewContainer.table.width;
			minHeight: userCollectionViewContainer.table.itemHeight;
			readOnly: userCollectionViewContainer.table.readOnly;
		}
	}
	
	onHeadersChanged: {
		table.setColumnContentById(UserItemTypeMetaInfo.s_roles, dataComp);
		table.setColumnContentById(UserItemTypeMetaInfo.s_groups, groupsContentComp);
	}
	
	Component {
		id: dataComp;
		
		TableCellDelegateBase {
			id: cellDelegate
			
			height: 20;
			
			clip: true;
			
			onReused: {
				if (!rowDelegate){
					return
				}
				
				let username = userCollectionViewContainer.table.elements.getData(UserItemTypeMetaInfo.s_name, rowIndex);
				let roles = userCollectionViewContainer.table.elements.getData(UserItemTypeMetaInfo.s_roles, rowIndex);
				arrowButton.visible = roles !== "";
				if (roles !== ""){
					let roleList = roles.split(';');
					name.text = qsTr("View roles") + "(" + roleList.length +  ")";
					
					let result = "<b><u>" + username + "</u></b> " + qsTr("has been assigned") + " <b>" + roleList.length + "</b> " + qsTr("roles:");
					for (let i = 0; i < roleList.length; i++){
						result += "<p>" + roleList[i] + "</p>";
					}
					
					arrowButton.tooltipText = result;
				}
				else{
					name.text = qsTr("No roles");
				}
			}
			
			Text {
				id: name;
				
				anchors.verticalCenter: parent.verticalCenter;
				
				font.pixelSize: Style.fontSizeNormal;
				font.family: Style.fontFamily;
				color: Style.textColor;
				
				elide: Text.ElideRight;
			}
			
			ToolButton {
				id: arrowButton;
				
				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: name.right;
				anchors.leftMargin: 5;
				
				width: 15;
				height: width;
				
				iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
				
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent";
						icon.width: 16;
					}
				}
			}
		}
	}
	
	Component {
		id: groupsContentComp;
		
		TableCellDelegateBase {
			id: cellDelegate
			
			onReused: {
				if (!rowDelegate){
					return
				}
				
				let username = userCollectionViewContainer.table.elements.getData(UserItemTypeMetaInfo.s_name, rowIndex);
				let groups = userCollectionViewContainer.table.elements.getData(UserItemTypeMetaInfo.s_groups, rowIndex);
				arrowButton.visible = groups !== "";
				if (groups !== ""){
					let groupList = groups.split(';');
					name2.text =  qsTr("View groups") + "(" + groupList.length +  ")";
					
					let result = "<b><u>" + username + "</u></b> " + qsTr("is included into") + " <b>" + groupList.length + "</b> " + qsTr("groups") + ":";
					for (let i = 0; i < groupList.length; i++){
						result += "<p>" + groupList[i] + "</p>";
					}
					arrowButton.tooltipText = result;
				}
				else{
					name2.text = qsTr("No groups");
				}
			}
			
			Item {
				anchors.fill: parent;
				
				clip: true;
				
				Text {
					id: name2;
					
					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: parent.left
					anchors.leftMargin: Style.sizeMainMargin;
					
					font.pixelSize: Style.fontSizeNormal;
					font.family: Style.fontFamily;
					color: Style.textColor;
					
					elide: Text.ElideRight;
				}
				
				ToolButton {
					id: arrowButton;
					
					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: name2.right;
					anchors.leftMargin: Style.sizeSmallMargin;
					
					width: 15;
					height: width;
					
					visible: name2.text !== "";
					
					iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
					
					decorator: Component {
						ToolButtonDecorator {
							color: "transparent";
							icon.width: 16;
						}
					}
				}
			}
		}
	}
	
	Component {
		id: documentValidator;
		DocumentValidator {
			property UserData userData: documentModel;
			property MailRegExpValidator mailRegExp: MailRegExpValidator {};
			
			property var regExp: new RegExp(mailRegExp.regularExpression)
			
			function isValid(data){
				if (!data.editor){
					return false;
				}
				
				if (!userData){
					return false;
				}
				
				if (userData.m_name === ""){
					return false;
				}
				
				if (userData.m_username === ""){
					return false;
				}
				
				if (userData.m_password === ""){
					return false;
				}
				
				if (!regExp.test(userData.m_email)){
					return false;
				}
				
				if (userData.m_id === ""){
					let passwordText1 = data.editor.passwordInput.text;
					let passwordText2 = data.editor.passwordInputConfirm.text;
					if (passwordText1 != passwordText2){
						return false;
					}
				}
				
				return true;
			}
		}
	}
	
	Component {
		id: userDocumentComp;
		
		UserView {
			id: userEditor;
			
			rolesModel: userCollectionViewContainer.rolesModel;
			groupsModel: userCollectionViewContainer.groupsModel;
			productId: userCollectionViewContainer.productId;
			
			Component.onCompleted: {
				userCollectionViewContainer.saved.connect(userEditor.onSaved);
			}
			
			commandsDelegateComp: Component {ViewCommandsDelegateBase {
					view: userEditor;
				}
			}
			
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "User";
				}
			}
			
			function onSaved(){
				if (userEditor){
					userEditor.checkChangePasswordLogic();
				}
			}
		}
	}
	
	Component {
		id: documentDataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property UserData userData: documentModel;
			
			typeId: "User";
			documentName: userData ? userData.m_name: "";
			
			gqlGetCommandId: ImtauthUsersSdlCommandIds.s_userItem;
			gqlUpdateCommandId: ImtauthUsersSdlCommandIds.s_userUpdate;
			gqlAddCommandId: ImtauthUsersSdlCommandIds.s_userAdd;
			
			Component.onCompleted: {
				getRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, userCollectionViewContainer.productId);
				addRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, userCollectionViewContainer.productId);
				updateRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, userCollectionViewContainer.productId);
			}
			
			documentModelComp: Component {
				UserData {}
			}
			
			onSaved: {
				userCollectionViewContainer.saved();
			}
			
			function getHeaders(){
				return userCollectionViewContainer.getHeaders();
			}
		}
	}
}


