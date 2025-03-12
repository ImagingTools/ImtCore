import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: rootElement;

    width: 700;
    height: content.height + 2 * Style.sizeLargeMargin;

    border.width: 1;
    border.color: Style.borderColor;

    radius: 5;

    color: Style.baseColor;

    clip: true;

    property alias name: textName.text;
    property alias description: textDescription.text;

    property alias controlComp: controlLoader.sourceComponent;
    property alias controlItem: controlLoader.item;

    property alias topComp: topLoader.sourceComponent;
    property alias topItem: topLoader.item;

    property alias bottomComp: bottomLoader.sourceComponent;
    property alias bottomItem: bottomLoader.item;

    property bool topRoundedCorners: true;
    property bool bottomRoundedCorners: true;

    property alias contentWidth: content.width

    property alias nameWidth: textName.width;

    property int controlWidth: 400;
    property int controlHeight: 30;

    onFocusChanged: {
        if (focus && controlItem){
            controlItem.forceActiveFocus();
        }
    }

    Rectangle {
        id: topRect;

        anchors.top: parent.top;

        width: parent.width;
        height: rootElement.radius;

        visible: !rootElement.topRoundedCorners;

        border.width: rootElement.border.width;
        border.color: rootElement.border.color;

        color: rootElement.color;
    }

    Rectangle {
        anchors.left: parent.left;
        anchors.leftMargin: rootElement.border.width;
        anchors.right: parent.right;
        anchors.rightMargin: rootElement.border.width;
        anchors.top: topRect.bottom;
        anchors.topMargin: -height;

        height: rootElement.border.width;

        color: rootElement.color;
    }

    Rectangle {
        id: bottomRect;

        anchors.bottom: parent.bottom;

        width: parent.width;
        height: rootElement.radius;

        visible: !rootElement.bottomRoundedCorners;

        border.width: rootElement.border.width;
        border.color: rootElement.border.color;

        color: rootElement.color;
    }

    Rectangle {
        anchors.left: parent.left;
        anchors.leftMargin: rootElement.border.width;
        anchors.right: parent.right;
        anchors.rightMargin: rootElement.border.width;
        anchors.top: bottomRect.top;

        height: rootElement.border.width;

        color: rootElement.color;
    }

    Column {
        id: content;

        anchors.verticalCenter: parent.verticalCenter;

        anchors.left: parent.left;
        anchors.leftMargin: Style.sizeLargeMargin;

        anchors.right: parent.right;
        anchors.rightMargin: Style.sizeLargeMargin;

        clip: true;

        spacing: Style.sizeMainMargin;

        Loader {
            id: topLoader;
            width: parent.width;
            visible: rootElement.topComp != null && rootElement.topComp != undefined;
        }

        Item {
            id: mainPart;

            width: parent.width;
            height: Math.max(textName.height, controlLoader.height)

            visible: textName.text !== "" || controlLoader.item;

            Text {
                id: textName;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.right: controlLoader.left;
                anchors.rightMargin: Style.sizeLargeMargin;

                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSizeXLarge;

                elide: Text.ElideRight;
                wrapMode: Text.NoWrap;
            }

            Loader {
                id: controlLoader;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
            }
        }

        Item {
            id: descriptionPart;

            width: parent.width;
            height: textDescription.text !== "" ? textDescription.height: 0;

            visible: height > 0;

            Text {
                id: textDescription;

                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width;

                color: Style.inactiveTextColor
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSizeNormal;

                wrapMode: Text.WordWrap;
                elide: Text.ElideRight;
            }
        }

        Loader {
            id: bottomLoader;
            width: parent.width;
            visible: rootElement.bottomComp != null && rootElement.bottomComp != undefined;
        }
    }
}


