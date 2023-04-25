import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel collectionModel: TreeItemModel {};

    property string commandId;

    // Fields to get from server
    property var fields: [];

    signal modelUpdated();

    function updateModel(inputParams){
        if (container.commandId == ""){
            console.log( "ERROR: CollectionDataProvider commandId is empty!");

            return;
        }

        if (!inputParams){
            inputParams = {}
        }

        itemsInfoModel.updateModel(inputParams, container.fields);
    }

    Component.onDestruction: {
        if (container.commandId !== ""){
            Events.unSubscribeEvent(container.commandId + "CollectionUpdated", container.updateModel);
        }
    }

    onCommandIdChanged: {
        if (container.commandId !== ""){
            Events.subscribeEvent(container.commandId + "CollectionUpdated", container.updateModel);
        }
    }

    GqlModel {
        id: itemsInfoModel;

        function updateModel(externInputParams, fields) {
            console.log( "gqlModelBaseContainer updateModel", container.commandId + "List");
            var query = Gql.GqlRequest("query", container.commandId + "List");

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
                    if (dataModelLocal.ContainsKey(container.commandId + "List")){
                        dataModelLocal = dataModelLocal.GetData(container.commandId + "List");
                        if (dataModelLocal.ContainsKey("items")){
                            container.collectionModel = dataModelLocal.GetData("items");
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");
                            if (dataModelLocal.ContainsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.GetData("PagesCount");
                            }
                        }

                        container.modelUpdated();
                    }
                }
            }
        }
    }
}
