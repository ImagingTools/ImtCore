import QtQuick 2.15
import Acf 1.0

Item {
    id: treeViewContainer;

    property string commandsId: "TreeView";

    property TreeItemModel modelItems;

    Rectangle {
        anchors.fill: rowCommands;
        color: Style.alternateBaseColor;
    }

    function resetSelectedItem(model){
        let count = model.GetItemsCount();
        for (let i = 0; i < count; i++){
            let selected = model.GetData("Selected", i);

            if (selected){
                model.SetData("Selected", false, i);
            }

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                resetSelectedItem(childModel);
            }
        }
    }

    Row {
        id: rowCommands;

        height: 25;
        width: parent.width;

        clip: true;

        MouseArea {
            anchors.fill: parent;
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            spacing: 10;

            Repeater {
                model: commandsDelegate.commandsModel;

                delegate: AuxButton {
                    anchors.verticalCenter: rowCommands.verticalCenter;

                    width: 21;
                    height: width;

                    iconSource: model.IconSource;

                    onClicked: {
                        Events.sendEvent(commandsId + "CommandActivated", model.Id);
                    }
                }
            }
        }
    }


    TreeItemModel {
        id: headersModel;

        Component.onCompleted: {
            let index = headersModel.InsertNewItem();

            headersModel.SetData("Id", "FeatureId", index);
            headersModel.SetData("Name", "Feature-ID", index);

            index = headersModel.InsertNewItem();

            headersModel.SetData("Id", "FeatureName", index);
            headersModel.SetData("Name", "Feature Name", index);

            index = headersModel.InsertNewItem();

            headersModel.SetData("Id", "FeatureDescription", index);
            headersModel.SetData("Name", "Description", index);

        }
    }

    Component {
        id: delegateComp;

        TableTreeItemDelegate {
            width: treeViewContainer.width;

            onDoubleClicked: {

            }
        }
    }

    AuxTable {
        id: table;

        anchors.top: rowCommands.bottom;

        width: parent.width;
        height: parent.height;

        headers: headersModel;

        elements: treeViewContainer.modelItems;

        delegate: delegateComp;
    }

    TreeViewCommandsDelegateBase {
        id: commandsDelegate;
        treeViewModel: modelItems;
    }
}
