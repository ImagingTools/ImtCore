import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import '../AuxComponents'
//import imtgui.AuxComponents 1.0

CollectionView {
    id: productCollectionView;
    width: 100;
    height: 50;
//    anchors.fill: parent;
//    color: "red";
    Component.onCompleted: {
//        productCollectionView.tabPanel.addToHeadersArray("Products")
        productInfoModel.updateModel()
        var tabPanel = productCollectionView.tabPanel
        console.log("tabPanel ", tabPanel)
    }

    GqlModel {
        id: productInfoModel;

        function updateModel() {
            console.log( "productInfoModel update");

            var query = Gql.GqlRequest("query", "ProductInfo");

            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField(CommandEnum.NAME);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ProductView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, productInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal = productInfoModel.GetData("data");
                if(dataModelLocal.ContainsKey("ProductInfo")){
                    dataModelLocal = dataModelLocal.GetData("ProductInfo")
                    if(dataModelLocal.ContainsKey("headers")){
                        tabPanel.addToHeadersArray("Products")
                        dataModelLocal = dataModelLocal.GetData("headers")
                        for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                            productCollectionView.table.addToHeadersArray(dataModelLocal.GetData(PageEnum.NAME,i));
                        }

                        productsModel.updateModel()

//                        this.table.addToHeadersArray("Description")
//                        this.table.addToHeadersArray("Added")
//                        this.table.addToHeadersArray("Last Modified")
//                        console.log("PackageView onCompleted 1")
//                        commandsModel.updateModel()
//                        console.log("PackageView onCompleted 2")
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("GetProductCollectionInfo")){
                            console.log("message", errorsModel.GetData("GetProductCollectionInfo").GetData("message"));
                        }
                    }
                }
            }
        }
    }



    GqlModel {
        id: productsModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", "ProductList");

            var queryFields = Gql.GqlObject("items");
            for (var i = 0; i < productCollectionView.table.headersArray.length; i++){
                queryFields.InsertField(productCollectionView.table.headersArray[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ProductView query ");
            console.log("ProductView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, productsModel);
            if (this.state === "Ready"){
                var dataModelLocal = productsModel.GetData("data");
                if(dataModelLocal.ContainsKey("ProductList")){
                    dataModelLocal = dataModelLocal.GetData("ProductList");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
//                        console.log("items",dataModelLocal);
                        productCollectionView.table.elementsModel = dataModelLocal;
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("ProductList")){
                            console.log("message", errorsModel.GetData("ProductList").GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
