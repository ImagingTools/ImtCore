import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    width: commands.width;

    property TreeItemModel commandModel: null;

    property alias radius: commands.radius;
    property alias color: commands.color;

    signal commandActivated(string commandId);
    signal commandAdded(int index, string commandId);

    onCommandModelChanged: {
        if (container.commandModel != null){
            repeater.model = container.commandModel;
        }
    }

    function setCommandVisible(commandId, visible){
        for (let i = 0; i < container.commandModel.GetItemsCount(); i++){
            let id = container.commandModel.GetData("Id", i);
            if (id === commandId){
                container.commandModel.SetData("Visible", visible, i);
                break;
            }
        }
    }

    Rectangle {
        id: commands;

        width: row.width;

        height: container.height;

        color: "transparent";

        Row {
            id: row;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            spacing: 10;

            Repeater {
                id: repeater;

                delegate: AuxButton {
                    width: 18;
                    height: width;

                    iconSource: enabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Normal) :
                                                  "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);

                    enabled: model.IsEnabled;

                    visible: model.Visible;

                    property string commandId: model.Id;

                    onClicked: {
                        container.commandActivated(model.Id);
                    }
                }

                onItemAdded: {
                    container.commandAdded(index, item.commandId)
                }
            }
        }
    }
}
