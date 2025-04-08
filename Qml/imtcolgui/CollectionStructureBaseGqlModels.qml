import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: gqlModelBaseContainer;

    property string getItemsGqlCommand;
    property string getObjectViewGqlCommand;
    property string objectViewEditorPath;
    property string objectViewEditorCommandsId;
    property string commandId;

    property var additionInputParams: ({})

    // property TreeItemModel items: TreeItemModel {};
    property TreeItemModel notificationModel: null;

    property Item rootItem: null;

    // Поля которые запросятся для модели коллекции
    property var fieldsData: ["id", "name"]

    signal itemsInfoGqlStateChanged(string state);
    signal itemsReceived(var selectIndex, var items)

    onCommandIdChanged: {
        if (commandId !== ""){
            getItemsGqlCommand = commandId + "GetElements";
            getObjectViewGqlCommand = commandId + "ObjectView";
        }
    }

    function updateModels(){
        console.log("CollectionStructure updateModels");
        if (commandId === ""){
            console.error("Update model was failed! Command-ID is invalid.");
            return;
        }

        gqlModelBaseContainer.objectViewModel.getObjectView();
        gqlModelBaseContainer.itemsInfoModel.updateModel("");
    }

    function updateItemsModel(selectIndex){
        if (commandId === ""){
            console.error("Update model was failed! Command-ID is invalid.");
            return;
        }

        gqlModelBaseContainer.itemsInfoModel.updateModel(selectIndex);
    }


    property GqlModel itemsInfoModel: GqlModel {
        function updateModel(selectIndex) {
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.getItemsGqlCommand);
            let count = -1;
            let offset = 0;

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");

            let filterModel = gqlModelBaseContainer.rootItem.modelFilter;
            var jsonString = filterModel.toJson();
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);
            // let parentNodeId = treeViewInternal.getData("id", selectIndex)
            // if (parentNodeId === undefined){
            //     parentNodeId = ""
            // }
            inputParams.InsertField("ParentNodeId", selectIndex);
            let additionParams = Gql.GqlObject("addition");
            if (Object.keys(gqlModelBaseContainer.additionInputParams).length > 0){
                for (let key in gqlModelBaseContainer.additionInputParams){
                    additionParams.InsertField(key, gqlModelBaseContainer.additionInputParams[key]);
                }
            }
            additionParams.InsertField("selectIndex", selectIndex)
            inputParams.InsertFieldObject(additionParams);
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
                let dataModelLocal;
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
                            let items = dataModelLocal.getData("items");
                            let selectIndex = dataModelLocal.getData("selectIndex");
                            gqlModelBaseContainer.itemsReceived(selectIndex, items)

                            // Events.sendEvent(gqlModelBaseContainer.commandId + "StructureUpdated");
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
            queryFields.InsertField("id");
            queryFields.InsertField("name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionStructure objectView query ", gqlData);
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
