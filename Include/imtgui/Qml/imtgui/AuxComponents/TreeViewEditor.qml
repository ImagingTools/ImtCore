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

    TreeView {
        anchors.top: rowCommands.bottom;

        width: parent.width;
        height: parent.height;

        modelItems: treeViewContainer.modelItems;
    }

    TreeViewCommandsDelegateBase {
        id: commandsDelegate;
        treeViewModel: modelItems;
    }
}
