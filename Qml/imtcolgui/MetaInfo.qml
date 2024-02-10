import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property TreeItemModel metaInfoModel: TreeItemModel {}
    property int elementHeight: 20;
    property bool contentVisible: true;

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

            visible: container.contentVisible;

            Repeater {
                id: repeaterColumn;

                model: container.metaInfoModel;

                delegate: Item {
                    id: repeaterTitle;

                    width: collectionMetaInfo.width;
                    height: childColumn.height + nameTitle.height + container.elementHeight;

                    Component.onCompleted: {
                        repeaterChilds.model = repeaterColumn.model.GetData("Children", model.index);
                    }

                    Text {
                        id: nameTitle;

                        width: column.width;

                        anchors.left: parent.left;
                        anchors.leftMargin: 10;
                        anchors.horizontalCenter: parent.horizontalCenter;

                        font.pixelSize: Style.fontSize_common;
                        font.family: Style.fontFamilyBold;
                        font.bold: true;

                        color: Style.lightBlueColor;
                        elide: Text.ElideRight;
                        wrapMode: Text.WrapAnywhere ;

                        text: model.Name;
                    }

                    Column {
                        id: childColumn;

                        anchors.top: nameTitle.bottom;
                        anchors.topMargin: 5;

                        width: collectionMetaInfo.width;

                        Repeater {
                            id: repeaterChilds;

                            width: collectionMetaInfo.width;

                            delegate: Rectangle {

                                height: container.elementHeight;
                                width: collectionMetaInfo.width;

                                color: "transparent";

                                Text {
                                    id: valueText;

                                    anchors.left: parent.left;
                                    anchors.leftMargin: 10;
                                    anchors.right: parent.right;
                                    anchors.rightMargin: 10;

                                    font.family: Style.fontFamily;
                                    font.pixelSize: Style.fontSize_small;

                                    color: Style.textColor;
                                    elide: Text.ElideRight;

                                    text: model.Value;
                                }
                            }
                        }
                    }
                }
            }
        }//Column main
    }
}

