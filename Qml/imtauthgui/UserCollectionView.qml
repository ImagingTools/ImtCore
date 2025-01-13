import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthUsersSdl 1.0

RemoteCollectionView {
	id: userCollectionViewContainer;

	collectionId: "Users";
	visibleMetaInfo: true;

	additionalFieldIds: ["SystemId"]
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

			Row {
				id: content;
				anchors.left: parent.left;
				anchors.verticalCenter: parent.verticalCenter;
				spacing: Style.size_mainMargin;

				TreeItemModel {
					id: systemInfoModel;

					Component.onCompleted: {
						updateModel();
					}

					function updateModel(){
						systemInfoModel.clear();

						let index = systemInfoModel.insertNewItem();
						systemInfoModel.setData("Id", "All", index)
						systemInfoModel.setData("Name", qsTr("All systems"), index)

						index = systemInfoModel.insertNewItem();
						systemInfoModel.setData("Id", "Internal", index)
						systemInfoModel.setData("Name", qsTr("Internal"), index)

						index = systemInfoModel.insertNewItem();
						systemInfoModel.setData("Id", "Ldap", index)
						systemInfoModel.setData("Name", qsTr("LDAP"), index)

						systemComboBox.model = systemInfoModel;
					}
				}

				ComboBox {
					id: systemComboBox;

					width: 250;
					height: filtermenu.height;
					currentIndex: 0;
					onCurrentIndexChanged: {
						if (currentIndex == 1){
							userCollectionViewContainer.collectionFilter.addAdditionalFilter("SystemId", "")
						}
						else if (currentIndex > 1){
							let id = systemComboBox.model.getData("Id", currentIndex);
							userCollectionViewContainer.collectionFilter.addAdditionalFilter("SystemId", id)
						}
						else{
							userCollectionViewContainer.collectionFilter.removeFilterById("SystemId");
						}

						userCollectionViewContainer.doUpdateGui();
					}
				}
			}

			FilterPanelDecorator {
				id: filtermenu
				anchors.verticalCenter: parent.verticalCenter;
				anchors.right: parent.right;
				baseElement: decoratorBase.baseElement;
				width: 325;
			}
		}
	}

	function onLocalizationChanged(language){
		userCollectionViewContainer.dataController.updateHeaders();
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", userCollectionViewContainer.onLocalizationChanged);
	}

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", userCollectionViewContainer.onLocalizationChanged);
		collectionFilter.setSortingOrder("ASC");
		collectionFilter.setSortingInfoId("Name");

		userCollectionViewContainer.table.nonSortableColumns = ["Roles", "Groups"]
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
		table.setColumnContentById("Roles", dataComp);
		table.setColumnContentById("Groups", groupsContentComp);
	}

	Component {
		id: dataComp;

		TableCellDelegateBase {
			id: cellDelegate

			height: 20;

			clip: true;

			onDraw: {
				if (!rowDelegate){
					return
				}

				let username = userCollectionViewContainer.table.elements.getData("Name", rowIndex);
				let roles = userCollectionViewContainer.table.elements.getData("Roles", rowIndex);
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

				font.pixelSize: Style.fontSize_common;
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

			onDraw: {
				if (!rowDelegate){
					return
				}

				let username = userCollectionViewContainer.table.elements.getData("Name", rowIndex);
				let groups = userCollectionViewContainer.table.elements.getData("Groups", rowIndex);
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
					anchors.leftMargin: Style.size_mainMargin;

					font.pixelSize: Style.fontSize_common;
					font.family: Style.fontFamily;
					color: Style.textColor;

					elide: Text.ElideRight;
				}

				ToolButton {
					id: arrowButton;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: name2.right;
					anchors.leftMargin: Style.size_smallMargin;

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

			commandsControllerComp: Component {CommandsPanelController {
					commandId: "User";
					uuid: userEditor.viewId;
					commandsView: userEditor.commandsView;
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

			payloadModel: UserDataPayload {
				onFinished: {
					requestDocumentDataController.documentModel = m_userData;
				}
			}

			onSaved: {
				userCollectionViewContainer.saved();
			}

			function getHeaders(){
				return userCollectionViewContainer.getHeaders();
			}

			function getDocumentName(){
				let prefixName = qsTr("Users");

				if (documentName !== ""){
					return prefixName + " / " + documentName;
				}

				return prefixName + " / " + qsTr("New User");
			}
		}
	}
}


