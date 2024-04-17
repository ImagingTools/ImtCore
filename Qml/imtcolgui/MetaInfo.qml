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
                        anchors.topMargin: Style.size_smallMargin;

                        width: collectionMetaInfo.width;

                        Repeater {
                            id: repeaterChilds;

                            width: collectionMetaInfo.width;

                            delegate: Rectangle {
                                height: container.elementHeight > valueText.contentHeight ? container.elementHeight : valueText.contentHeight;
                                width: collectionMetaInfo.width;

                                color: "transparent";

                                Image {
                                    id: icon
                                    anchors.left: parent.left;
                                    anchors.leftMargin: Style.size_mainMargin;
                                    anchors.verticalCenter: parent.verticalCenter

                                    width: model.Icon === undefined ? 0 : Style.iconSizeSmall
                                    height: width
                                    sourceSize.width: width
                                    sourceSize.height: height
                                    source: model.Icon === undefined ? "" : "../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal)
                                }

                                Text {
                                    id: valueText;

                                    anchors.left: icon.right;
                                    anchors.leftMargin: icon.width == 0 ? 0 : Style.size_mainMargin;
                                    anchors.right: parent.right;
                                    anchors.rightMargin: Style.size_mainMargin;
                                    anchors.verticalCenter: parent.verticalCenter

                                    font.family: Style.fontFamily;
                                    font.pixelSize: Style.fontSize_small;
                                    wrapMode: Text.WordWrap

                                    color: Style.textColor;
                                    // elide: Text.ElideRight;

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

