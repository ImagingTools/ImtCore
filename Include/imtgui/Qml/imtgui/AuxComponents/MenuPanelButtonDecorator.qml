import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: leftPanelElement;

    width: 80;
    height: 80;

    property string title: "";
    property string imageSource: "";
    property bool visibleMarker: false;

    signal accepted(string text);
    signal clicked();

    property bool highlighted;
    property bool selected: false;
    property string text;

    Image {
        id: image;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 5;

        width: 50;
        height: 50;

        fillMode: Image.PreserveAspectFit;

        source: container.imageSource;
        sourceSize.height: height;
        sourceSize.width: width;
    }

    Rectangle {
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;

        width: 5;

        color: Style.iconColorOnSelected;
        visible: container.selected;
    }

    Text {
        id: description;

        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        text: container.text;
        color: container.selected ? Style.iconColorOnSelected: Style.textColor;
        font.pixelSize: container.fontSize;
        font.family: Style.fontFamily;
    }
}
