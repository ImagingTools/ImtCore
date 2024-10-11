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

        documentTypeId: "Group";
        viewTypeId: "GroupEditor";
    }
    }

    Component.onCompleted: {
        CachedUserCollection.updateModel();
        CachedRoleCollection.updateModel();
        CachedGroupCollection.updateModel();

        collectionFilter.setSortingOrder("ASC");
        collectionFilter.setSortingInfoId("Name");

        // let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            userGroupCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Group", "GroupEditor", userGroupDocumentComp);
            documentManager.registerDocumentDataController("Group", documentDataControllerComp);
        }
    }

    dataControllerComp: Component {
        CollectionRepresentation {
            collectionId: userGroupCollectionViewContainer.collectionId;
            additionalFieldIds: userGroupCollectionViewContainer.additionalFieldIds;

            function getHeaders(){
                return userGroupCollectionViewContainer.getHeaders();
            }
        }
    }

    function handleSubscription(dataModel){
        userGroupCollectionViewContainer.doUpdateGui();
    }

    property TreeItemModel usersModel: CachedUserCollection.collectionModel;
    property TreeItemModel rolesModel: CachedRoleCollection.collectionModel;
    property TreeItemModel groupsModel: CachedGroupCollection.collectionModel;

    Component {
        id: userGroupDocumentComp;

        UserGroupView {
            id: groupEditor;

            usersModel: userGroupCollectionViewContainer.usersModel;
            rolesModel: userGroupCollectionViewContainer.rolesModel;
            groupsModel: userGroupCollectionViewContainer.groupsModel;

            productId: userGroupCollectionViewContainer.productId;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                view: groupEditor;
            }}

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

            function getDocumentName(){
                let prefixName = qsTr("Groups");

                if (documentName !== ""){
                    return prefixName + " / " + documentName;
                }

                return prefixName + " / " + qsTr("New Group");
            }

            function getHeaders(){
                return userGroupCollectionViewContainer.getHeaders();
            }
        }
    }
}
