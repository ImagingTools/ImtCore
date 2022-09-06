import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: collectionMetaInfo;

    height: parent.height;
    width: 200;

    color: Style.backgroundColor;

    property string gqlModelMetaInfo;

    property var modelData;

    property int elementHeight: 20;

    property bool contentVisible: true;

    property Item tableData;

    onModelDataChanged: {
        console.log("MetaInfo onModelDataChanged", collectionMetaInfo.modelData);
    }

    Column {
        id: column;

        anchors.fill: parent;
        anchors.rightMargin: 5;
        anchors.leftMargin: 5;
        anchors.topMargin: 5;

        visible: collectionMetaInfo.contentVisible;

        Repeater {
            id: repeaterColumn;

            model: collectionMetaInfo.modelData;

            delegate: Item {
                id: repeaterTitle;

                width: collectionMetaInfo.width;
                height: childColumn.height + nameTitle.height + collectionMetaInfo.elementHeight;

                Component.onCompleted: {
                    repeaterChilds.model = repeaterColumn.model.GetData("Children", model.index);
                }

                Text {
                    id: nameTitle;

                    width: column.width;

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    text: model.Name;

                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamilyBold;
                    font.bold: true;

                    color: Style.lightBlueColor;
                    elide: Text.ElideRight;
                    wrapMode: Text.WrapAnywhere ;
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

    function getMetaInfo(){
        metaInfo.getMetaInfo();
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo(){
            console.log( "CollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", collectionMetaInfo.gqlModelMetaInfo);
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id", collectionMetaInfo.tableData.getSelectedId());

            var queryFields = Gql.GqlObject("metaInfo");
            query.AddParam(inputParams);

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

                    if (dataModelLocal.ContainsKey(collectionMetaInfo.gqlModelMetaInfo)){
                        dataModelLocal = dataModelLocal.GetData(collectionMetaInfo.gqlModelMetaInfo);

                        if (dataModelLocal.ContainsKey("metaInfo")){
                            dataModelLocal = dataModelLocal.GetData("metaInfo");
                            collectionMetaInfo.modelData = dataModelLocal;
                        }
                    }
                }
            }
        }
    }//MetaInfo
}
