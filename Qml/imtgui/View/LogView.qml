import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: root;

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Item {
        id: topPanel;

        anchors.top: parent.top;

        width: parent.width;
        height: 35;

        Text {
            anchors.verticalCenter: parent.verticalCenter;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeXSmall;

            text: qsTr("Log");
        }
    }

    function updateGui(){
        if (root.model.containsKey("Text")){
            textContent.text = root.model.getData("Text")
        }
        else{
            textContent.text = "";
        }
    }

    CustomScrollbar {
        id: scrollbar;

        anchors.left: flickable.right;
        anchors.leftMargin: 5;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        secondSize: 10;
        targetItem: flickable;

        visible: parent.visible;
    }

    Flickable {
        id: flickable;

        anchors.fill: content;

        contentWidth: textContent.width;
        contentHeight: textContent.height;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;
    }

    Rectangle {
        id: content;

        anchors.top: topPanel.bottom;
        anchors.bottom: parent.bottom;

        width: parent.width;

        color: Style.baseColor;

        Text {
            id: textContent;

            anchors.fill: parent;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeXSmall;

            lineHeight: 2;
        }
    }
}


