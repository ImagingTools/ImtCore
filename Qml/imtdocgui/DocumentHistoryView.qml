import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property TreeItemModel historyModel: TreeItemModel {}

    property int elementHeight: 20;

    onHistoryModelChanged: {
        repeaterColumn.model = container.historyModel;
    }

    DocumentHistoryProvider {
        id: documentHistoryProvider;

        onHistoryModelChanged: {
            container.historyModel = documentHistoryProvider.historyModel;
        }

        onStateChanged: {
            if (state === "Loading"){
                loading.visible = true;
            }
            else{
                loading.visible = false;
            }
        }
    }

    function updateModel(documentTypeId, documentId){
        documentHistoryProvider.updateModel(documentTypeId, documentId);
    }

    Component {
        id: delegateComp;

        Column {
            width: parent.width;

            Item {
                width: parent.width;
                height: 30;

                Text {
                    id: nameTitle;

                    width: parent.width;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;

                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamilyBold;
                    font.bold: true;

                    color: Style.lightBlueColor;
                    elide: Text.ElideRight;
                    wrapMode: Text.WrapAnywhere ;

                    text: model.OwnerName + ", " + model.Time;
                }
            }

            Text {
                width: parent.width;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_small;

                color: Style.textColor;

                wrapMode: Text.WrapAnywhere;

                text: model.OperationDescription;
            }
        }
    }

    Flickable {
        id: flickable;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        width: parent.width;

        contentWidth: width;
        contentHeight: column.height;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: column;

            anchors.left: parent.left;
            anchors.leftMargin: Style.size_mainMargin;
            anchors.right: parent.right;
            anchors.rightMargin: Style.size_mainMargin;

            spacing: 2 * Style.size_mainMargin;

            Repeater {
                id: repeaterColumn;

                delegate: delegateComp;
            }
        }//Column main
    }

    CustomScrollbar {
        id: scrollbar;

        anchors.right: flickable.right;
        anchors.rightMargin: 5;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;

        radius: 2;
    }

    Text {
        anchors.centerIn: parent;

        font.pixelSize: Style.fontSize_common;
        font.family: Style.fontFamilyBold;
        font.bold: true;

        color: Style.textColor;

        wrapMode: Text.WrapAnywhere;

        text: qsTr("There is no history for this document");

        visible: repeaterColumn.count === 0;
    }

    Loading {
        id: loading;

        anchors.fill: parent;
    }
}

