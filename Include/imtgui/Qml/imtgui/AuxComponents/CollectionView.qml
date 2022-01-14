import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: collectionViewContainer;
    height: 100;
    width: 100;
    color: "transparent";
    property TreeItemModel model;
    property alias table: tableInternal;
    property int selectedIndex: -1;
    property string gqlModelInfo;
    property string gqlModelItems;
    property string itemId;
    signal selectItem(string itemId, string name);

    onModelChanged: {
        console.log("collectionViewContainer onModelChanged", collectionViewContainer.gqlModelInfo)
        if (collectionViewContainer.model.ContainsKey("headers")){
            var dataModelLocal = collectionViewContainer.model.GetData("headers");
            tableInternal.headers = dataModelLocal;
            console.log("collectionViewContainer header count",tableInternal.headers.GetItemsCount())
        }
        else{
            headerInfoModel.updateModel()
        }

        if (collectionViewContainer.model.ContainsKey("data")){
            var dataModelLocal = collectionViewContainer.model.GetData("data");
            tableInternal.elements = dataModelLocal;

            var selectedIndexLocal = collectionViewContainer.model.GetData("selectedIndex");
            tableInternal.selectedIndex = selectedIndexLocal;
            console.log("collectionViewContainer data count",dataModelLocal.GetItemsCount())
        }

    }

    function menuActivated(menuId) {
        console.log("CollectionView menuActivated", menuId);

        //console.log("CollectionView itemId ", itemId, "name", name);

        if (menuId  === "New"){
            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Edit") {
            var itemId = tableInternal.getSelectedId();
            var name = tableInternal.getSelectedName();
            if (itemId != "" && name != ""){
                collectionViewContainer.selectItem(itemId, name);
            }
        }
    }

    AuxTable {
        id: tableInternal;
        anchors.fill: parent;
        onSelectItem: {
            collectionViewContainer.selectItem(itemId, name);
        }

        onSelectedIndexChanged: {
            console.log("collectionViewContainer.selectedIndex 1", collectionViewContainer.selectedIndex, tableInternal.selectedIndex);
            collectionViewContainer.selectedIndex = tableInternal.selectedIndex;
            collectionViewContainer.model.SetData("selectedIndex", tableInternal.selectedIndex);
            console.log("collectionViewContainer.selectedIndex 2", collectionViewContainer.selectedIndex, tableInternal.selectedIndex);
        }
    }

    GqlModel {
        id: headerInfoModel;

        function updateModel() {
            console.log( "headerInfoModel update", collectionViewContainer.gqlModelInfo);

            var query = Gql.GqlRequest("query", collectionViewContainer.gqlModelInfo);
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
                if(dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                    dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo)
                    if(dataModelLocal.ContainsKey("headers")){
                        tableInternal.headers = dataModelLocal.GetData("headers")
                        console.log("headerInfoModel ContainsKey",dataModelLocal.ContainsKey("headers"))
                        collectionViewContainer.model.SetExternTreeModel('headers',tableInternal.headers )
                        itemsModel.updateModel()
                    }
                    else if(packageInfoModel.ContainsKey("errors")){
                        var errorsModelLocal = packageInfoModel.GetData("errors");
                        if(errorsModelLocal !== null && errorsModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                            console.log("message", errorsModelLocal.GetData(collectionViewContainer.gqlModelInfo).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "collectionViewContainer updateModel");

            var query = Gql.GqlRequest("query", collectionViewContainer.gqlModelItems);

            if(collectionViewContainer.itemId != ""){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", collectionViewContainer.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("items");            

            queryFields.InsertField("Id");
            for(var i = 0; i < tableInternal.headers.GetItemsCount(); i++){
                queryFields.InsertField(tableInternal.headers.GetData("Id",i));
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("collectionViewContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){
                    dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        tableInternal.elements = dataModelLocal.GetData("items");
                        console.log("collectionViewContainer items",dataModelLocal);
                        collectionViewContainer.model.SetExternTreeModel('data', tableInternal.elements)
                    }
                    else if(itemsModel.ContainsKey("errors")){
                        var errorsModel = itemsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(collectionViewContainer.gqlModelItems)){
                            console.log("message", errorsModel.GetData(collectionViewContainer.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
