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
                id: colorCommandsModel;

                property bool completed: false;

                Component.onCompleted: {

                    let index = colorCommandsModel.insertNewItem();
                    colorCommandsModel.setData("Id", "Up", index);
                    colorCommandsModel.setData("Icon", "Icons/Up", index);
                    colorCommandsModel.setData("TooltipText", qsTr("Up"), index);
                    colorCommandsModel.setData("Visible", !root.opened, index);

                    index = colorCommandsModel.insertNewItem();
                    colorCommandsModel.setData("Id", "Down", index);
                    colorCommandsModel.setData("Icon", "Icons/Down", index);
                    colorCommandsModel.setData("TooltipText", qsTr("Down"), index);
                    colorCommandsModel.setData("Visible", root.opened, index);

                    commands.commandModel = colorCommandsModel
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

    // ToolButton {
    //     id: openButton;

    //     anchors.verticalCenter: parent.verticalCenter;
    //     anchors.right: parent.right;
    //     anchors.rightMargin: Style.size_mainMargin;

    //     height: 22;
    //     width: height;
    //     visible: root.groupView != null

    //     iconSource: root.opened
    //                 ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal)
    //                 : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

    //     onClicked: {
    //         root.opened = !root.opened;
    //     }
    // }

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


