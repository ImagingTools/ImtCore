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

    property alias stateModel: container.itemsInfoModel.state;

    property int offset: 0;
    property int count: -1;

    property TreeItemModel filterModel: TreeItemModel {};

    signal modelUpdated();
    signal failed();

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

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", container.offset);
            viewParams.InsertField("Count", container.count);

            var jsonString = container.filterModel.toJSON();
            viewParams.InsertField("FilterModel", jsonString);
            console.log( "gqlModelBaseContainer filterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            let keys = Object.keys(externInputParams)
            for (let key of keys){
                inputParams.InsertField(key, externInputParams[key]);
            }
            inputParams.InsertFieldObject(viewParams);

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

                        container.modelUpdated();

                        container.completed = true;
                    }
                }
            }
            else if (this.state === "Error"){
                container.failed();

                container.completed = true;
            }
        }
    }
}
