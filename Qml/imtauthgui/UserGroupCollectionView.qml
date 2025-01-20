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
        collectionFilter.setSortingOrder("ASC");
        collectionFilter.setSortingInfoId("Name");

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

            commandsControllerComp: Component {CommandsPanelController {
                commandId: "Group";
                uuid: groupEditor.viewId;
                commandsView: groupEditor.commandsView;
            }
            }
        }
    }

    Component {
        id: documentDataControllerComp;

        GqlRequestDocumentDataController {
            id: requestDocumentDataController

			typeId: "Group";

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

            payloadModel: GroupDataPayload {
                onFinished: {
                    requestDocumentDataController.documentModel = m_groupData;
                }
            }

            function getHeaders(){
                return userGroupCollectionViewContainer.getHeaders();
            }
        }
    }
}
