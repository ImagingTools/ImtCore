import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcolqml 1.0

CollectionView {
    id: container;

    filterMenuVisible: true;

    documentName: qsTr("Customers");

    Component.onCompleted: {
        container.commandsDelegatePath = "../imtauthgui/AccountCollectionViewCommandsDelegate.qml";

        container.commandId = "Accounts";
    }

    onDocumentManagerPtrChanged: {
        if (documentManagerPtr){
//            documentManagerPtr.setDocumentTitle(0, title);
            documentManagerPtr.registerDocument("Account", accountEditorComp);
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();


        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }

    Component {
        id: accountEditorComp;

        AccountEditor {
        }
    }
}
