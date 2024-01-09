import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: featuresDialog;

    buttonsModel: ListModel{
        ListElement{Id: Enums.ButtonType.Ok; Name:qsTr("Add"); Enabled: true}
        ListElement{Id: Enums.ButtonType.Cancel; Name:qsTr("Cancel"); Enabled: true}
    }

    width: 500;

    title: qsTr("Select features");

    signal featureAdded(var modelIndex);

    property var selectedIndexes: [];

    property TreeItemModel featuresModel: TreeItemModel {}

    property TreeItemModel tableModel: TreeItemModel {}

    property var excludeFeatureIds: []

    onFeaturesModelChanged: {
        for (let i = 0; i < featuresModel.GetItemsCount(); i++){
            let featureId = featuresModel.GetData("Id", i);
            if (!excludeFeatureIds.includes(featureId)){
                let index = tableModel.InsertNewItem();

                tableModel.CopyItemDataFromModel(index, featuresModel, i)
            }
        }

        updateGui();
    }

    Component.onCompleted: {
//        featuresDialog.buttons.addButton({"Id": "Add", "Name": "Add", "Enabled": false});
//        featuresDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

//        featuresDialog.title = qsTr("Select features");

        updateGui();
    }

    onFinished: {
        if (buttonId == Enums.ButtonType.Ok){
            featuresDialog.selectedIndexes = featuresDialog.contentItem.tableView.getSelectedIndexes();
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Return){
            let enabled = featuresDialog.buttons.getButtonState(Enums.ButtonType.Ok);
            if (enabled){
                featuresDialog.finished(Enums.ButtonType.Ok);
            }
        }
        else if (event.key === Qt.Key_Escape){
            featuresDialog.finished(Enums.ButtonType.Cancel);
        }
    }

    function updateGui(){
        featuresDialog.contentItem.tableView.elements = featuresDialog.tableModel;
    }

    contentComp: Component{ Item {
        id: dialogBody;

        width: featuresDialog.width;
        height: bodyColumn.height + 40;

        property alias tableView: tableTreeView;

        property TreeItemModel headersModel: TreeItemModel {
            Component.onCompleted: {
                let index = dialogBody.headersModel.InsertNewItem();
                dialogBody.headersModel.SetData("Id", "FeatureName", index);
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
                    featuresDialog.buttons.setButtonState(Enums.ButtonType.Ok, indexes.length > 0);
                }
            }
        }
    } }
}
