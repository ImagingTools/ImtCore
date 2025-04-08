import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthGroupsSdl 1.0

RemoteCollectionView {
	id: userGroupCollectionViewContainer;
	
	collectionId: "Groups";
	visibleMetaInfo: true;
	
	property string productId;
	property var documentManager: null;
	
	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: userGroupCollectionViewContainer;
			documentManager: userGroupCollectionViewContainer.documentManager;
			
			documentTypeId: "Group";
			viewTypeId: "GroupEditor";
			
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

	Component.onCompleted: {
		table.setSortingInfo(GroupItemTypeMetaInfo.s_name, "ASC");
		
		if (documentManager){
			documentManager.registerDocumentView("Group", "GroupEditor", userGroupDocumentComp);
			documentManager.registerDocumentDataController("Group", documentDataControllerComp);
		}
	}
	
	function handleSubscription(dataModel){
		userGroupCollectionViewContainer.doUpdateGui();
	}
	
	property TreeItemModel usersModel;
	property TreeItemModel rolesModel;
	property TreeItemModel groupsModel;
	
	Component {
		id: userGroupDocumentComp;
		
		UserGroupView {
			id: groupEditor;
			
			usersModel: userGroupCollectionViewContainer.usersModel;
			rolesModel: userGroupCollectionViewContainer.rolesModel;
			groupsModel: userGroupCollectionViewContainer.groupsModel;
			
			productId: userGroupCollectionViewContainer.productId;
			
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "Group";
				}
			}
		}
	}
	
	Component {
		id: documentDataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property GroupData groupData: documentModel;

			typeId: "Group";
			documentName: groupData ? groupData.m_name: "";
			documentDescription: groupData ? groupData.m_description: "";
			
			gqlGetCommandId: ImtauthGroupsSdlCommandIds.s_groupItem;
			gqlUpdateCommandId: ImtauthGroupsSdlCommandIds.s_groupUpdate;
			gqlAddCommandId: ImtauthGroupsSdlCommandIds.s_groupAdd;
			
			Component.onCompleted: {
				getRequestInputParam.InsertField(GroupItemInputTypeMetaInfo.s_productId, userGroupCollectionViewContainer.productId);
				addRequestInputParam.InsertField(GroupItemInputTypeMetaInfo.s_productId, userGroupCollectionViewContainer.productId);
				updateRequestInputParam.InsertField(GroupItemInputTypeMetaInfo.s_productId, userGroupCollectionViewContainer.productId);
			}
			
			documentModelComp: Component {
				GroupData {}
			}
			
			function getHeaders(){
				return userGroupCollectionViewContainer.getHeaders();
			}
		}
	}
}
