import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcolqml 1.0

CollectionView {
    id: userGroupCollectionViewContainer;

    documentName: qsTr("Groups");

    Component.onCompleted: {
        userGroupCollectionViewContainer.commandsDelegatePath = "../imtauthgui/UserGroupCollectionViewCommandsDelegate.qml";

        userGroupCollectionViewContainer.commandId = "Groups";
    }

    onDocumentManagerPtrChanged: {
        if (documentManagerPtr){
            documentManagerPtr.registerDocument("Group", userGroupDocumentComp);
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }

    function selectItem(id){
        if (id === ""){
            documentManagerPtr.insertNewDocument("Group");
        }
        else{
            documentManagerPtr.openDocument(id, "Group");
        }
    }

    Component {
        id: userGroupDocumentComp;

        UserGroupView {
        }
    }
}
