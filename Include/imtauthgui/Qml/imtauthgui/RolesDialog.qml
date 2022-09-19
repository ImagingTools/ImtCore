import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: container;

    property TreeItemModel model;
    property string productId;

    property int selectedIndex: bodyItem.selectedIndex;

    onFinished: {
        if (buttonId === "Include"){
        }
    }

    onProductIdChanged: {
        console.log("RolesDialog onProductIdChanged", productId);
        bodyItem.productId = productId;
    }

    onModelChanged: {
        console.log("RolesDialog onModelChanged", model);
        bodyItem.model = model;
    }

    Component.onCompleted: {
        container.buttons.addButton({"Id": "Include", "Name": "Include", "Enabled": true});
        container.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        container.bodySource = "../../../imtauthgui/RolesDialogBody.qml";
        container.title = qsTr("Include Roles");
    }
}
