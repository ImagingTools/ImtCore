import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: collectionMetaInfo;

    height: parent.height;
    width: 200;

    color: Style.backgroundColor;

    property var modelData;

    property int elementHeight: 20;

    property bool contentVisible: true;

    onModelDataChanged: {
        console.log("MetaInfo onModelDataChanged", collectionMetaInfo.modelData);
    }

    Column {
        id: column;

        anchors.top: collectionMetaInfo.top;
        anchors.topMargin: 5;

        visible: collectionMetaInfo.contentVisible;

        width: collectionMetaInfo.width;

        Repeater {
            id: repeaterColumn;

            model: collectionMetaInfo.modelData;

            delegate: Rectangle {
                id: repeaterTitle;

                width: collectionMetaInfo.width;
                height: childColumn.height + nameTitle.height + collectionMetaInfo.elementHeight;

                color: "transparent";

                Component.onCompleted: {
                    console.log("MetaInfo Component.onCompleted", model.Childs);
                    repeaterChilds.model = repeaterColumn.model.GetData("Childs", model.index);
                }

                Text {
                    id: nameTitle;

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    text: model.Name;

                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamilyBold;
                    font.bold: true;

                    color: Style.lightBlueColor;
                    elide: Text.ElideRight;
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

                            height: collectionMetaInfo.elementHeight;
                            width: collectionMetaInfo.width;

                            color: "transparent";

                            Text {
                                id: valueText;

                                anchors.left: parent.left;
                                anchors.leftMargin: 10;

                                text: model.Value;

                                font.family: Style.fontFamily;
                                font.pixelSize: Style.fontSize_small;

                                color: Style.textColor;
                                elide: Text.ElideRight;
                            }
                        }
                    }
                }
            }
        }
    }
}
