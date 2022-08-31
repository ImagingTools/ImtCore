import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: gqlModelBaseContainer;

//    property string itemId;

    property string gqlModelHeadersInfo: commandsId + "Info";
    property string gqlModelItemsInfo: commandsId + "List";
    property string gqlModelObjectView: commandsId + "ObjectView";

    property string objectViewEditorPath;
    property string objectViewEditorCommandsId;

    property TreeItemModel headers;
    property TreeItemModel items;

    function updateModels(){
        headerInfoModel.updateModel();

        objectViewModel.getObjectView();
    }

    function updateItemsModel(){
        itemsInfoModel.updateModel();
    }

    GqlModel {
        id: headerInfoModel;

        function updateModel() {
            console.log( "headerInfoModel update", gqlModelBaseContainer.gqlModelHeadersInfo);

            var query = Gql.GqlRequest("query", gqlModelBaseContainer.gqlModelHeadersInfo);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("LanguageId", Style.language);
            query.AddParam(inputParams);

            var queryHeaders = Gql.GqlObject("headers");
            queryHeaders.InsertField("Id");
            queryHeaders.InsertField("Name");
            query.AddField(queryHeaders);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, headerInfoModel);
            if (this.state === "Ready"){
                console.log("onStateChanged Ready", gqlModelBaseContainer.gqlModelHeadersInfo);

                var dataModelLocal;

                if (headerInfoModel.ContainsKey("errors")){

                    return;
                }

                if (headerInfoModel.ContainsKey("data")){
                    dataModelLocal = headerInfoModel.GetData("data");

                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.gqlModelHeadersInfo)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.gqlModelHeadersInfo);

                        if (dataModelLocal.ContainsKey("FilterSearch")){
                            let filterSearchModel = dataModelLocal.GetData("FilterSearch")
                            modelFilter.SetData("FilterIds", filterSearchModel);
                        }

                        if(dataModelLocal.ContainsKey("Headers")){
                            dataModelLocal = dataModelLocal.GetData("Headers");
                            gqlModelBaseContainer.headers = dataModelLocal;

                            itemsInfoModel.updateModel();
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: itemsInfoModel;

        function updateModel() {
            console.log( "gqlModelBaseContainer updateModel", gqlModelBaseContainer.gqlModelItemsInfo, gqlModelBaseContainer.itemId);
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.gqlModelItemsInfo);

            let height = collectionViewBaseContainer.height - pagination.height - table.itemHeight; //Убрать высоту от заголовка и меню пагинации
            let count = Math.floor(height / table.itemHeight);
            let offset = (pagination.currentValue - 1) * count;

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");
            var jsonString = modelFilter.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);

            if (itemId){
                inputParams.InsertField("Id", itemId);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            for(var i = 0; i < gqlModelBaseContainer.headers.GetItemsCount(); i++){
                queryFields.InsertField(gqlModelBaseContainer.headers.GetData("Id", i));
            }

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("gqlModelBaseContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (itemsInfoModel.ContainsKey("errors")){
                    return;
                }

                if (itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = itemsInfoModel.GetData("data");
                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.gqlModelItemsInfo)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.gqlModelItemsInfo);
                        if (dataModelLocal.ContainsKey("items")){
                            gqlModelBaseContainer.items = dataModelLocal.GetData("items");
                            table.selectedIndex = -1;
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");
                            if (dataModelLocal.ContainsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.GetData("PagesCount");

                                pagination.pagesSize = dataModelLocal;
                            }
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: objectViewModel;

        function getObjectView(){
            console.log( "CollectionView objectView");
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.gqlModelObjectView);
            var inputParams = Gql.GqlObject("input");
            var queryFields = Gql.GqlObject("objectView");
            query.AddParam(inputParams);


            queryFields.InsertField("Id");
            queryFields.InsertField("Name");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView objectView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, objectViewModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (objectViewModel.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = objectViewModel.GetData("data");

                if (dataModelLocal.ContainsKey(gqlModelBaseContainer.gqlModelObjectView)){
                    dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.gqlModelObjectView);

                    let objectView = dataModelLocal.GetData("Path");
                    let objectViewCommandsId = dataModelLocal.GetData("CommandsId");

                    gqlModelBaseContainer.objectViewEditorPath = objectView;
                    gqlModelBaseContainer.objectViewEditorCommandsId = objectViewCommandsId;
                }
            }
        }
    }
}
