import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
    id: container;

    visibleMetaInfo: true;
    collectionId: "Accounts";

    dataControllerComp: Component {CollectionRepresentation {
            collectionId: "Accounts";
        }
    }

    commandsControllerComp: Component {CommandsRepresentationProvider {
            commandId: "Accounts";
            uuid: container.viewId;
        }
    }

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: container;

            documentTypeId: "Account";
            viewTypeId: "AccountEditor";

            function setupContextMenu(){
                let commandsController = collectionView.commandsController;
                if (commandsController){
                    contextMenuModel.Clear();

                    let canEdit = commandsController.commandExists("Edit");
                    let canRemove = commandsController.commandExists("Remove");

                    if (canEdit){
                        let index = contextMenuModel.InsertNewItem();

                        contextMenuModel.SetData("Id", "Edit", index);
                        contextMenuModel.SetData("Name", qsTr("Edit"), index);
                        contextMenuModel.SetData("Icon", "Icons/Edit", index);
                    }

                    if (canRemove){
                        let index = contextMenuModel.InsertNewItem();

                        contextMenuModel.SetData("Id", "Remove", index);
                        contextMenuModel.SetData("Name", qsTr("Remove"), index);
                        contextMenuModel.SetData("Icon", "Icons/Delete", index);
                    }

                    if (canEdit){
                        let index = contextMenuModel.InsertNewItem();

                        contextMenuModel.SetData("Id", "SetDescription", index);
                        contextMenuModel.SetData("Name", qsTr("Set Description"), index);
                        contextMenuModel.SetData("Icon", "", index);
                    }

                    contextMenuModel.Refresh();
                }
            }
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("ASC");
        collectionFilter.setSortingInfoId("Name");

        let documentManager = MainDocumentManager.getDocumentManager("Accounts");
        if (documentManager){
            container.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Account", "AccountEditor", accountEditorComp);
            documentManager.registerDocumentDataController("Account", dataControllerComp);
        }
    }

    Component {
        id: accountEditorComp;

        AccountEditor {
            id: accountEditor;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                view: accountEditor;
            }
            }

            commandsControllerComp: Component {CommandsRepresentationProvider {
                    commandId: "Account";
                    uuid: accountEditor.viewId;
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "AccountItem";
            gqlUpdateCommandId: "AccountUpdate";
            gqlAddCommandId: "AccountAdd";
        }
    }
}
