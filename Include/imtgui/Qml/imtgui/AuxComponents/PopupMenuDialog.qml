import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtqml 1.0

Item {
    id: popupMenuContainer;

    width: popupMenuContainer.itemWidth;
    height: columnPopupMenu.height;

    property Item resultItem;
    property Item loaderDialog;

    property var model;
    property real backgroundOpacity: 0;

    property bool backgroundExist: false;
    property bool clickBackgroundClose: true;
    property bool centered: false;
    property bool hasIcon: true;

    property int itemWidth: 100;
    property int itemHeight: 26;

    property int emptyItemCount: 0;

    function exit(status) {
        var parameters  = {};
        parameters["status"] = status;
        parameters["dialog"] = "PopupMenu";
        popupMenuContainer.resultItem.dialogResult(parameters);
    }

    onResultItemChanged: {
        console.log("PopupMenuDialog onResultItemChanged", popupMenuContainer.resultItem);
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

    DropShadow {
       id: dropShadow;

       anchors.fill: mainBody;

       horizontalOffset: 2;
       verticalOffset: 2;

       radius: 4;
       samples: 10;
       color: Style.shadowColor;
       source: mainBody;

//       opacity: 0.5;
    }

    Rectangle {
        id: mainBody;

        anchors.fill: parent;

        color: Style.baseColor;
        radius: 3;

        border.width: 1;
        border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

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
                        anchors.verticalCenter: parent.verticalCenter;

                        width: parent.width - 2;
                        height: parent.height - 2;

                        color: Style.selectedColor;
                        visible: delegateListViewPopupMA.containsMouse && model.id !== "" && model.mode !== "Disabled";
                    }

                    Text {
                        anchors.left: popupMenuContainer.hasIcon ? iconDelegateListViewPopup.right : delegateListViewPopup.left;
                        anchors.verticalCenter: iconDelegateListViewPopup.verticalCenter;
                        anchors.leftMargin: 10;
                        anchors.rightMargin: 10;

                        text: model.name;
                        color: model.mode === "Disabled" ? Style.disabledInActiveTextColor : Style.textColor;
                        font.pixelSize: Style.fontSize_common;
                        font.family: Style.fontFamily;
                    }

                    Image {
                        id: iconDelegateListViewPopup;

                        anchors.left: delegateListViewPopup.left;
                        anchors.verticalCenter: delegateListViewPopup.verticalCenter;
                        anchors.leftMargin: 10;

                        width: 18;
                        height: width;

                        visible: popupMenuContainer.hasIcon && model.name !== "";
                        source: model.imageSource;
                        sourceSize.width: width;
                        sourceSize.height: height;
                    }

                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter;
                        anchors.bottom: delegateListViewPopup.bottom;

                        width: parent.width - 20;
                        height: 1;

                        visible: model.id === "";

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
