import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.0

DecoratorBase {
    id: leftPanelElement;

    width: 85;
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
        anchors.margins: Style.size_smallMargin;
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
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            width: 30;
            height: width;
            fillMode: Image.PreserveAspectFit;
            source: !leftPanelElement.baseElement ? "" : leftPanelElement.baseElement.iconSource
            sourceSize.height: height;
            sourceSize.width: width;
        }

        Text {
            id: description;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: image.bottom;
            anchors.topMargin: Style.size_mainMargin;
            anchors.left: parent.left;
            anchors.leftMargin: Style.size_mainMargin;
            anchors.right: parent.right;
            anchors.rightMargin: Style.size_mainMargin;
            color: leftPanelElement.baseElement.selected || leftPanelElement.baseElement.highlighted ? Style.iconColorOnSelected : Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_small;
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
                dropShadow.samples = 17;
            }
        }
    }

    TooltipArea {
        id: tooltipArea;
        anchors.fill: parent;
        mouseArea: !leftPanelElement.baseElement ? null : leftPanelElement.baseElement.mouseArea;
        text: !leftPanelElement.baseElement || !leftPanelElement.textIsCropped ? "" : leftPanelElement.baseElement.text;
    }
}
