import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: container;
    width: 250;
    height: listViewPopup.count * 30 + 2;
    border.color: Style.textColor;
    color: Style.baseColor;
    clip: true;
    property alias model: listViewPopup.model;

    ListView {
        id: listViewPopup;
        anchors.fill: parent;
        anchors.margins: 1;
        model: popupModel;
        delegate: Rectangle {
            id: delegateListViewPopup;
            width: parent.width;
            height: 30;

            Rectangle {
                id: highlightRect;
                anchors.fill: parent;
                color: Style.selectedColor;
                visible: delegateListViewPopupMA.containsMouse;
            }

            Text {
                text: model.text;
                color: Style.textColor;
                font.pixelSize: Style.fontSize_subtitle;
                font.family: Style.fontFamily;

                anchors.left: iconDelegateListViewPopup.right;
                anchors.verticalCenter: iconDelegateListViewPopup.verticalCenter;
                anchors.leftMargin: 10;
            }

            Image {
                id: iconDelegateListViewPopup;
                width: 18;
                height: width;
                visible: true;
                source: "../../../" + "Icons/" + Style.theme + "/" + model.text + "_" + "On" + "_" + "Normal" + ".svg";

                anchors.left: delegateListViewPopup.left;
                anchors.verticalCenter: delegateListViewPopup.verticalCenter;
                anchors.leftMargin: 5;
            }

            Rectangle {
                width: parent.width - 20;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: iconDelegateListViewPopup.bottom;
                anchors.topMargin: 5;
                visible: model.index === 1;
                height: 1;
                color: "gray";
            }

            MouseArea {
                id: delegateListViewPopupMA;
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                onClicked: {
                    if (model.text === "Edit"){
                        editFeatureDialog.visible = true;
                    }
                }
            }
        }
    }

    ListModel {
        id: popupModel;
        ListElement {
            text: "Edit";
        }

        ListElement {
            text: "Remove";
        }

        ListElement {
            text: "Set Description";
        }

        ListElement {
            text: "Rename";
        }
    }
}
