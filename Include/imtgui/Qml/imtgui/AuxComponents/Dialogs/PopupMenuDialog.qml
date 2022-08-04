import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0

Rectangle {
    id: popupMenuContainer;

    width: popupMenuContainer.itemWidth;
    height: popupMenuListView.height;

    property var model;
    property Item root;

    property int itemWidth: 200;
    property int itemHeight: 26;

    signal finished(string commandId, int index);

    onFinished: {
        root.closeDialog();
    }

    onModelChanged: {
        popupMenuListView.model = model;
    }

    onRootChanged: {
        /**
            Opacity of the background = 0
        */
        root.backgroundItem.opacity = 0;

        /**
            Close the dialog by clicking on the background
        */
        root.backgroundItem.backgroundAreaItem.clicked.connect(root.closeDialog);
    }

    Rectangle {
        id: itemBody;

        width: popupMenuListView.width;
        height: popupMenuListView.height;

        color: Style.baseColor;

        border.width: 1;
        border.color: Style.alternateBaseColor;

        ListView {
            id: popupMenuListView;

            width: popupMenuContainer.itemWidth;
            height: popupMenuListView.count * popupMenuContainer.itemHeight;

            delegate: Item {
                width: popupMenuContainer.width;
                height: popupMenuContainer.itemHeight;

                MouseArea {
                    id: mouseArea;

                    anchors.fill: parent;

                    hoverEnabled: true;
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                    onClicked: {
                        finished(model.Id, model.index);
                    }
                }

                Rectangle {
                    anchors.fill: parent;
                    color: Style.selectedColor;
                    visible: mouseArea.containsMouse;
                }

                Item {
                    id: iconItem;

                    anchors.left: parent.left;
                    anchors.leftMargin: 5;
                    anchors.verticalCenter: parent.verticalCenter;

                    width: 18;
                    height: width;

                    Image {
                        id: icon;

                        anchors.fill: parent;

                        source: model.IconSource;
                        sourceSize.width: width;
                        sourceSize.height: height;
                    }
                }

                Text {
                    anchors.left: iconItem.right;
                    anchors.leftMargin: 10;
                    anchors.verticalCenter: parent.verticalCenter;

                    text: model.Name;
                    color: Style.textColor;
                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamily;
                }
            }// Delegate Item
        }//ListView
    }//ItemListView

    DropShadow {
       id: dropShadow;

       anchors.fill: itemBody;

       horizontalOffset: 2;
       verticalOffset: 2;

       radius: 4;
       color: Style.shadowColor;

       source: itemBody;
    }
}
