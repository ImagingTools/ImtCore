import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtqml 1.0

Item {
    id: popupMenuContainer;
//    x: 100;
//    y: 100;
    x: popupMenuContainer.resultItem.getMenuButtonsX();
    y: popupMenuContainer.resultItem.getMenuButtonsY();
    width: popupMenuContainer.itemWidth;
    height: columnPopupMenu.height;
//    clip: true;


    property Item resultItem;
    property var model;
    property var backgroundOpacity;
    property bool backgroundExist: false;
    property bool clickBackgroundClose: true;
    property bool centered: false;

    property int itemWidth: 100;
    property int itemHeight: 26;

    property int emptyItemCount: 0;

    function exit(status) {
        var parameters  = {};
        parameters["status"] = status;
        popupMenuContainer.resultItem.dialogResult(parameters);
    }

    onModelChanged: {
        console.log("PopupMenuDialog ListView onModelChanged", popupMenuContainer.model.count);
        for (var i = 0; i < popupMenuContainer.model.count; i++) {
            if (popupMenuContainer.model.get(i).id === "") {
                popupMenuContainer.emptyItemCount++;
            }
        }
        console.log("PopupMenuDialog popupMenuContainer.emptyItemCount", popupMenuContainer.emptyItemCount);
    }

//    DropShadow {
//        anchors.fill: popupMenuContainer;

//        horizontalOffset: 5;
//        verticalOffset: 5;

//        radius: 5;
//        samples: 5;
//        color: "#80000000";
//        source: mainBody;
//    }

    Rectangle {
        id: mainBody;

        anchors.fill: parent;

        color: Style.baseColor;
        radius: 3;

        Column {
            id: columnPopupMenu;
            width: parent.width;

            Repeater {
                model: popupMenuContainer.model;

                delegate: Rectangle {
                    id: delegateListViewPopup;
                    width: popupMenuContainer.width;
                    height: model.id !== "" ? popupMenuContainer.itemHeight : 0;
                    color: "transparent";
                    Rectangle {
                        id: highlightRect;
                        anchors.horizontalCenter: parent.horizontalCenter;

                        width: parent.width - 1;
                        height: parent.height;

                        color: Style.selectedColor;
                        visible: delegateListViewPopupMA.containsMouse && model.id !== "" && model.mode !== "Disabled";
                    }

                    Text {
                        text: model.name;
                        color: model.mode === "Disabled" ? Style.disabledInActiveTextColor : Style.textColor;
                        font.pixelSize: Style.fontSize_common;
                        font.family: Style.fontFamily;

                        anchors.left: iconDelegateListViewPopup.right;
                        anchors.verticalCenter: iconDelegateListViewPopup.verticalCenter;
                        anchors.leftMargin: 10;
                        anchors.rightMargin: 10;
                    }

                    Image {
                        id: iconDelegateListViewPopup;
                        width: 18;
                        height: width;
                        visible: model.name !== "";
                        //source: "../../../Icons//_On_Normal.svg";
                        source: model.imageSource;

                        anchors.left: delegateListViewPopup.left;
                        anchors.verticalCenter: delegateListViewPopup.verticalCenter;
                        anchors.leftMargin: 10;
                    }

                    Rectangle {
                        width: parent.width - 20;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        anchors.bottom: delegateListViewPopup.bottom;
                        //anchors.topMargin: 5;
                        visible: model.id === "";
                        height: 1;
                        color: Style.hover;
                    }

                    MouseArea {
                        id: delegateListViewPopupMA;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                        visible: model.mode !== "Disabled";

                        onClicked: {
                            popupMenuContainer.exit(model.name);
                            loaderDialog.closeItem();
                        }
                    }
                }
            }
        }
    }
}
