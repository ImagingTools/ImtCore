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

    Column{
        id: column;

        anchors.centerIn: parent;

        width: parent.width;
        spacing: 100;

        Rectangle{
            id: splitViewContainer;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: splitView.orientation == Qt.Horizontal ? 800 : 200;
            height: splitView.orientation == Qt.Horizontal ? 200 : 800;
            color: "transparent";

            SplitView{
                id: splitView;

                anchors.centerIn: parent;

                width: parent.width;
                height: parent.height;

                //orientation: Qt.Vertical;//!!!

                Component.onCompleted: {
                    //sizeModel.SetData("MinimumWidth", 50, 0);
                    sizeModel.SetData("FillWidth", true, 0);
                }

                Rectangle{
                    id: red;

                    width: splitView.orientation == Qt.Horizontal ? 100 : 0;
                    height: splitView.orientation == Qt.Vertical ? 100 : 0;

                    color: "red";
                    Text {
                        anchors.centerIn: parent;
                        text: "red";
                    }
                }

                Rectangle{
                    id: green;

                    width: splitView.orientation == Qt.Horizontal ? 200 : 0;
                    height: splitView.orientation == Qt.Vertical ? 200 : 0;
                    color: "green";
                    Text {
                        anchors.centerIn: parent;
                        text: "green";
                    }
                }

                Rectangle{
                    id: yellow;

                    width: splitView.orientation == Qt.Horizontal ? 400 : 0;
                    height: splitView.orientation == Qt.Vertical ? 400 : 0;
                    color: "yellow";
                    Text {
                        anchors.centerIn: parent;
                        text: "yellow";
                    }
                }


            }//SplitView

        }//splitViewContainer


//        Item{//testElement
//            id: testElement;

//            //anchors.verticalCenter: parent.verticalCenter;
//            anchors.horizontalCenter: parent.horizontalCenter;

//            width: decorator ? decorator.width : 0;
//            height: decorator ? decorator.height : 0;

//            //                property Component decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.switchDecorator :
//            //                                                                                            Decorators.switchDecorator;

//            property Component decoratorComponent : DecoratorsQt.splitViewDecorator;
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
