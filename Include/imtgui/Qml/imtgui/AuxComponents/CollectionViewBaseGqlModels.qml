import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: gqlModelBaseContainer;

    property string getHeadersGqlCommand;
    property string getItemsGqlCommand;
    property string getObjectViewGqlCommand;
    property string objectViewEditorPath;
    property string objectViewEditorCommandsId;
    property string commandId;

    property var table;
    property var additionInputParams: ({})

    property TreeItemModel headers: TreeItemModel {};
    property TreeItemModel items: TreeItemModel {};
    property TreeItemModel notificationModel: null;

    property Item rootItem: null;
    property Item pagination: null;

    // Поля которые запросятся для модели коллекции
    property var fieldsData: ["Id", "Name"]

    signal itemsInfoGqlStateChanged(string state);

    onCommandIdChanged: {
        if (commandId !== ""){
            getHeadersGqlCommand = commandId + "Info";
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
        gqlModelBaseContainer.headerInfoModel.updateModel();
    }

    function updateItemsModel(){
        if (commandId === ""){
            console.error("Update model was failed! Command-ID is invalid.");
            return;
        }

        gqlModelBaseContainer.itemsInfoModel.updateModel();
    }

    property GqlModel headerInfoModel: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.getHeadersGqlCommand);

            if (Object.keys(gqlModelBaseContainer.additionInputParams).length > 0){
                let inputParams = Gql.GqlObject("input");
                let additionParams = Gql.GqlObject("addition");
                for (let key in gqlModelBaseContainer.additionInputParams){
                    additionParams.InsertField(key, gqlModelBaseContainer.additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
                query.AddParam(inputParams);
            }

            var queryHeaders = Gql.GqlObject("headers");
            queryHeaders.InsertField("Id");
            queryHeaders.InsertField("Name");
            query.AddField(queryHeaders);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.headerInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (gqlModelBaseContainer.headerInfoModel.ContainsKey("errors")){
                    dataModelLocal = gqlModelBaseContainer.headerInfoModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.getHeadersGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.getHeadersGqlCommand);
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

                if (gqlModelBaseContainer.headerInfoModel.ContainsKey("data")){
                    dataModelLocal = gqlModelBaseContainer.headerInfoModel.GetData("data");

                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.getHeadersGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.getHeadersGqlCommand);
                        if (!dataModelLocal){
                            return;
                        }

                        if (dataModelLocal.ContainsKey("FilterSearch")){
                            let filterSearchModel = dataModelLocal.GetData("FilterSearch")
                            gqlModelBaseContainer.rootItem.modelFilter.SetData("FilterIds", filterSearchModel);
                        }

                        if(dataModelLocal.ContainsKey("Headers")){
                            dataModelLocal = dataModelLocal.GetData("Headers");
                            gqlModelBaseContainer.headers = dataModelLocal;

                            for(var i = 0; i < gqlModelBaseContainer.headers.GetItemsCount(); i++){
                                let headerId = gqlModelBaseContainer.headers.GetData("Id", i);
                                if (!gqlModelBaseContainer.fieldsData.includes(headerId)){
                                    gqlModelBaseContainer.fieldsData.push(headerId);
                                }
                            }

                            gqlModelBaseContainer.itemsInfoModel.updateModel();
                        }
                    }
                }
            }
        }
    }

    property GqlModel itemsInfoModel: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.getItemsGqlCommand);
            let count = gqlModelBaseContainer.pagination.countElements;
            let offset = (gqlModelBaseContainer.pagination.currentIndex) * count;

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
                            if (!gqlModelBaseContainer.table){
                                return;
                            }

                            let indexes = gqlModelBaseContainer.table.getSelectedIndexes();
                            gqlModelBaseContainer.table.resetSelection();
                            gqlModelBaseContainer.items.Clear();
                            gqlModelBaseContainer.items = dataModelLocal.GetData("items");

                            if (indexes.length === 1){
                                let selectedIndex = indexes[0];
                                if (gqlModelBaseContainer.items.GetItemsCount() > selectedIndex){
                                    gqlModelBaseContainer.table.select(selectedIndex);
                                }
                            }

                            Events.sendEvent(gqlModelBaseContainer.commandId + "CollectionUpdated");
                        }
                    }
                }
            }
        }
    }

    property GqlModel objectViewModel: GqlModel {

        function getObjectView(){
            console.log( "CollectionView objectView");
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
            console.log("CollectionView objectView query ", gqlData);
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
