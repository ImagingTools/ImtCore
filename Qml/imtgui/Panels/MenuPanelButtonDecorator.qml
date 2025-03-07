import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.0

DecoratorBase {
    id: leftPanelElement;

    width: 87;
    height: 80;

    signal accepted(string text);
    signal clicked();

    property bool textIsCropped: helperText.width > description.width;

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
        id: marker;
        anchors.fill: leftPanelElement;
//        anchors.margins: Style.sizeSmallMargin;
        anchors.margins: 8;
        radius: Style.buttonRadius;
        color: leftPanelElement.baseElement.selected || leftPanelElement.baseElement.highlighted ? Style.backgroundColor2 : "transparent";
        border.width: 1;
        border.color: leftPanelElement.baseElement.selected ? Style.iconColorOnSelected : "transparent";
    }

    Rectangle {
        id: itemBody;

        anchors.verticalCenter: leftPanelElement.verticalCenter;
        anchors.left: leftPanelElement.left;
        anchors.right: leftPanelElement.right;
        height: image.height + description.height + description.anchors.topMargin;

        color: "transparent";

        Image {
            id: image;
            anchors.horizontalCenter: itemBody.horizontalCenter;
            anchors.top: itemBody.top;
            width: 30;
            height: width;
            fillMode: Image.PreserveAspectFit;
            source: !leftPanelElement.baseElement ? "" : leftPanelElement.baseElement.iconSource
            sourceSize.height: height;
            sourceSize.width: width;
        }

        Text {
            id: description;
            anchors.top: image.bottom;
            anchors.topMargin: Style.sizeMainMargin;
            anchors.left: itemBody.left;
            anchors.leftMargin: Style.sizeMainMargin;
            anchors.right: itemBody.right;
            anchors.rightMargin: Style.sizeMainMargin;
            color: leftPanelElement.baseElement.selected || leftPanelElement.baseElement.highlighted ? Style.iconColorOnSelected : Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeSmall;
            text: leftPanelElement.baseElement.text;
            horizontalAlignment: Text.AlignHCenter;
            elide: Text.ElideRight;
        }

        Text {
            id: helperText;
            font.family: Style.fontFamily;
            font.pixelSize: description.font.pixelSize;
            text: !leftPanelElement.baseElement ? "" : leftPanelElement.baseElement.text;
            wrapMode: Text.NoWrap;
            elide: Text.ElideRight;
            horizontalAlignment: Text.AlignHCenter;
            visible: false;
        }
    }

    DropShadow {
        id: dropShadow;
        anchors.fill: marker;
        z: marker.z-1
        horizontalOffset:ok ? 3 : 0;
        verticalOffset: ok ? 3 : 0;
        radius: ok ? 8 : 0;
        spread: 0;
        color: ok ? Style.shadowColor : "transparent";
        source: marker;
        visible: ok;

        property bool ok: leftPanelElement.baseElement && (leftPanelElement.baseElement.selected || leftPanelElement.baseElement.highlighted);

        Component.onCompleted: {
            if (Qt.platform.os === "web"){
                dropShadow.samples = 15;
            }
        }
    }
}
