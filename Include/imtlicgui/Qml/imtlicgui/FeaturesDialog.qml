import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: root;

    width: 300;

    signal featureAdded(var modelIndex);

    property ModelIndex selectedIndex: null;

    onFinished: {
        if (buttonId == "Add"){
            root.selectedIndex = contentItem.tableView.selectedIndex;
        }
    }

    function updateGui(){
        contentItem.tableView.columnModel.clear();
        contentItem.tableView.rowModel.clear();

        contentItem.tableView.addColumn({"Id": "Name", "Name": "Feature Name"})

        let model = featuresProvider.model;

        for (let i = 0; i < model.GetItemsCount(); i++){
            let id = model.GetData("Id", i);
            let name = model.GetData("Name", i);

            contentItem.tableView.insertRow([i], {"Name": name, "Id": id, "CheckBoxVisible": false})

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                for (let j = 0; j < childModel.GetItemsCount(); j++){
                    let featureId = childModel.GetData("Id", j);
                    let featureName = childModel.GetData("Name", j);

                    contentItem.tableView.insertRow([i, j], {"Name": featureName, "Id": featureId, "CheckBoxVisible": false})
                }
            }
        }
    }

    contentComp: Item {
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

            width: root.width;

            BasicTreeView {
                id: tableTreeView;

                width: parent.width;
                height: 300;

                onSelectedIndexChanged: {
                    console.log("onSelectedIndexChanged", selectedIndex);

                    if (selectedIndex != null){
                         console.log("selectedIndex.itemData.Level", selectedIndex.itemData.Level);
                        let state = selectedIndex.itemData.Level == 1;
                        root.buttons.setButtonState("Add", state);
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        root.buttons.addButton({"Id": "Add", "Name": "Add", "Enabled": false});
        root.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        root.title = qsTr("Select features");

        updateGui();
    }
}
