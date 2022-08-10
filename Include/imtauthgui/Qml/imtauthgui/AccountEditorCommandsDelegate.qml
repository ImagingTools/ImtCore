import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    onItemIdChanged: {
        accountItemModel.updateModel();
    }

    GqlModel {
        id: accountItemModel;

        function updateModel() {
            console.log( "updateModel AccountItem");
            var query = Gql.GqlRequest("query", "AccountItem");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);
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
                            updateGui();


                            undoRedoManager.model = accountModel;
                        }
                    }
                }
            }
        }
    }//GqlModel accountItemModel
}

