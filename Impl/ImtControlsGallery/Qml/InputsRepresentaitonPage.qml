import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0



Rectangle {
    id: inputsRepresentaitonPage;

    anchors.fill: parent;


    property string styleName: "Qt";
    property bool isQtStyle: false;

    Component.onCompleted: {
        Events.subscribeEvent("QtStyle", inputsRepresentaitonPage.setQtStyle);
        Events.subscribeEvent("AcfStyle", inputsRepresentaitonPage.setAcfStyle);
        //textField.setFocus(true);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("QtStyle", inputsRepresentaitonPage.setQtStyle);
        Events.unSubscribeEvent("AcfStyle", inputsRepresentaitonPage.setAcfStyle);

    }

    function setQtStyle(){
        inputsRepresentaitonPage.isQtStyle = true;
    }

    function setAcfStyle(){
        inputsRepresentaitonPage.isQtStyle = false;

    }

    Row{
        id: buttonsRow;

        anchors.centerIn: parent;
        spacing: 50;

        TextField{
            id: textField;

            anchors.verticalCenter: parent.verticalCenter;

            decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.textFieldDecorator :
                                                                  Decorators.textFieldDecorator;
            placeHolderText: "placeHolder";
        }

        Rectangle{
            width: textEdit.width + 20;
            height: textEdit.height + 20;
            anchors.verticalCenter:  parent.verticalCenter;
            color: Style.backgroundColor;

            TextEditCustom{
                id: textEdit;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.horizontalCenter: parent.horizontalCenter;

                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.textEditDecorator :
                                                                      Decorators.textEditDecorator;

                placeHolderText: "placeHolder";
            }
        }

    }//row


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
