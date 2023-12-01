import QtQuick 2.12
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
    property Item rootItem: null;
    property Item menuItem: null;
    property bool textCentered: false;

    property bool selected: !rootItem ? false : rootItem.selectedIndex == model.index ;
    property bool isSeparator: model.Id === "Separator";

    signal clicked(string commandId, int index);

    Component.onCompleted: {
        //console.log("DEBUG::", model.Id, model.index, popupMenuDelegate.width, popupMenuDelegate.height, popupMenuDelegate.visible, popupMenuDelegate.x, popupMenuDelegate.y, popupMenuDelegate.z, popupMenuDelegate.parent)
    }

    onIsSeparatorChanged: {
        if (isSeparator){
            mouseArea.visible = false;

            popupMenuDelegate.height = 5;
        }
    }

    Rectangle{
        id: background;

        anchors.fill: parent;
        color: popupMenuDelegate.rootItem.currentIndex == model.index ? popupMenuDelegate.selectedColor : popupMenuDelegate.mainColor;
    }

    Rectangle {
        id: separator;

        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        anchors.verticalCenter: parent.verticalCenter;

//        width: parent.width;
        height: 1;

        color: popupMenuDelegate.fontColor;

        visible: isSeparator;
    }

    Rectangle {
        anchors.fill: parent;

        color: popupMenuDelegate.selected ? popupMenuDelegate.selectedColor  : "transparent";
        visible: true;
        opacity: 0.5;
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

        anchors.left: !popupMenuDelegate.textCentered ? icon2.source != "" ? iconItem2.right : parent.left : undefined;
        anchors.leftMargin: !popupMenuDelegate.textCentered ? 10 : 0;
        anchors.right: !popupMenuDelegate.textCentered ? parent.right : undefined;
        anchors.rightMargin: !popupMenuDelegate.textCentered ? 10 : 0;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.horizontalCenter: popupMenuDelegate.textCentered ? parent.horizontalCenter : undefined;

        color: popupMenuDelegate.fontColor;
        font.pixelSize: popupMenuDelegate.textSize;
        font.family: Style.fontFamily;

//        text: model.Name;

        text: model[popupMenuDelegate.rootItem.nameId];
        elide: Text.ElideRight;
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        visible: !popupMenuDelegate.rootItem ? true : !popupMenuDelegate.rootItem.hoverBlocked;
        onEntered: {
            if(popupMenuDelegate.rootItem && popupMenuDelegate.rootItem.selectedIndex !== undefined)
            {
                popupMenuDelegate.rootItem.selectedIndex = model.index;
            }
        }

        onClicked: {
            //console.log("DEBUG::2022-11-13")
            popupMenuDelegate.clicked(model.Id, model.index);
            popupMenuDelegate.rootItem.finished(model.Id, model.index)
        }
    }
}
