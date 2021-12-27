import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import '../AuxComponents'
//import imtgui.AuxComponents 1.0

CollectionView {
    id: packageCollectionView;
    width: 100;
    height: 50;
//    anchors.fill: parent;
//    color: "red";
    Component.onCompleted: {
//        packageCollectionView.tabPanel.addToHeadersArray("Packages")
        packageInfoModel.updateModel()
        var tabPanel = packageCollectionView.tabPanel
        console.log("tabPanel ", tabPanel)
    }

    GqlModel {
        id: packageInfoModel;

        function updateModel() {
            console.log( "packageInfoModel update");

            var query = Gql.GqlRequest("query", "FeaturePackageInfo");

            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField(CommandEnum.NAME);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, packageInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal = packageInfoModel.GetData("data");
                if(dataModelLocal.ContainsKey("FeaturePackageInfo")){
                    dataModelLocal = dataModelLocal.GetData("FeaturePackageInfo")
                    if(dataModelLocal.ContainsKey("headers")){
                        tabPanel.addToHeadersArray("Packages")
                        dataModelLocal = dataModelLocal.GetData("headers")
                        for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                            packageCollectionView.table.addToHeadersArray(dataModelLocal.GetData(PageEnum.NAME,i));
                        }

                        packagesModel.updateModel()

//                        this.table.addToHeadersArray("Description")
//                        this.table.addToHeadersArray("Added")
//                        this.table.addToHeadersArray("Last Modified")
//                        console.log("PackageView onCompleted 1")
//                        commandsModel.updateModel()
//                        console.log("PackageView onCompleted 2")
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("GetPackageCollectionInfo")){
                            console.log("message", errorsModel.GetData("GetPackageCollectionInfo").GetData("message"));
                        }
                    }
                }
            }
        }
    }



    GqlModel {
        id: packagesModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", "FeaturePackageList");

            var queryFields = Gql.GqlObject("items");
            for (var i = 0; i < packageCollectionView.table.headersArray.length; i++){
                queryFields.InsertField(packageCollectionView.table.headersArray[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageView query ");
            console.log("PackageView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, packagesModel);
            if (this.state === "Ready"){
                var dataModelLocal = packagesModel.GetData("data");
                if(dataModelLocal.ContainsKey("FeaturePackageList")){
                    dataModelLocal = dataModelLocal.GetData("FeaturePackageList");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
//                        console.log("items",dataModelLocal);
                        packageCollectionView.table.elementsModel = dataModelLocal;
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("FeaturePackageList")){
                            console.log("message", errorsModel.GetData("FeaturePackageList").GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
