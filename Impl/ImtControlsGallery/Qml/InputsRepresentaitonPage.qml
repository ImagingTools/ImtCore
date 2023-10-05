import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0



Rectangle {
    id: buttonsRepresentaitonPage;

    anchors.fill: parent;


    property string styleName: "Qt";
    property bool isQtStyle: false;

    Component.onCompleted: {
        Events.subscribeEvent("QtStyle", buttonsRepresentaitonPage.setQtStyle);
        Events.subscribeEvent("AcfStyle", buttonsRepresentaitonPage.setAcfStyle);

    }

    Component.onDestruction: {
        Events.unSubscribeEvent("QtStyle", buttonsRepresentaitonPage.setQtStyle);
        Events.unSubscribeEvent("AcfStyle", buttonsRepresentaitonPage.setAcfStyle);

    }

    function setQtStyle(){
        buttonsRepresentaitonPage.isQtStyle = true;
    }

    function setAcfStyle(){
        buttonsRepresentaitonPage.isQtStyle = false;

    }

    TextField{
        anchors.centerIn: parent;
        width: 200;
        height: 30;
    }


    ///////////////////

    PauseAnimation {
        id: pauseMessage;
        function show(text) {
            messageRec.text = text
            pauseMessage.restart()
        }

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
        visible: false;

    }

    Rectangle{
        id: messageRec;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top
        anchors.topMargin: 50

        property alias text: messageText.text

        width: parent.width - 50;
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
