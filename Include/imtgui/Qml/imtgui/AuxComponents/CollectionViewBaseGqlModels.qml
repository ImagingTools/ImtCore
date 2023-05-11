import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: gqlModelBaseContainer;

    property string gqlModelHeadersInfo: commandsId + "Info";
    property string gqlModelItemsInfo: commandsId + "List";
    property string gqlModelObjectView: commandsId + "ObjectView";

    property string objectViewEditorPath;
    property string objectViewEditorCommandsId;

    property string itemId;
    property var table;

    property string commandsId;

    property TreeItemModel headers: TreeItemModel {};
    property TreeItemModel items: TreeItemModel {};

    property TreeItemModel notificationModel: null;

    signal itemsInfoGqlStateChanged(string state);

    onNotificationModelChanged: {
        if (gqlModelBaseContainer.notificationModel != null){
            if (gqlModelBaseContainer.notificationModel.ContainsKey("PagesCount")){
                gqlModelBaseContainer.rootItem.pagination.pagesSize = gqlModelBaseContainer.notificationModel.GetData("PagesCount");
            }
        }
    }

    property Item rootItem: null;
    property Item pagination: null;

    property Item loading: null;

    // Поля которые запросятся для модели коллекции
    property var fieldsData: ["Id", "Name"]

    function updateModels(){
        console.log("CollectionView updateModels");
        gqlModelBaseContainer.headerInfoModel.updateModel();

        gqlModelBaseContainer.objectViewModel.getObjectView();
    }

    function updateItemsModel(){
        gqlModelBaseContainer.itemsInfoModel.updateModel();
    }

    property GqlModel headerInfoModel: GqlModel {
        function updateModel() {
            console.log( "headerInfoModel update", gqlModelBaseContainer.gqlModelHeadersInfo);

            var query = Gql.GqlRequest("query", gqlModelBaseContainer.gqlModelHeadersInfo);

            var queryHeaders = Gql.GqlObject("headers");
            queryHeaders.InsertField("Id");
            queryHeaders.InsertField("Name");
            query.AddField(queryHeaders);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.headerInfoModel);
            if (this.state === "Ready"){
                console.log("onStateChanged Ready", gqlModelBaseContainer.gqlModelHeadersInfo);

                var dataModelLocal;

                if (gqlModelBaseContainer.headerInfoModel.ContainsKey("errors")){

                    return;
                }

                if (gqlModelBaseContainer.headerInfoModel.ContainsKey("data")){
                    dataModelLocal = gqlModelBaseContainer.headerInfoModel.GetData("data");

                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.gqlModelHeadersInfo)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.gqlModelHeadersInfo);

                        if (dataModelLocal.ContainsKey("FilterSearch")){
                            let filterSearchModel = dataModelLocal.GetData("FilterSearch")
                            gqlModelBaseContainer.rootItem.modelFilter.SetData("FilterIds", filterSearchModel);
                        }

                        if(dataModelLocal.ContainsKey("Headers")){
                            dataModelLocal = dataModelLocal.GetData("Headers");
                            gqlModelBaseContainer.headers = dataModelLocal;

                            for(var i = 0; i < gqlModelBaseContainer.headers.GetItemsCount(); i++){
                                let headerId = gqlModelBaseContainer.headers.GetData("Id", i);
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
        function updateModel() {
            console.log( "gqlModelBaseContainer updateModel", gqlModelBaseContainer.gqlModelItemsInfo, gqlModelBaseContainer.itemId);
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.gqlModelItemsInfo);
            let count = gqlModelBaseContainer.pagination.countElements;
            let offset = (gqlModelBaseContainer.pagination.currentIndex) * count;

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");

            let filterModel = gqlModelBaseContainer.rootItem.modelFilter;
            var jsonString = filterModel.toJSON();
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            for(var i = 0; i < gqlModelBaseContainer.fieldsData.length; i++){
                queryFields.InsertField(gqlModelBaseContainer.fieldsData[i]);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            console.log("gqlData", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.itemsInfoModel);
            gqlModelBaseContainer.itemsInfoGqlStateChanged(this.state);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (gqlModelBaseContainer.itemsInfoModel.ContainsKey("errors")){
                    return;
                }

                if (gqlModelBaseContainer.itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = gqlModelBaseContainer.itemsInfoModel.GetData("data");
                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.gqlModelItemsInfo)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.gqlModelItemsInfo);
                        if (dataModelLocal.ContainsKey("notification")){
                            gqlModelBaseContainer.notificationModel = dataModelLocal.GetData("notification");
                        }

                        if (!dataModelLocal.ContainsKey("items")){
                            dataModelLocal.AddTreeModel("items")
                        }

                        if (dataModelLocal.ContainsKey("items")){
                            if (!gqlModelBaseContainer.table){
                                return;
                            }

                            let indexes = gqlModelBaseContainer.table.getSelectedIndexes();
                            gqlModelBaseContainer.table.resetSelection();
                            gqlModelBaseContainer.items.Clear();
                            gqlModelBaseContainer.items = dataModelLocal.GetData("items");

                            if (indexes.length === 1){
                                let selectedIndex = indexes[0];
                                if (gqlModelBaseContainer.items.GetItemsCount() > selectedIndex){
                                    gqlModelBaseContainer.table.select(selectedIndex);
                                }
                            }

                            Events.sendEvent(gqlModelBaseContainer.commandsId + "CollectionUpdated");
                        }
                    }
                }
            }
        }
    }

    property GqlModel objectViewModel: GqlModel {

        function getObjectView(){
            console.log( "CollectionView objectView");
            var query = Gql.GqlRequest("query", gqlModelBaseContainer.gqlModelObjectView);

            var queryFields = Gql.GqlObject("objectView");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView objectView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, gqlModelBaseContainer.objectViewModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (gqlModelBaseContainer.objectViewModel.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = gqlModelBaseContainer.objectViewModel.GetData("data");

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
