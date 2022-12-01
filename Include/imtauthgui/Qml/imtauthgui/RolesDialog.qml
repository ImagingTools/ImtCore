import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: container;

    property TreeItemModel model;
    property string productId;

    property var selectedIndex;

    contentComp: RolesDialogBody {
    }

    onFinished: {
        if (buttonId == "Include"){
            container.selectedIndex = contentItem.tableView.selectedIndex;
        }
    }

    onProductIdChanged: {
        console.log("RolesDialog onProductIdChanged", productId);
        contentItem.productId = productId;
    }

    onModelChanged: {
        console.log("RolesDialog onModelChanged", model);
        contentItem.model = model;
    }

    Component.onCompleted: {
        container.buttons.addButton({"Id": "Include", "Name": "Include", "Enabled": true});
        container.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        container.bodySource = "../../../imtauthgui/RolesDialogBody.qml";
        container.title = qsTr("Include Roles");
    }
}
