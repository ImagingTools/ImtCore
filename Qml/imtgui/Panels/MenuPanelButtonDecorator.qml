import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: leftPanelElement;

    width: 100;
    height: 80;

    property string title: "";
    property string imageSource: "";
    property bool visibleMarker: false;


    property bool highlighted;
    property bool selected: !leftPanelElement.baseElement ? false : leftPanelElement.baseElement.selected;
    property string text;

    signal accepted(string text);
    signal clicked();


    onWidthChanged: {
        if(leftPanelElement.baseElement){
            leftPanelElement.baseElement.contentWidth = width;
        }
    }

    onHeightChanged: {
        if(leftPanelElement.baseElement){
            leftPanelElement.baseElement.contentHeight = height;
        }
    }

    Rectangle {
        id: indicator;

        anchors.top: leftPanelElement.top;
        anchors.bottom: leftPanelElement.bottom;
        anchors.left: leftPanelElement.left;

        width: 5;

        color: Style.iconColorOnSelected;
        visible: leftPanelElement.selected;
    }

    Item{
        id: body;

        anchors.verticalCenter: leftPanelElement.verticalCenter;
        anchors.left: leftPanelElement.left;
        anchors.right: leftPanelElement.right;

        height: image.height + description.height + description.anchors.topMargin;

        Image {
            id: image;

            anchors.horizontalCenter: body.horizontalCenter;
            anchors.top: body.top;

            width: 60;
            height: 45;

            fillMode: Image.PreserveAspectFit;

            source: !leftPanelElement.baseElement ? "" : leftPanelElement.baseElement.iconSource;
            sourceSize.height: height;
            sourceSize.width: width;
        }

        Text {
            id: description;

            anchors.top: image.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.topMargin: 10;
            anchors.leftMargin:  indicator.width;
            anchors.rightMargin: indicator.width;

            color: leftPanelElement.selected ? Style.iconColorOnSelected: Style.textColor;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_small;

            text: !leftPanelElement.baseElement ? "" : leftPanelElement.baseElement.text;
            wrapMode: Text.NoWrap;
            elide: Text.ElideRight;
            horizontalAlignment: Text.AlignHCenter;
        }
    }
}
