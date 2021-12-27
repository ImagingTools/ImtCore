import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import '../AuxComponents'
//import imtgui.AuxComponents 1.0

CollectionView {
    id: installationCollectionView;
    width: 100;
    height: 50;
//    anchors.fill: parent;
//    color: "red";
    Component.onCompleted: {
//        installationCollectionView.tabPanel.addToHeadersArray("installations")
        installationInfoModel.updateModel()
        var tabPanel = installationCollectionView.tabPanel
        console.log("tabPanel ", tabPanel)
    }

    GqlModel {
        id: installationInfoModel;

        function updateModel() {
            console.log( "installationInfoModel update");

            var query = Gql.GqlRequest("query", "InstallationInfo");

            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField(CommandEnum.NAME);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, installationInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal = installationInfoModel.GetData("data");
                if(dataModelLocal.ContainsKey("InstallationInfo")){
                    dataModelLocal = dataModelLocal.GetData("InstallationInfo")
                    if(dataModelLocal.ContainsKey("headers")){
                        tabPanel.addToHeadersArray("Installations")
                        dataModelLocal = dataModelLocal.GetData("headers")
                        for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                            installationCollectionView.table.addToHeadersArray(dataModelLocal.GetData(PageEnum.NAME,i));
                        }

                        installationsModel.updateModel()

//                        this.table.addToHeadersArray("Description")
//                        this.table.addToHeadersArray("Added")
//                        this.table.addToHeadersArray("Last Modified")
//                        console.log("PackageView onCompleted 1")
//                        commandsModel.updateModel()
//                        console.log("PackageView onCompleted 2")
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("GetInstallationCollectionInfo")){
                            console.log("message", errorsModel.GetData("GetInstallationCollectionInfo").GetData("message"));
                        }
                    }
                }
            }
        }
    }



    GqlModel {
        id: installationsModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", "InstallationList");

            var queryFields = Gql.GqlObject("items");
            for (var i = 0; i < installationCollectionView.table.headersArray.length; i++){
                queryFields.InsertField(installationCollectionView.table.headersArray[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationView query ");
            console.log("InstallationView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, installationsModel);
            if (this.state === "Ready"){
                var dataModelLocal = installationsModel.GetData("data");
                if(dataModelLocal.ContainsKey("InstallationList")){
                    dataModelLocal = dataModelLocal.GetData("InstallationList");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
//                        console.log("items",dataModelLocal);
                        installationCollectionView.table.elementsModel = dataModelLocal;
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("InstallationList")){
                            console.log("message", errorsModel.GetData("InstallationList").GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
