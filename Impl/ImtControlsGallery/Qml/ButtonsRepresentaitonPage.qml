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
        iconButton.forceActiveFocus();
        iconButton.focus = true;
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


    Row{
        id: buttonsRow;

        anchors.centerIn: parent;
        spacing: 50;



        Button{
            id: textButton;

            anchors.verticalCenter:  parent.verticalCenter;

            decoratorComponent: buttonsRepresentaitonPage.isQtStyle?
                                    DecoratorsQt.buttonDecorator : Decorators.buttonDecorator;

            KeyNavigation.right: roundButton
            KeyNavigation.tab: roundButton

            text: "Text button";
            onClicked: {
                pauseMessage.show("Text button clicked!")
            }
        }

        Button{
            id: roundButton;

            anchors.verticalCenter:  parent.verticalCenter;

            KeyNavigation.right: iconButton
            KeyNavigation.tab: iconButton

            decoratorComponent: buttonsRepresentaitonPage.isQtStyle?
                                    DecoratorsQt.roundButtonDecorator : Decorators.roundButtonDecorator;

            iconSource: "../../../" + Style.theme + "/Icons" + "/Close.svg";
            onClicked: {
                pauseMessage.show("Round button clicked!")

            }
        }

        Button{
            id: iconButton;

            anchors.verticalCenter:  parent.verticalCenter;

            KeyNavigation.right: toggledButton
            KeyNavigation.tab: toggledButton

            decoratorComponent: buttonsRepresentaitonPage.isQtStyle?
                                    DecoratorsQt.buttonDecorator : Decorators.buttonDecorator;

            iconSource: "../../../" + Style.theme + "/Icons" + "/Eye.svg";

            text: "Icon button";
            onClicked: {
                pauseMessage.show("Icon button clicked!")
            }
        }

        Rectangle{
            height: 50
            width: 200
            anchors.verticalCenter:  parent.verticalCenter;
            color: Style.backgroundColor

            Button{
                id: toggledButton;
                anchors.centerIn: parent

                KeyNavigation.right: textButton
                KeyNavigation.tab: textButton

                decoratorComponent: buttonsRepresentaitonPage.isQtStyle?
                                        DecoratorsQt.buttonDecorator : Decorators.buttonDecorator;


                checkable: true

                iconSource: "../../../" + Style.theme + "/Icons" + "/Eye.svg";
                text: "Toggled button";

                onClicked: {

                }

                onToggled: {
                    if(checked){
                        pauseMessage.show("toggled on!")
                    }
                    else {
                        pauseMessage.show("toggled off!")
                    }
                }
            }
        }


    }


    ///////////////////

    Component{
        id: toolButtonDecorator;

        ToolButtonDecorator{

        }
    }

    Component{
        id: topButtonDecorator;

        TopButtonDecorator{
        }
    }

    Component{
        id: topButtonDecoratorNew;

        TopButtonDecoratorNew{
        }
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
            //console.log("onStarted", messageRec.visible)
            messageRec.visible = true;
        }
        onFinished: {
            //console.log("onFinished", messageRec.visible)
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
