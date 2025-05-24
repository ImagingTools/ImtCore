import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0

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
    property var fieldsData: ["id", "name"]

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
        function updateModel(){
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
            queryHeaders.InsertField("id");
            queryHeaders.InsertField("name");
            query.AddField(queryHeaders);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.headerInfoModel);

            if (this.state === "Ready"){
                var dataModelLocal;

                if (gqlModelBaseContainer.headerInfoModel.containsKey("errors")){
                    dataModelLocal = gqlModelBaseContainer.headerInfoModel.getData("errors");

                    if (dataModelLocal.containsKey(gqlModelBaseContainer.getHeadersGqlCommand)){
                        dataModelLocal = dataModelLocal.getData(gqlModelBaseContainer.getHeadersGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (gqlModelBaseContainer.headerInfoModel.containsKey("data")){
                    dataModelLocal = gqlModelBaseContainer.headerInfoModel.getData("data");

                    if (dataModelLocal.containsKey(gqlModelBaseContainer.getHeadersGqlCommand)){
                        dataModelLocal = dataModelLocal.getData(gqlModelBaseContainer.getHeadersGqlCommand);
                        if (!dataModelLocal){
                            return;
                        }

                        if (dataModelLocal.containsKey("FilterSearch")){
                            let filterSearchModel = dataModelLocal.getData("FilterSearch")
                            gqlModelBaseContainer.rootItem.modelFilter.setData("FilterIds", filterSearchModel);
                        }

                        if(dataModelLocal.containsKey("Headers")){
                            dataModelLocal = dataModelLocal.getData("Headers");
                            gqlModelBaseContainer.headers = dataModelLocal;

                            for(var i = 0; i < gqlModelBaseContainer.headers.getItemsCount(); i++){
                                let headerId = gqlModelBaseContainer.headers.getData("id", i);
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
        function updateModel(){
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.getItemsGqlCommand);
            let count = gqlModelBaseContainer.pagination.countElements;
            let offset = (gqlModelBaseContainer.pagination.currentIndex) * count;

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");

            let filterModel = gqlModelBaseContainer.rootItem.modelFilter;
            var jsonString = filterModel.toJson();
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

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.itemsInfoModel);
            gqlModelBaseContainer.itemsInfoGqlStateChanged(this.state);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (gqlModelBaseContainer.itemsInfoModel.containsKey("errors")){
                    dataModelLocal = gqlModelBaseContainer.itemsInfoModel.getData("errors");

                    if (dataModelLocal.containsKey(gqlModelBaseContainer.getItemsGqlCommand)){
                        dataModelLocal = dataModelLocal.getData(gqlModelBaseContainer.getItemsGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (gqlModelBaseContainer.itemsInfoModel.containsKey("data")){
                    dataModelLocal = gqlModelBaseContainer.itemsInfoModel.getData("data");
                    if (dataModelLocal.containsKey(gqlModelBaseContainer.getItemsGqlCommand)){
                        dataModelLocal = dataModelLocal.getData(gqlModelBaseContainer.getItemsGqlCommand);
                        if (dataModelLocal.containsKey("notification")){
                            gqlModelBaseContainer.notificationModel = dataModelLocal.getData("notification");
                        }

                        if (!dataModelLocal.containsKey("items")){
                            dataModelLocal.addTreeModel("items")
                        }

                        if (dataModelLocal.containsKey("items")){
                            if (!gqlModelBaseContainer.table){
                                return;
                            }

                            let indexes = gqlModelBaseContainer.table.getSelectedIndexes();
                            gqlModelBaseContainer.table.resetSelection();
                            gqlModelBaseContainer.items.clear();
                            gqlModelBaseContainer.items = dataModelLocal.getData("items");

                            if (indexes.length === 1){
                                let selectedIndex = indexes[0];
                                if (gqlModelBaseContainer.items.getItemsCount() > selectedIndex){
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
            queryFields.InsertField("id");
            queryFields.InsertField("name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView objectView query ", gqlData);
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.objectViewModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (gqlModelBaseContainer.objectViewModel.containsKey("errors")){
                    dataModelLocal = gqlModelBaseContainer.objectViewModel.getData("errors");

                    if (dataModelLocal.containsKey(gqlModelBaseContainer.getObjectViewGqlCommand)){
                        dataModelLocal = dataModelLocal.getData(gqlModelBaseContainer.getObjectViewGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                dataModelLocal = gqlModelBaseContainer.objectViewModel.getData("data");
                if(!dataModelLocal)
                    return;
                if (dataModelLocal.containsKey(gqlModelBaseContainer.getObjectViewGqlCommand)){
                    dataModelLocal = dataModelLocal.getData(gqlModelBaseContainer.getObjectViewGqlCommand);

                    let objectView = dataModelLocal.getData("Path");
                    let objectViewCommandsId = dataModelLocal.getData("CommandId");

                    gqlModelBaseContainer.objectViewEditorPath = objectView;
                    gqlModelBaseContainer.objectViewEditorCommandsId = objectViewCommandsId;
                }
            }
        }
    }
}
