import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: leftPanelElement;

    width: 90;
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

        width: 60;
        height: 45;

        fillMode: Image.PreserveAspectFit;

        source: parent.imageSource;
        sourceSize.height: height;
        sourceSize.width: width;
    }

    Rectangle {
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;

        width: 5;

        color: Style.iconColorOnSelected;
        visible: parent.selected;
    }

    Item {
        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width - 12;
        height: description.height;

        Text {
            id: description;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.horizontalCenter: parent.horizontalCenter;
            width: parent.width;
            wrapMode: Text.WordWrap;
            horizontalAlignment: Text.AlignHCenter;

            elide: Text.ElideRight;

            color: parent.selected ? Style.iconColorOnSelected: Style.textColor;
            font.pixelSize: Style.fontSize_small;
            font.family: Style.fontFamily;

            text: leftPanelElement.title;
        }
    }
}
