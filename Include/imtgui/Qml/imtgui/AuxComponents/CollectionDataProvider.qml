import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel collectionModel: null;

    property string commandId;

    function updateModel(inputParams, fields){
        if (commandId == ""){
            console.log( "ERROR: CollectionDataProvider commandId is empty!");

            return;
        }

        itemsInfoModel.updateModel(inputParams, fields);
    }

    GqlModel {
        id: itemsInfoModel;

        function updateModel(externInputParams, fields) {
            console.log( "gqlModelBaseContainer updateModel", commandId + "List");
            var query = Gql.GqlRequest("query", commandId + "List");

            var inputParams = Gql.GqlObject("input");
            let keys = Object.keys(externInputParams)
            for (let key of keys){
                inputParams.InsertField(key, externInputParams[key]);
            }
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            for (let key of fields){
                queryFields.InsertField(key);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("gqlModelBaseContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (itemsInfoModel.ContainsKey("errors")){
                    return;
                }

                if (itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = itemsInfoModel.GetData("data");
                    if (dataModelLocal.ContainsKey(commandId + "List")){
                        dataModelLocal = dataModelLocal.GetData(commandId + "List");
                        if (dataModelLocal.ContainsKey("items")){
                            container.collectionModel = dataModelLocal.GetData("items");
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");
                            if (dataModelLocal.ContainsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.GetData("PagesCount");
                            }
                        }
                    }
                }
            }
        }
    }
}
