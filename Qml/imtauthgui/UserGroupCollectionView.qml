import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

CollectionView {
    id: userGroupCollectionViewContainer;

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
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

        let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            userGroupCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Group", "GroupEditor", userGroupDocumentComp);
            documentManager.registerDocumentDataController("Group", dataControllerComp);
        }
    }

    Component {
        id: userGroupDocumentComp;

        UserGroupView {
            id: groupEditor;
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

                if (documentModel.ContainsKey("Name")){
                    return prefixName + " / " + documentModel.GetData("Name")
                }

                let newGroupName = qsTr("New Group");
                return newGroupName;
            }
        }
    }
}
