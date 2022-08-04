import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: container;

    property Item editorItem: null;
    property Item commandsProvider: null;

    property string commandsId;

    property string itemId: editorItem.itemId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    onCommandsIdChanged: {
        console.log("AccountEditor onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onItemIdChanged: {
        accountItemModel.updateModel();
    }

    onVisibleChanged: {
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
        }
    }

    function commandHandle(commandId){
        console.log("AccountEditor commandHandle", commandId);

        if (commandId === "Save"){
            if (itemId === ""){
                gqlModelQueryType = "Add";
                gqlModelQueryTypeNotify = "addedNotification";
            }
            else{
                gqlModelQueryType = "Update"
                gqlModelQueryTypeNotify = "updatedNotification";
            }

            saveQuery.updateModel();
        }
        else if (commandId === "Close"){
            multiDocView.closePage("");
        }
    }

    GqlModel {
        id: accountItemModel;

        function updateModel() {
            console.log( "updateModel AccountItem");
            var query = Gql.GqlRequest("query", "AccountItem");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", accountEditorContainer.itemId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("item");

            queryFields.InsertField("Name");
            queryFields.InsertField("Description");
            queryFields.InsertField("FirstName");
            queryFields.InsertField("LastName");
            queryFields.InsertField("NickName");
            queryFields.InsertField("Email");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, accountItemModel);
            if (this.state === "Ready"){

                 var dataModelLocal;
                if (accountItemModel.ContainsKey("errors")){
                    return;
                }

                if (accountItemModel.ContainsKey("data")){
                    dataModelLocal = accountItemModel.GetData("data");

                    if(dataModelLocal.ContainsKey("AccountItem")){
                        dataModelLocal = dataModelLocal.GetData("AccountItem");
                        if(dataModelLocal.ContainsKey("item")){
                            dataModelLocal = dataModelLocal.GetData("item");

                            let keys = dataModelLocal.GetKeys();
                            console.log("keys", keys);

                            for (let i = 0; i < keys.length; i++){
                                console.log("keys[i]", keys[i], "->", dataModelLocal.GetData(keys[i]));
                                accountModel.SetData(keys[i], dataModelLocal.GetData(keys[i]));
                            }

                            undoRedoManager.model = accountModel;

                            updateGui();
                        }
                    }
                }
            }
        }
    }//GqlModel accountItemModel


    GqlModel {
        id: saveQuery;

        function updateModel() {
            console.log( "updateModel saveQuery");

            var query = Gql.GqlRequest("query", "Account" + gqlModelQueryType);


            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.itemId);

            var queryFields = Gql.GqlObject(gqlModelQueryTypeNotify);
            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");
            query.AddField(queryFields);

            var jsonString = accountModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            inputParams.InsertField ("Item", jsonString);

            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            console.log("AccountEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveQuery);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (saveQuery.ContainsKey("errors")){
                    dataModelLocal = saveQuery.GetData("errors");

                    return;
                }

                if (saveQuery.ContainsKey("data")){
                    dataModelLocal = saveQuery.GetData("data");

                    if (dataModelLocal.ContainsKey("Account" + gqlModelQueryType)){
                        dataModelLocal = dataModelLocal.GetData("Account" + gqlModelQueryType);

                        if (dataModelLocal.ContainsKey(gqlModelQueryTypeNotify)){
                            dataModelLocal = dataModelLocal.GetData(gqlModelQueryTypeNotify);
                            let itemId = dataModelLocal.GetData("Id");
                            let itemName = dataModelLocal.GetData("Name");

                            multiDocView.updatePageTitle({"ItemId": "", "Title": itemName});
                        }
                    }

                    Events.sendEvent(commandsId + "CollectionUpdateGui");
                }
            }
        }
    }
}

