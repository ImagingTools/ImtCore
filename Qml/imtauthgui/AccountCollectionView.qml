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
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

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
