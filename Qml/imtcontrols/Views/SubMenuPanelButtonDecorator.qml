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

    property bool highlighted;
    property bool selected: false;
    property string text;

    signal accepted(string text);
    signal clicked();

    Rectangle {
        id: marker;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        width: 8;
        height: width;

        radius: 8;

        color: Style.iconColorOnSelected;
        visible: parent.selected;
    }

    Text {

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        color: parent.selected ? Style.iconColorOnSelected: Style.textColor;
        font.pixelSize: container.fontSize;
        font.family: Style.fontFamily;

        text: parent.title;
    }
}
