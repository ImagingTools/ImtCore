import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: container;
    x: resultItem.getMenuButtonsX();
    y: resultItem.getMenuButtonsY();
    width: container.itemWidth;
    height: model.count * container.itemHeight - container.emptyItemCount * container.itemHeight;
    color: Style.baseColor;
    radius: 3;
    clip: true;

    layer.enabled: true;
    layer.effect: DropShadow {
        transparentBorder: true;
        horizontalOffset: 1;
        verticalOffset: 1;
        color: Style.textColor;
        spread: 0.05;
    }
    property Item resultItem;
    property var model;
    property var backgroundOpacity;
    property bool backgroundExist: false;
    property bool centered: false;

    property int itemWidth: 100;
    property int itemHeight: 26;

    property int emptyItemCount: 0;


    function exit(status) {
        var parameters  = {};
        parameters["status"] = status;
        container.resultItem.dialogResult(parameters);
    }

    onModelChanged: {
        console.log("PopupMenuDialog ListView onModelChanged", container.model.count);
        for (var i = 0; i < container.model.count; i++) {
            if (container.model.get(i).id === "") {
                container.emptyItemCount++;
            }
        }
        console.log("PopupMenuDialog container.emptyItemCount", container.emptyItemCount);
    }

    ListView {
        id: listViewPopup;
        anchors.fill: parent;
        anchors.margins: 1;
        model: container.model;
        boundsBehavior: Flickable.StopAtBounds;
        delegate: Rectangle {
            id: delegateListViewPopup;
            width: container.width;
            height: model.id !== "" ? container.itemHeight : 0;
            color: "transparent";
            Rectangle {
                id: highlightRect;
                anchors.fill: parent;
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
                //source: "../../../" + "Icons/" + Style.theme + "/" + model.text + "_" + "On" + "_" + "Normal" + ".svg";
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
                    container.exit(model.name);
                    loaderDialog.closeItem();
                }
            }
        }
    }

    ListModel {
        id: popupModel;
    }
}
