import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    height: bodyColumn.height + 40;

    property TreeItemModel model;
    property string productId;

    property alias tableView: table;

    property Item rootItem;

    onProductIdChanged: {
        for (let i = 0; i < container.model.GetItemsCount(); i++){
            let currentProductId = container.model.GetData("Id", i);
            if (currentProductId == container.productId){
                let roles = container.model.GetData("Roles", i);

                for (let j = 0; j < roles.GetItemsCount(); j++){
                    let roleId = roles.GetData("Id", j);

                    if (roleId == rootItem.currentRoleId){
                        continue;
                    }

                    let roleName = roles.GetData("Name", j);

                    console.log("roleId", roleId);

                    table.addRow({"Id": roleId, "Name": roleName});
                }

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

        BasicTableView {
            id: table;

            width: bodyColumn.width;
            height: 150;

            Component.onCompleted: {
                table.addColumn({"Id": "Name", "Name": "Name"});
            }

            onSelectedIndexChanged: {
                let isEnabled = table.selectedIndex != null;
                container.rootItem.buttons.setButtonState("Include", isEnabled);
            }
        }
    }//Column
}
