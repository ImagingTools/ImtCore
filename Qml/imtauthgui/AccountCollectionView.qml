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

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: container;

            documentTypeId: "Account";
            viewTypeId: "AccountEditor";

            function setupContextMenu(){
                let commandsController = collectionView.commandsController;
                if (commandsController){
                    contextMenuModel.clear();

                    let canEdit = commandsController.commandExists("Edit");
                    let canRemove = commandsController.commandExists("Remove");

                    if (canEdit){
                        let index = contextMenuModel.insertNewItem();

                        contextMenuModel.setData("Id", "Edit", index);
                        contextMenuModel.setData("Name", qsTr("Edit"), index);
                        contextMenuModel.setData("Icon", "Icons/Edit", index);
                    }

                    if (canRemove){
                        let index = contextMenuModel.insertNewItem();

                        contextMenuModel.setData("Id", "Remove", index);
                        contextMenuModel.setData("Name", qsTr("Remove"), index);
                        contextMenuModel.setData("Icon", "Icons/Delete", index);
                    }

                    let ok = PermissionsController.checkPermission("ChangeAccountDescription");
                    if (ok){
                        let index = contextMenuModel.insertNewItem();

                        contextMenuModel.setData("Id", "SetDescription", index);
                        contextMenuModel.setData("Name", qsTr("Set Description"), index);
                        contextMenuModel.setData("Icon", "", index);
                    }

                    contextMenuModel.refresh();
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
