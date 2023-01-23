import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    property TreeItemModel commandModel: null;

    property alias radius: commands.radius;
    property alias color: commands.color;

    signal commandActivated(string commandId);

    onCommandModelChanged: {
        if (container.commandModel != null){
            repeater.model = container.commandModel;
        }
    }

    Rectangle {
        id: commands;

        width: parent.width;
        height: 25;

        color: Style.alternateBaseColor;

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

                    iconSource: enabled ? "../../../../Icons/Light/" + model.Icon + "_Off_Normal.svg" :
                                                  "../../../../Icons/Light/" + model.Icon + "_Off_Disabled.svg";

                    enabled: model.IsEnabled;

                    onClicked: {
                        container.commandActivated(model.Id);
                    }
                }
            }
        }
    }
}
