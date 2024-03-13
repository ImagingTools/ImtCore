import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
    id: userGroupCollectionViewContainer;

    collectionId: "Groups";

    dataController: CollectionRepresentation {
        collectionId: "Groups";
    }

    commandsController: CommandsRepresentationProvider {
        commandId: "Groups";
        uuid: userGroupCollectionViewContainer.viewId;
    }

    commandsDelegate: DocumentCollectionViewDelegate {
        collectionView: userGroupCollectionViewContainer;

        documentTypeId: "Group";
        viewTypeId: "GroupEditor";
    }

    Component.onCompleted: {
        CachedUserCollection.updateModel();
        CachedRoleCollection.updateModel();
        CachedGroupCollection.updateModel();

        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

        let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            userGroupCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Group", "GroupEditor", userGroupDocumentComp);
            documentManager.registerDocumentDataController("Group", dataControllerComp);
        }
    }

    property TreeItemModel usersModel: CachedUserCollection.collectionModel;
    property TreeItemModel rolesModel: CachedRoleCollection.rolesModel;
    property TreeItemModel groupsModel: CachedGroupCollection.collectionModel;

    Component {
        id: userGroupDocumentComp;

        UserGroupView {
            id: groupEditor;

            usersModel: userGroupCollectionViewContainer.usersModel;
            rolesModel: userGroupCollectionViewContainer.rolesModel;
            groupsModel: userGroupCollectionViewContainer.groupsModel;

            commandsDelegate: ViewCommandsDelegateBase {
                view: groupEditor;
            }

            commandsController: CommandsRepresentationProvider {
                commandId: "Group";
                uuid: groupEditor.viewId;
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "GroupItem";
            gqlUpdateCommandId: "GroupUpdate";
            gqlAddCommandId: "GroupAdd";

            function getDocumentName(){
                let prefixName = qsTr("Groups");

                if (documentName !== ""){
                    return prefixName + " / " + documentName;
                }

                return prefixName + " / " + qsTr("New Group");
            }
        }
    }
}
