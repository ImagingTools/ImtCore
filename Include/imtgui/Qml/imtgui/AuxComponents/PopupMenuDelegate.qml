import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: popupMenuDelegate;

    property int textSize: Style.fontSize_common;
    property string fontColor: Style.textColor;
    property string mainColor: "transparent";
    property string selectedColor: Style.selectedColor;

    property alias text: mainText.text;
    property Item rootItem;
    property bool textCentered: false;

    signal clicked(string commandId, int index);

    Component.onCompleted: {
        console.log("DEBUG::", model.Id, model.index, width, height, visible, x, y, z, parent)
    }

    Rectangle{
        id: background;

        anchors.fill: parent;
        color: popupMenuDelegate.mainColor;
    }

    Rectangle {
        anchors.fill: parent;
        color: popupMenuDelegate.selectedColor;
        visible: mouseArea.containsMouse;
    }



    Item {
        id: iconItem2;

        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.verticalCenter: parent.verticalCenter;

        visible: icon2.source != "";
        width: icon2.source != "" ? 18 : 1;
        height: width;

        Image {
            id: icon2;

            anchors.fill: parent;

            source: model.IconSource == undefined ? "" : model.IconSource;
            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

    Text {
        id: mainText;

        anchors.left: icon2.source != "" ? iconItem2.right : parent.left;
        anchors.leftMargin: !popupMenuDelegate.textCentered ? 10 : (parent.width - width)/2;
        anchors.verticalCenter: parent.verticalCenter;

        text: model.Name;
        color: popupMenuDelegate.fontColor;
        font.pixelSize: popupMenuDelegate.textSize;
        font.family: Style.fontFamily;
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            console.log("DEBUG::2022-11-13")
            popupMenuDelegate.clicked(model.Id, model.index);
            popupMenuDelegate.rootItem.finished(model.Id, model.index)
        }
    }
}
