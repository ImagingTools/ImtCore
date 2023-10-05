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
//        textButton.forceActiveFocus();
//        textButton.focus = true;
    }

    function setAcfStyle(){
        buttonsRepresentaitonPage.isQtStyle = false;
//        textButton.forceActiveFocus();
//        textButton.focus = true;

    }

    //    TopPanel {
    //        id: topPanel
    //        anchors.top: parent.top
    //        anchors.topMargin: 150
    //        width: parent.width
    //        //decoratorComponent: topPanelDecorator;

    //    }



    Row{
        id: buttonsRow;

        anchors.centerIn: parent;
        spacing: 50;



        Button{
            id: textButton;

            anchors.verticalCenter:  parent.verticalCenter;

            decoratorComponent: buttonsRepresentaitonPage.isQtStyle?
                                    DecoratorsQt.buttonDecorator : buttonDecorator;

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
                                    DecoratorsQt.roundButtonDecorator : roundButtonDecorator;

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
                                    DecoratorsQt.buttonDecorator : buttonDecorator;

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
                                        DecoratorsQt.buttonDecorator : buttonDecorator;


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


    Component{
        id: roundButtonDecorator;

        ButtonDecorator{
            width: 30;
            height: width;
            radius: width;
            //            icon.width: 12;
            //            icon.height: 12;
        }
    }

    Component{
        id: buttonDecorator;

        ButtonDecorator{
            width: 160;
            height: 30;
            radius: 4;
            //            icon.width: 20;
            //            icon.height: 20;

        }
    }

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

//    Component{
//        id: topPanelDecorator;

//        TopPanelDecorator{
//            Button {
//                id: but1
//                anchors.left: parent.left
//                anchors.leftMargin: Style.paddingSmall
//                KeyNavigation.right: but2
//                KeyNavigation.tab: but2
//                decoratorComponent: topButtonDecorator;
//                iconSource: "../../../Icons/" + Style.theme + "/Eye.svg";
//                text: "Top button";
//            }

//            Button {
//                id: but2
//                anchors.left: but1.right
//                anchors.leftMargin: Style.paddingLarge

//                KeyNavigation.right: but3
//                KeyNavigation.tab: but3
//                decoratorComponent: topButtonDecoratorNew;

//                iconSource: "../../../Icons/" + Style.theme + "/Eye.svg";
//                text: "Top button new";
//            }

//            Button {
//                id: but3
//                anchors.left: but2.right
//                anchors.leftMargin: Style.paddingLarge

//                KeyNavigation.right: but1
//                KeyNavigation.tab: but1
//                decoratorComponent: topButtonDecoratorNew;
//                checkable: true

//                iconSource: "../../../Icons/" + Style.theme + "/Eye.svg";
//                text: "Toggled button";
//            }

//            CommandsDecorator {
//                id: commandsDecorator;
//                anchors.left: but3.right
//                anchors.leftMargin: Style.paddingSmall
//                anchors.horizontalCenter: parent.horizontalCenter;
//                onVisibleChanged: {
//                    if (!commandsDecorator.visible){
//                        Events.unSubscribeEvent("CommandsModelChanged", commandsDecorator.setCommandsModel);
//                    }
//                }

//                TreeItemModel {
//                    id: commandsModel
//                    Component.onCompleted: {
//                        let index = commandsModel.InsertNewItem()
//                        commandsModel.SetData("Id", "edit", index);
//                        commandsModel.SetData("Name", "Edit", index);
//                        commandsModel.SetData("Description", "Edit description", index);
//                        commandsModel.SetData("IsEnabled", true, index);
//                        commandsModel.SetData("Visible", true, index);
//                        commandsModel.SetData("Icon", "Edit", index);
//                        commandsModel.SetData("Status", "", index);
//                        index = commandsModel.InsertNewItem()
//                        commandsModel.SetData("Id", "remove", index);
//                        commandsModel.SetData("Name", "Remove", index);
//                        commandsModel.SetData("Description", "Remove description", index);
//                        commandsModel.SetData("IsEnabled", true, index);
//                        commandsModel.SetData("Visible", true, index);
//                        commandsModel.SetData("Icon", "Remove", index);
//                        commandsModel.SetData("Status", "", index);
//                        commandsDecorator.model = commandsModel
//                        //                        let object = {}
//                        //                        object["Model"] = commandsModel
//                        //                        commandsDecorator.setCommandsModel(object)
//                    }
//                }

//            }
//            //            TopRightPanelDecorator{

//            //            }
//        }
//    }



    //    ListModel {
    //        id: singleSelectionModel
    //        property int selectedIndex
    //        function
    //    }


    //    ButtonGroup

    //     SegmentedButtons{
    //         model: singleSelectionModel
    //         onButtonSelected{

    //         }
    //     }

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
