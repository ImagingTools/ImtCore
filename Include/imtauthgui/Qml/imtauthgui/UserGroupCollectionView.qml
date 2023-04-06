import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: userGroupCollectionViewContainer;
    Rectangle {
        anchors.fill: parent;
        color: "red";
    }

    Component.onCompleted: {
        console.log("UserGroupCollection onCompleted", model.index);
        userGroupCollectionViewContainer.commandUpdateGui = "UserGroupCollectionUpdateGui";
        userGroupCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserGroupCollectionViewCommandsDelegate.qml";
        
        baseCollectionView.commands.fieldsData.push("UserGroupId");
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "Rename", "Name": qsTr("Rename"), "IconSource": ""});
    }
}
