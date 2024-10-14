import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: featuresDialog;

    width: 500;

    title: qsTr("Select features");

    signal featureAdded(var modelIndex);

    property var selectedIndexes: [];
    property TreeItemModel featuresModel: TreeItemModel {}
    property TreeItemModel tableModel: TreeItemModel {}
    property var excludeFeatureIds: []

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.ok, Name:qsTr("Add"), Enabled: true})
        buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})

        updateGui();
    }

    onFeaturesModelChanged: {
        for (let i = 0; i < featuresModel.getItemsCount(); i++){
            let featureId = featuresModel.getData("Id", i);
            if (!excludeFeatureIds.includes(featureId)){
                let index = tableModel.insertNewItem();

                tableModel.copyItemDataFromModel(index, featuresModel, i)
            }
        }

        updateGui();
    }

    onFinished: {
        if (buttonId == Enums.ok){
            featuresDialog.selectedIndexes = featuresDialog.contentItem.tableView.getCheckedItems();
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Return){
            let enabled = featuresDialog.buttons.getButtonState(Enums.ok);
            if (enabled){
                featuresDialog.finished(Enums.ok);
            }
        }
        else if (event.key === Qt.Key_Escape){
            featuresDialog.finished(Enums.cancel);
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
            function updateHeaders(){
                dialogBody.headersModel.clear();

                let index = dialogBody.headersModel.insertNewItem();
                dialogBody.headersModel.setData("Id", "FeatureName", index);
                dialogBody.headersModel.setData("Name", qsTr("Feature Name"), index);

                dialogBody.headersModel.refresh();

                tableTreeView.headers = dialogBody.headersModel;
            }

            Component.onCompleted: {
                updateHeaders();
            }
        }

        Column {
            id: bodyColumn;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.rightMargin: Style.size_mainMargin;
            anchors.leftMargin: Style.size_mainMargin;
            width: featuresDialog.width;

            Table {
                id: tableTreeView;
                width: parent.width;
                height: 300;
                radius: 0;
                checkable: true;
                onCheckedItemsChanged: {
                    let indexes = getCheckedItems();
                    featuresDialog.buttons.setButtonState(Enums.ok, indexes.length > 0);
                }
            }
        }
    } }
}
