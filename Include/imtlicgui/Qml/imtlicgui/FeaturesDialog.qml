import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: featuresDialog;

    width: 300;

    signal featureAdded(var modelIndex);

    property var selectedIndexes: [];

    property TreeItemModel featuresModel: TreeItemModel {}

    Component.onCompleted: {
        featuresDialog.buttons.addButton({"Id": "Add", "Name": "Add", "Enabled": false});
        featuresDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        featuresDialog.title = qsTr("Select features");

        updateGui();
    }

    onFinished: {
        if (buttonId == "Add"){
            featuresDialog.selectedIndexes = featuresDialog.contentItem.tableView.getSelectedIndexes();
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Return){
            let enabled = featuresDialog.buttons.getButtonState("Add");
            if (enabled){
                featuresDialog.finished("Add");
            }
        }
        else if (event.key === Qt.Key_Escape){
            featuresDialog.finished("Cancel");
        }
    }

    function updateGui(){
        featuresDialog.contentItem.tableView.elements = featuresDialog.featuresModel;
    }

    contentComp: Component{ Item {
        id: dialogBody;

        height: bodyColumn.height + 40;

        property alias tableView: tableTreeView;

        property TreeItemModel headersModel: TreeItemModel {
            Component.onCompleted: {
                let index = dialogBody.headersModel.InsertNewItem();
                dialogBody.headersModel.SetData("Id", "Name", index);
                dialogBody.headersModel.SetData("Name", "Feature Name", index);

                tableTreeView.headers = dialogBody.headersModel;
            }
        }

        Column {
            id: bodyColumn;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.rightMargin: 10;
            anchors.leftMargin: 10;

            width: featuresDialog.width;

            AuxTable {
                id: tableTreeView;

                width: parent.width;
                height: 300;

                radius: 0;

                onSelectionChanged: {
                    let indexes = tableTreeView.getSelectedIndexes();
                    featuresDialog.buttons.setButtonState("Add", indexes.length > 0);
                }
            }
        }
    } }
}
