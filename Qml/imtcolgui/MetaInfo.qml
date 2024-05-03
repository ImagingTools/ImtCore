import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor2;

    clip: true;

    property TreeItemModel metaInfoModel: TreeItemModel {}
    property int elementHeight: 20;
    property bool contentVisible: true;

    function startLoading(){
        loading.start();
    }

    function stopLoading(){
        loading.stop();
    }

    Flickable {
        id: collectionMetaInfo;

        anchors.fill: parent;

        contentWidth: width;
        contentHeight: column.height;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: column;

            anchors.top: parent.top;
            anchors.topMargin: Style.size_mainMargin;
            anchors.left: parent.left;
            anchors.leftMargin: Style.size_mainMargin;
            anchors.right: parent.right;
            anchors.rightMargin: Style.size_mainMargin;

            visible: container.contentVisible;

            Repeater {
                id: repeaterColumn;

                model: container.metaInfoModel;

                delegate: Item {
                    id: repeaterTitle;

                    width: column.width;
                    height: childColumn.height + nameTitle.height + container.elementHeight;

                    Component.onCompleted: {
                        repeaterChilds.model = repeaterColumn.model.GetData("Children", model.index);
                    }

                    Text {
                        id: nameTitle;

                        width: column.width;

//                        anchors.horizontalCenter: parent.horizontalCenter;
//                        anchors.left: parent.left;
//                        anchors.leftMargin: Style.size_mainMargin;

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

                        width: column.width;

                        Repeater {
                            id: repeaterChilds;

                            width: column.width;

                            delegate: Rectangle {
                                height: container.elementHeight > valueText.contentHeight ? container.elementHeight : valueText.contentHeight;
                                width: column.width;

                                color: "transparent";

                                clip: true;

                                Image {
                                    id: icon

                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left;
                                    anchors.leftMargin: width > 0 ? Style.size_mainMargin : 0;

                                    width: model.Icon === undefined ? 0 : Style.iconSizeSmall
                                    height: width

                                    sourceSize.width: width
                                    sourceSize.height: height
                                    source: model.Icon === undefined ? "" : "../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal)
                                }

                                Text {
                                    id: valueText;

                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: icon.right;
                                    anchors.leftMargin: icon.width == 0 ? 0 : Style.size_mainMargin;
                                    anchors.right: parent.right;
//                                    anchors.rightMargin: Style.size_mainMargin;

                                    font.family: Style.fontFamily;
                                    font.pixelSize: Style.fontSize_small;
                                    wrapMode: Text.WordWrap

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

    Rectangle {
        anchors.fill: parent;
        color: parent.color;
        visible: loading.visible;
    }

    Loading {
        id: loading;

        anchors.top: parent.top;

        width: parent.width;
        height: 200;

        indicatorSize: 20;

        color: container.color;

        visible: false;
    }
}

