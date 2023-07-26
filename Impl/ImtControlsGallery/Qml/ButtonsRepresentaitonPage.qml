import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcontrols 1.0

Rectangle {
    anchors.fill: parent;

    Row{
        id: buttonsRow;

        anchors.centerIn: parent;
        spacing: 50;

        Button{
            id: textButton;

            anchors.verticalCenter:  parent.verticalCenter;

            style: textButtonDecorator;

            text: "Text button";
            onClicked: {
                pauseMessage.restart();
            }
        }

        Button{
            id: roundButton;

            anchors.verticalCenter:  parent.verticalCenter;

            style: roundButtonDecorator;
            iconSource: "../../../Icons/" + Style.theme + "/Close.svg";
            onClicked: {
                pauseMessage.restart();
            }
        }

        Button{
            id: iconButton;

            anchors.verticalCenter:  parent.verticalCenter;

            style: iconButtonDecorator;

            iconSource: "../../../Icons/" + Style.theme + "/Eye.svg";
            text: "Icon button";
            onClicked: {
                pauseMessage.restart();
            }
        }
    }


    Component{
        id: textButtonDecorator;

        TextButtonDecorator{
            width: 140;
            height: 30;
            radius: 4;
        }
    }

    Component{
        id: roundButtonDecorator;

        IconButtonDecorator{
            width: 30;
            height: width;
            radius: width;
            icon.width: 12;
            icon.height: 12;
        }
    }

    Component{
        id: iconButtonDecorator;

        IconButtonDecorator{
            width: 140;
            height: 30;
            radius: 4;
            icon.width: 20;
            icon.height: 20;
        }
    }

    PauseAnimation {
        id: pauseMessage;

        duration: 1000;
        onStarted: {
            messageRec.visible = true;
        }
        onFinished: {
            messageRec.visible = false;
        }
    }

    Rectangle{
        id: darkBackground;

        anchors.fill: parent;

        color: "#000000";
        opacity: 0.3;
        visible: messageRec.visible;

    }

    Rectangle{
        id: messageRec;

        anchors.centerIn: parent;

        width: 100;
        height: 40;
        radius:4;

        visible: false;

        Text {
            id: messageText;

            anchors.centerIn: parent;

            width: parent.width - 80;
            wrapMode: Text.WordWrap;
            horizontalAlignment: Text.AlignHCenter;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_title;
            color: Style.errorTextColor;

            text: "Click!";
        }

    }
}
