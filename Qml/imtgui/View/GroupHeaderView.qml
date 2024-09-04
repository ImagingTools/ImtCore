import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: root;

    height: titleText.height + topPadding + bottomPadding;

    color: "transparent";

    property bool opened: true;

    property GroupElementView groupView: null;
    property alias title: titleText.text;
    property int topPadding: 0
    property int bottomPadding: 0
    property Component controlComp: baseControlComp

    onOpenedChanged: {
        if (!groupView){
            console.error("property 'groupView' is invalid");
            return;
        }

        if (opened){
            animation.from = 0;
            animation.to = groupView.contentHeight;
        }
        else{
            animation.from = groupView.contentHeight;
            animation.to = 0;
        }

        animation.start();
    }

    onGroupViewChanged: {
        if (groupView){
            animation.target = groupView;
        }
    }

    NumberAnimation {
        id: animation;

        property: "height";
        duration: 200;
    }

    Loader {
        id: controlLoader;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;

        sourceComponent: root.controlComp
    }

    Component {
        id: baseControlComp

        Item {
            id: item;

            width: commands.width + commands.anchors.rightMargin;
            height: 30;

            TreeItemModel {
                id: commandsModel;

                property bool completed: false;

                Component.onCompleted: {
                    let index = commandsModel.insertNewItem();
                    commandsModel.setData("Id", "Up", index);
                    commandsModel.setData("Icon", "Icons/Up", index);
                    commandsModel.setData("TooltipText", qsTr("Up"), index);
                    commandsModel.setData("Visible", root.opened, index);
                    commandsModel.setData("IsEnabled", true, index);

                    index = commandsModel.insertNewItem();
                    commandsModel.setData("Id", "Down", index);
                    commandsModel.setData("Icon", "Icons/Down", index);
                    commandsModel.setData("TooltipText", qsTr("Down"), index);
                    commandsModel.setData("Visible", !root.opened, index);
                    commandsModel.setData("IsEnabled", true, index);

                    commands.commandModel = commandsModel
                }
            }

            SimpleCommandsDecorator {
                id: commands;

                anchors.right: parent.right;
                height: 30;
                visible: root.groupView != null

                onCommandActivated: {
                    if (commandId == "Up"){
                        root.opened = true
                        setCommandVisible("Up", false)
                        setCommandVisible("Down", true)
                    }
                    else if (commandId == "Down"){
                        root.opened = false
                        setCommandVisible("Up", true)
                        setCommandVisible("Down", false)
                    }

                }
            }
        }
    }

    Text {
        id: titleText;

        anchors.top: parent.top
        anchors.topMargin: root.topPadding
        anchors.left: parent.left;

        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_title;
    }
}


