import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0



Rectangle {
    id: indicatorsRepresentaitonPage;

    anchors.fill: parent;


    property string styleName: "Qt";
    property bool isQtStyle: false;

    Component.onCompleted: {
        Events.subscribeEvent("QtStyle", indicatorsRepresentaitonPage.setQtStyle);
        Events.subscribeEvent("AcfStyle", indicatorsRepresentaitonPage.setAcfStyle);

    }

    Component.onDestruction: {
        Events.unSubscribeEvent("QtStyle", indicatorsRepresentaitonPage.setQtStyle);
        Events.unSubscribeEvent("AcfStyle", indicatorsRepresentaitonPage.setAcfStyle);

    }

    function setQtStyle(){
        indicatorsRepresentaitonPage.isQtStyle = true;
    }

    function setAcfStyle(){
        indicatorsRepresentaitonPage.isQtStyle = false;
    }

    Column{
        id: column;

        anchors.centerIn: parent;

        width: parent.width;
        spacing: 100;

        Row{
            id: progressBarRow;

            anchors.horizontalCenter: parent.horizontalCenter;

            height:  100;
            spacing: 100;

            ProgressBar{
                id: progressBar1;

                anchors.verticalCenter:  parent.verticalCenter;

                from: 10; to: 110;
                value: 40;
                text: value + " единиц (" + percent + " %)";

                decoratorComponent: indicatorsRepresentaitonPage.isQtStyle ? DecoratorsQt.progressBarDecorator :
                                                                      Decorators.progressBarDecorator;

            }

            ProgressBar{
                id: progressBar2;

                anchors.verticalCenter:  parent.verticalCenter;

                from: 10; to: 110;
                value: 60;
                indeterminate: true;
                text: percent + " %";

                decoratorComponent: indicatorsRepresentaitonPage.isQtStyle ? DecoratorsQt.progressBarDecorator :
                                                                      Decorators.progressBarDecorator;

            }

            ProgressBar{
                id: progressBar3;

                anchors.bottom:  parent.bottom;
                anchors.bottomMargin: indicatorsRepresentaitonPage.isQtStyle ? (parent.height/2 - height/2) : 0;

                width: 150;
                from: 10; to: 110;
                value: 60;
                hasText: false;
                hasTitle: false;
                text: percent + " %";

                decoratorComponent: indicatorsRepresentaitonPage.isQtStyle ? DecoratorsQt.progressBarDecorator :
                                                                      Decorators.progressBarDecorator;

            }


        }//progressBarRow

//        Item{//testElement
//            id: testElement;

//            //anchors.verticalCenter: parent.verticalCenter;
//            anchors.horizontalCenter: parent.horizontalCenter;

//            width: decorator ? decorator.width : 0;
//            height: decorator ? decorator.height : 0;

//            //                property Component decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.switchDecorator :
//            //                                                                                            Decorators.switchDecorator;

//            property Component decoratorComponent : DecoratorsQt.progressBarDecorator;
//            property var decorator : null;

//            onDecoratorComponentChanged: {
//                if(!decoratorComponent){
//                    return;
//                }
//                if(decorator){
//                    decorator.destroy()
//                }
//                decorator = decoratorComponent.createObject(testElement)
//                decorator.baseElement = testElement
//                bindWidth.target = decorator
//                bindHeight.target = decorator
//            }

//            Binding {
//                id: bindWidth
//                property: "width"
//                value: testElement.width;
//            }

//            Binding {
//                id: bindHeight
//                property: "height"
//                value: testElement.height;
//            }

//        }//testElement

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
