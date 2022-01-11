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
    property TreeItemModel model;
    property alias table: tableInternal;
    property string gqlModelInfo;
    property string gqlModelItems;
    property string itemId;
    signal selectItem(string itemId, string name);

//    color : Style.baseColor;
    onModelChanged: {
        console.log("collectionView onModelChanged", collectionView.gqlModelInfo)
        if (collectionView.model.ContainsKey("headers")){
            var dataModelLocal = collectionView.model.GetData("headers");
            tableInternal.headersModel = dataModelLocal;
//            tableInternal.headersModel.Refresh();
            console.log("collectionView header count",tableInternal.headersModel.GetItemsCount())
        }
        else{
            headerInfoModel.updateModel()
        }

        if (collectionView.model.ContainsKey("data")){
            var dataModelLocal = collectionView.model.GetData("data");
            tableInternal.elementsModel = dataModelLocal;
            console.log("collectionView data count",dataModelLocal.GetItemsCount())
        }

    }

    function menuActivated(menuId) {
        var itemId = tableInternal.getSelectedId();
        var name = tableInternal.getSelectedName();
        if (itemId != "" && name != ""){
            collectionView.selectItem(itemId, name);
        }
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
            console.log( "headerInfoModel update", collectionView.gqlModelInfo);

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
                        tableInternal.headersModel = dataModelLocal.GetData("headers")
                        console.log("headerInfoModel ContainsKey",dataModelLocal.ContainsKey("headers"))
                        collectionView.model.SetExternTreeModel('headers',tableInternal.headersModel )
                        itemsModel.updateModel()
                    }
                    else if(packageInfoModel.ContainsKey("errors")){
                        var errorsModelLocal = packageInfoModel.GetData("errors");
                        if(errorsModelLocal !== null && errorsModelLocal.ContainsKey(collectionView.gqlModelInfo)){
                            console.log("message", errorsModelLocal.GetData(collectionView.gqlModelInfo).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "collectionView updateModel");

            var query = Gql.GqlRequest("query", collectionView.gqlModelItems);

            if(collectionView.itemId != ""){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", collectionView.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("items");            

            queryFields.InsertField("Id");
            for(var i = 0; i < tableInternal.headersModel.GetItemsCount(); i++){
                queryFields.InsertField(tableInternal.headersModel.GetData("Id",i));
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("collectionView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey(collectionView.gqlModelItems)){
                    dataModelLocal = dataModelLocal.GetData(collectionView.gqlModelItems);
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        tableInternal.elementsModel = dataModelLocal.GetData("items");
                        console.log("collectionView items",dataModelLocal);
                        collectionView.model.SetExternTreeModel('data',tableInternal.elementsModel)
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
