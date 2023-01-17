import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    visible: !connection;

    property bool connection: true;

    function setConnectionState(state){
        console.log("ConnectionManager setInvalidConnection", state);
        container.connection = state;
    }

    Rectangle {
        id: background;

        anchors.fill: parent;
        color: "gray";
        opacity: 0.4;
        MouseArea {
            anchors.fill: parent;
            onWheel: {}
        }
    }

    Rectangle {
        id: body;

        anchors.verticalCenter: parent.verticalCenter;

        width: parent.width;
        height: parent.height / 2;

        color: Style.backgroundColor;

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.3; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.5; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }

        Text {
            id: textNoConnection;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            color: Style.textColor;
            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            text: qsTr("There is no connection to the server!");
        }

        AuxButton {
            id: refreshButton;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: textNoConnection.bottom;
            anchors.topMargin: 10;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            borderColor: (refreshButton.highlighted || refreshButton.focus) ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.baseColor;

            textButton: qsTr("Refresh");

            onClicked: {
                window.updateModels();
            }
        }
    }
}

