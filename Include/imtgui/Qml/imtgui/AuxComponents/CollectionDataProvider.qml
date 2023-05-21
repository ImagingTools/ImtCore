import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property TreeItemModel collectionModel: TreeItemModel {};

    property string commandId;

    // Property indicating whether the model is ready for use
    property bool completed: false;

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

        container.itemsInfoModel.updateModel(inputParams, container.fields);
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

    function onModelUpdated(){
        if (!container.completed){
            container.completed = true;
        }
    }

    property GqlModel itemsInfoModel: GqlModel {
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

            container.completed = false;
            console.log("gqlModelBaseContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, container.itemsInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (container.itemsInfoModel.ContainsKey("errors")){
                    return;
                }

                if (container.itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = container.itemsInfoModel.GetData("data");
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

//                        container.onModelUpdated();

                        container.modelUpdated();

                        container.completed = true;
                    }
                }
            }
        }
    }
}
