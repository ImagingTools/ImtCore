import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: featuresDialog;

    width: 300;

    signal featureAdded(var modelIndex);

    property ModelIndex selectedIndex: null;

    Component.onCompleted: {
        featuresDialog.buttons.addButton({"Id": "Add", "Name": "Add", "Enabled": false});
        featuresDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        featuresDialog.title = qsTr("Select features");

        featuresDialog.updateGui();
    }

    onFinished: {
        if (buttonId == "Add"){
            featuresDialog.selectedIndex = featuresDialog.contentItem.tableView.selectedIndex;
        }
    }

    function updateGui(){
        featuresDialog.contentItem.tableView.columnModel.clear();
        featuresDialog.contentItem.tableView.rowModel.clear();

        featuresDialog.contentItem.tableView.addColumn({"Id": "Name", "Name": "Feature Name"})

        let model = featuresProvider.model;

        for (let i = 0; i < model.GetItemsCount(); i++){
            let id = model.GetData("Id", i);
            let name = model.GetData("Name", i);

            featuresDialog.contentItem.tableView.insertRow([i], {"Name": name, "Id": id, "CheckBoxVisible": false})

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                for (let j = 0; j < childModel.GetItemsCount(); j++){
                    let featureId = childModel.GetData("Id", j);
                    let featureName = childModel.GetData("Name", j);

                    featuresDialog.contentItem.tableView.insertRow([i, j], {"Name": featureName, "Id": featureId, "CheckBoxVisible": false})
                }
            }
        }
    }

    contentComp: Component{ Item {
        id: dialogBody;

        height: bodyColumn.height + 40;

        property alias tableView: tableTreeView;

        Column {
            id: bodyColumn;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.rightMargin: 10;
            anchors.leftMargin: 10;

            width: featuresDialog.width;

            BasicTreeView {
                id: tableTreeView;

                width: parent.width;
                height: 300;

                onSelectedIndexChanged: {
                    console.log("onSelectedIndexChanged", tableTreeView.selectedIndex);

                    if (tableTreeView.selectedIndex != null && tableTreeView.selectedIndex.itemData){
                         console.log("selectedIndex.itemData.Level", tableTreeView.selectedIndex.itemData.Level);
                        let state = tableTreeView.selectedIndex.itemData.Level === 1;

                        console.log("featuresDialog", featuresDialog);
                        console.log("featuresDialog.buttons", featuresDialog.buttons);
                        featuresDialog.buttons.setButtonState("Add", state);
                    }
                }
            }
        }
    } }


}
