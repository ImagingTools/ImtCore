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
    property int leftPadding: 0
    property int rightPadding: Style.size_mainMargin
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
        id: baseControlComp;

        ToolButton {
            id: openButton;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: root.rightPadding;

            height: 22;
            width: height;
            visible: root.groupView != null

            iconSource: root.opened
                        ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal)
                        : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
                root.opened = !root.opened;
            }
        }
    }

    Text {
        id: titleText;

        anchors.top: parent.top
        anchors.topMargin: root.topPadding
        anchors.left: parent.left;
        anchors.leftMargin: root.leftPadding

        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_title;
    }
}


