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

    onCommandsIdChanged: {
        console.log("PackageCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onItemIdChanged: {
        accountItemModel.updateModel();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
    }

    function commandHandle(commandId){
        console.log("AccountsCommands commandActivated", commandId);

        if (commandId === "Save"){
            saveQuery.updateModel();
        }
        else if (commandId === "Close"){
            multiDocView.closeTab(tabPanelInternal.selectedIndex);
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
                                accountModel.SetData(keys[i], dataModelLocal.GetData(keys[i]));
                            }

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

            var query = Gql.GqlRequest("query", "AccountUpdate");


            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.itemId);

            var queryFields = Gql.GqlObject("updatedNotification");
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

                }
            }
        }
    }
}

