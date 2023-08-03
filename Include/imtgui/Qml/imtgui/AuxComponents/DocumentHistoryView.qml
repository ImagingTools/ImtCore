import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property TreeItemModel historyModel: TreeItemModel {}

    property int elementHeight: 20;

    onHistoryModelChanged: {
        repeaterColumn.model = container.historyModel;
    }

    Component {
        id: delegateComp;

        Column {
            width: parent.width;

            Component.onCompleted: {
                changesRepeater.model = model.ChangesModel;
            }

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

                    text: model.User + ", " + model.DateTime;
                }
            }

            Column {
                width: parent.width;

                Repeater {
                    id: changesRepeater;

                    delegate: Text {
                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_small;

                        color: Style.textColor;

                        wrapMode: Text.WrapAnywhere;

                        text: model.Change;
                    }
                }
            }
        }
    }

    Flickable {
        id: collectionMetaInfo;

        anchors.fill: parent;

        contentWidth: width;
        contentHeight: column.height;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: column;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.leftMargin: 5;

            spacing: 20;

            Repeater {
                id: repeaterColumn;

                delegate: delegateComp;
            }
        }//Column main
    }
}

