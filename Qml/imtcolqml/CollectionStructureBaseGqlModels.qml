import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: gqlModelBaseContainer;

    property string getItemsGqlCommand;
    property string getObjectViewGqlCommand;
    property string objectViewEditorPath;
    property string objectViewEditorCommandsId;
    property string commandId;

    property var additionInputParams: ({})

    property TreeItemModel items: TreeItemModel {};
    property TreeItemModel notificationModel: null;

    property Item rootItem: null;

    // Поля которые запросятся для модели коллекции
    property var fieldsData: ["Id", "Name"]

    signal itemsInfoGqlStateChanged(string state);

    onCommandIdChanged: {
        if (commandId !== ""){
            getItemsGqlCommand = commandId + "List";
            getObjectViewGqlCommand = commandId + "ObjectView";
        }
    }

    function updateModels(){
        console.log("CollectionView updateModels");
        if (commandId === ""){
            console.error("Update model was failed! Command-ID is invalid.");
            return;
        }

        gqlModelBaseContainer.objectViewModel.getObjectView();
    }

    function updateItemsModel(){
        if (commandId === ""){
            console.error("Update model was failed! Command-ID is invalid.");
            return;
        }

        gqlModelBaseContainer.itemsInfoModel.updateModel();
    }


    property GqlModel itemsInfoModel: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.getItemsGqlCommand);
            let count = -1;
            let offset = 0;

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");

            let filterModel = gqlModelBaseContainer.rootItem.modelFilter;
            var jsonString = filterModel.toJSON();
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);
            if (Object.keys(gqlModelBaseContainer.additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in gqlModelBaseContainer.additionInputParams){
                    additionParams.InsertField(key, gqlModelBaseContainer.additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            for(var i = 0; i < gqlModelBaseContainer.fieldsData.length; i++){
                queryFields.InsertField(gqlModelBaseContainer.fieldsData[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            console.log("gqlData", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.itemsInfoModel);
            gqlModelBaseContainer.itemsInfoGqlStateChanged(this.state);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (gqlModelBaseContainer.itemsInfoModel.ContainsKey("errors")){
                    dataModelLocal = gqlModelBaseContainer.itemsInfoModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.getItemsGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.getItemsGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                    return;
                }

                if (gqlModelBaseContainer.itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = gqlModelBaseContainer.itemsInfoModel.GetData("data");
                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.getItemsGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.getItemsGqlCommand);
                        if (dataModelLocal.ContainsKey("notification")){
                            gqlModelBaseContainer.notificationModel = dataModelLocal.GetData("notification");
                        }

                        if (!dataModelLocal.ContainsKey("items")){
                            dataModelLocal.AddTreeModel("items")
                        }

                        if (dataModelLocal.ContainsKey("items")){

                            gqlModelBaseContainer.items.Clear();
                            gqlModelBaseContainer.items = dataModelLocal.GetData("items");

                            Events.sendEvent(gqlModelBaseContainer.commandId + "StructureUpdated");
                        }
                    }
                }
            }
        }
    }

    property GqlModel objectViewModel: GqlModel {

        function getObjectView(){
            console.log( "CollectionStructure objectView");
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.getObjectViewGqlCommand);

            if (Object.keys(gqlModelBaseContainer.additionInputParams).length > 0){
                let inputParams = Gql.GqlObject("input");
                let additionParams = Gql.GqlObject("addition");
                for (let key in gqlModelBaseContainer.additionInputParams){
                    additionParams.InsertField(key, gqlModelBaseContainer.additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("objectView");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionStructure objectView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.objectViewModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (gqlModelBaseContainer.objectViewModel.ContainsKey("errors")){
                    dataModelLocal = gqlModelBaseContainer.objectViewModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.getObjectViewGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.getObjectViewGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                    return;
                }

                dataModelLocal = gqlModelBaseContainer.objectViewModel.GetData("data");
                if(!dataModelLocal)
                    return;
                if (dataModelLocal.ContainsKey(gqlModelBaseContainer.getObjectViewGqlCommand)){
                    dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.getObjectViewGqlCommand);

                    let objectView = dataModelLocal.GetData("Path");
                    let objectViewCommandsId = dataModelLocal.GetData("CommandId");

                    gqlModelBaseContainer.objectViewEditorPath = objectView;
                    gqlModelBaseContainer.objectViewEditorCommandsId = objectViewCommandsId;
                }
            }
        }
    }
}
