import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: popupMenuDelegate;

    property int textSize: Style.fontSize_common;
    property string fontColor: Style.textColor;
    property alias text: mainText.text;

    signal clicked(string commandId, int index);


    MouseArea {
        id: mouseArea;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {

            popupMenuDelegate.clicked(model.Id, model.index);
            finished(model.Id, model.index);
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: Style.selectedColor;
        visible: mouseArea.containsMouse;
    }

    Item {
        id: iconItem2;

        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.verticalCenter: parent.verticalCenter;

        width: icon2.source != "" ? 18 : 0;
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

        anchors.left: iconItem2.right;
        anchors.leftMargin: 10;
        anchors.verticalCenter: parent.verticalCenter;

        text: model.Name;
        color: popupMenuDelegate.fontColor;
        font.pixelSize: popupMenuDelegate.textSize;
        font.family: Style.fontFamily;
    }
}
