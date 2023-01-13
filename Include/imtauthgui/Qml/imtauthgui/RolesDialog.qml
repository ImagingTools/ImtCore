import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: rolesDialog;

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;


    property TreeItemModel model;
    property string productId;

    property var selectedIndex;

    contentComp: RolesDialogBody {
    }

    onFinished: {
        if (buttonId == "Include"){
            rolesDialog.selectedIndex = contentItem.tableView.selectedIndex;
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
        rolesDialog.buttons.addButton({"Id": "Include", "Name": "Include", "Enabled": false});
        rolesDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        rolesDialog.bodySource = "../../../imtauthgui/RolesDialogBody.qml";
        rolesDialog.title = qsTr("Include Roles");
    }

    Loader{
        id: messageDecoratorLoader;

        sourceComponent: Style.messageDecorator !==undefined ? Style.messageDecorator: emptyDecorator;
        onLoaded: {
            if(messageDecoratorLoader.item){
                messageDecoratorLoader.item.rootItem = rolesDialog;
            }
        }
    }
}
