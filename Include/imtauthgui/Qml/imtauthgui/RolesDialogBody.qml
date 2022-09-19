import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    height: bodyColumn.height + 40;

    property TreeItemModel model;
    property string productId;

    property int selectedIndex: table.selectedIndex;

    onProductIdChanged: {
        for (let i = 0; i < model.GetItemsCount(); i++){
            let currentProductId = model.GetData("Id", i);
            if (currentProductId == productId){
                let roles = model.GetData("Roles", i);

                table.elements = roles;
                break;
            }
        }
    }

    Column {
        id: bodyColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 10;

        width: container.width;

        spacing: 5;

        TreeItemModel {
            id: headersModelRoles;

            Component.onCompleted: {
                let index = headersModelRoles.InsertNewItem();
                headersModelRoles.SetData("Id", "Name", index)
                headersModelRoles.SetData("Name", "Name", index)
            }
        }

        AuxTable {
            id: table;

            width: bodyColumn.width;
            height: 150;

            headers: headersModelRoles;
        }
    }//Column
}
