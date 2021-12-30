import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: collectionView;
    height: 100;
    width: 100;
    color: "transparent";
//    property alias tabPanel: tabPanelInternal;
//    property TabPanel tabPanel: tabPanelInternal;
    property AuxTable table: tableInternal;
    property string gqlModelInfo;
    property string gqlModelItems;
    property string itemId;
    signal selectItem(string itemId, string name);

//    color : Style.baseColor;

    onGqlModelInfoChanged: {
        headerInfoModel.updateModel()
    }

    AuxTable {
        id: tableInternal;
        anchors.fill: parent;
//        width: parent.width;
//        anchors.top: tabPanelInternal.bottom;
//        anchors.bottom: parent.bottom;
        onSelectItem: {
            collectionView.selectItem(itemId, name);
        }
    }



    GqlModel {
        id: headerInfoModel;

        function updateModel() {
            console.log( "headerInfoModel update");

            var query = Gql.GqlRequest("query", collectionView.gqlModelInfo);
            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, headerInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal = headerInfoModel.GetData("data");
                if(dataModelLocal.ContainsKey(collectionView.gqlModelInfo)){
                    dataModelLocal = dataModelLocal.GetData(collectionView.gqlModelInfo)
                    if(dataModelLocal.ContainsKey("headers")){
                        dataModelLocal = dataModelLocal.GetData("headers")
                        for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                            collectionView.table.addToHeadersArray(dataModelLocal.GetData("Id",i), dataModelLocal.GetData("Name",i));
                        }

                        itemsModel.updateModel()
                    }
                    else if(packageInfoModel.ContainsKey("errors")){
                        var errorsModel = packageInfoModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(collectionView.gqlModelInfo)){
                            console.log("message", errorsModel.GetData(collectionView.gqlModelInfo).GetData("message"));
                        }
                    }
                }
            }
        }
    }



    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", collectionView.gqlModelItems);

            if(collectionView.itemId != ""){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", collectionView.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("items");            

            queryFields.InsertField("Id");
            for (var i = 0; i < collectionView.table.headerKeysArray.length; i++){
                queryFields.InsertField(collectionView.table.headerKeysArray[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageView query ");
            console.log("PackageView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                var dataModelLocal = itemsModel.GetData("data");
                if(dataModelLocal.ContainsKey(collectionView.gqlModelItems)){
                    dataModelLocal = dataModelLocal.GetData(collectionView.gqlModelItems);
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
                        //                        console.log("items",dataModelLocal);
                        collectionView.table.elementsModel = dataModelLocal;
                    }
                    else if(itemsModel.ContainsKey("errors")){
                        var errorsModel = itemsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(collectionView.gqlModelItems)){
                            console.log("message", errorsModel.GetData(collectionView.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
