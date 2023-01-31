import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property string gqlModelMetaInfo;

    property var modelData;

    property int elementHeight: 20;

    property bool contentVisible: true;

    property Item tableData: null;

    function getMetaInfo(){
        metaInfo.getMetaInfo();
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

            visible: container.contentVisible;

            Repeater {
                id: repeaterColumn;

                model: container.modelData;

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

    GqlModel {
        id: metaInfo;

        function getMetaInfo(){
            console.log( "CollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", container.gqlModelMetaInfo);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.tableData.getSelectedId());
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("metaInfo");
            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageCollectionView metaInfo query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, metaInfo);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (metaInfo.ContainsKey("errors")){
                    return;
                }

                if (metaInfo.ContainsKey("data")){
                    dataModelLocal = metaInfo.GetData("data");

                    if (dataModelLocal.ContainsKey(container.gqlModelMetaInfo)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlModelMetaInfo);

                        container.modelData = dataModelLocal;
                    }
                }
            }
        }
    }//MetaInfo
}

