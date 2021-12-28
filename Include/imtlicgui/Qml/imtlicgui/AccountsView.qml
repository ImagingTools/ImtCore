import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import '../AuxComponents'
//import imtgui.AuxComponents 1.0

CollectionView {
    id: accountCollectionView;
    width: 100;
    height: 50;
//    anchors.fill: parent;
//    color: "red";
    Component.onCompleted: {
//        packageCollectionView.tabPanel.addToHeadersArray("Packages")
        accountInfoModel.updateModel()
        var tabPanel = accountCollectionView.tabPanel
        console.log("tabPanel ", tabPanel)
    }

    GqlModel {
        id: accountInfoModel;

        function updateModel() {
            console.log( "accountInfoModel update");

            var query = Gql.GqlRequest("query", "AccountInfo");

            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField(CommandEnum.NAME);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, accountInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal = accountInfoModel.GetData("data");
                if(dataModelLocal.ContainsKey("AccountInfo")){
                    dataModelLocal = dataModelLocal.GetData("AccountInfo")
                    if(dataModelLocal.ContainsKey("headers")){
                        tabPanel.addToHeadersArray("Accounts")
                        dataModelLocal = dataModelLocal.GetData("headers")
                        for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                            console.log("Account header key:", dataModelLocal.GetData("Id",i))
                            accountCollectionView.table.addToHeadersArray(dataModelLocal.GetData("Id",i), dataModelLocal.GetData("Name",i));
                        }

                        accountsModel.updateModel()
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("GetAccountCollectionInfo")){
                            console.log("message", errorsModel.GetData("GetAccountCollectionInfo").GetData("message"));
                        }
                    }
                }
            }
        }
    }



    GqlModel {
        id: accountsModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", "AccountList");

            var queryFields = Gql.GqlObject("items");
            for (var i = 0; i < accountCollectionView.table.headerKeysArray.length; i++){
                queryFields.InsertField(accountCollectionView.table.headerKeysArray[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountView query ");
            console.log("AccountView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, accountsModel);
            if (this.state === "Ready"){
                var dataModelLocal = accountsModel.GetData("data");
                if(dataModelLocal.ContainsKey("AccountList")){
                    dataModelLocal = dataModelLocal.GetData("AccountList");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
//                        console.log("items",dataModelLocal);
                        accountCollectionView.table.elementsModel = dataModelLocal;
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("AccountList")){
                            console.log("message", errorsModel.GetData("AccountList").GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
