import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: collectionViewContainer;

    height: 100;
    width: 100;

    color: "transparent";

    property TreeItemModel collectionViewModel;

    property var table: tableInternal;

    property int selectedIndex: -1;

    property string gqlModelInfo;
    property string gqlModelItems;
    property string gqlModelItem;
    property string gqlModelRemove;
    property string gqlModelRename;
    property string gqlModelSetDescription;

    property string itemId;
    property string itemName;

    property bool autoRefresh: false;
    property bool hasPagination: true;

    signal itemSelect(string idSelect, string name);
    signal headerClicked(string headerId, string sortOrder);
    signal removedItem(string itemId);
    signal renamedItem(string oldId, string newId);
    signal setDescriptionItem(string newDescription);
    signal collectionViewRightButtonMouseClicked(Item item, int mouseX, int mouseY);
    signal setActiveFocusFromCollectionView();

    Component.onCompleted: {
        console.log("CollectionView onCompleted");
    }

    onCollectionViewModelChanged: {
        console.log("CollectionView onModelChanged", collectionViewContainer.itemId, collectionViewContainer.itemName);
        collectionViewContainer.refresh();
    }

    onGqlModelInfoChanged: {
        console.log("CollectionView onGqlModelInfoChanged");
        headerInfoModel.updateModel();
    }

    onGqlModelItemsChanged: {
        console.log("CollectionView onGqlModelItemsChanged");
        modelItems.updateModel();
    }

    function openMessageDialog(nameDialog, message, type) {
        console.log("CollectionView openMessageDialog", nameDialog, message, type);
        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = collectionViewContainer;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "OK";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;
        parameters["dialogId"] = type;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function getDescriptionBySelectedItem(){
        console.log("CollectionView getDescriptionBySelectedItem");
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        var description = "";
//        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){

//            console.log("collectionViewContainer.gqlModelInfo", collectionViewContainer.gqlModelInfo);
//            console.log("dataModelLocal", dataModelLocal.toJSON());
//            console.log("tableInternal.selectedIndex", tableInternal.selectedIndex);

//            description = dataModelLocal.GetData("AccountDescription", tableInternal.selectedIndex);
//        }
//        else{
//            description = dataModelLocal.GetData("Description", tableInternal.selectedIndex);
//        }

        description = dataModelLocal.GetData("Description", tableInternal.selectedIndex);

        console.log("description", description);
        return description;
    }

    function getNameBySelectedItem(){
        console.log("CollectionView getNameBySelectedItem");
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        var name;
        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            name = dataModelLocal.GetData("AccountName", tableInternal.selectedIndex);
        }
        else{
            name = dataModelLocal.GetData("Name", tableInternal.selectedIndex);
        }
        return name;
    }

    function getCountItems(){
        console.log("CollectionView getCountItems");
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        if (!dataModelLocal){
            return -1;
        }

        return dataModelLocal.GetItemsCount();
    }

    function callSetDescriptionQuery(value){
        console.log("CollectionView callSetDescriptionQuery", value);
        setDescriptionQuery.setDescription(value);
    }

    function callRenameQuery(value){
        console.log("CollectionView callRenameQuery", value);
        renameQuery.rename(value);
    }

    function updateItemAfterRename(newId, newName) {
        console.log("CollectionView updateItemAfterRename", newId, newName);
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        dataModelLocal.SetData("Id", newId, tableInternal.selectedIndex);

        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            dataModelLocal.SetData("AccountName", newName, tableInternal.selectedIndex);
        }
        else{
            dataModelLocal.SetData("Name", newName, tableInternal.selectedIndex);
        }

        collectionViewContainer.collectionViewModel.SetData("data", dataModelLocal);
    }

    function updateItemAfterSetDescription(description) {
        console.log("CollectionView updateItemAfterSetDescription", description);
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            dataModelLocal.SetData("AccountDescription", description, tableInternal.selectedIndex);
        }
        else{
            dataModelLocal.SetData("Description", description, tableInternal.selectedIndex);
        }

        collectionViewContainer.collectionViewModel.SetData("data", dataModelLocal);
    }

    function dialogResult(parameters) {
        console.log("CollectionView dialogResult", parameters["status"]);

        if (parameters["status"] === "yes") {

            if (collectionViewContainer.gqlModelRemove !== "") {
                collectionViewContainer.removeSelectedItem();
            }
            collectionViewContainer.refresh();
            tableInternal.selectedIndex = -1;
        }
    }

    function refresh(){
        var isHeaderUpdated = false;
        console.log("CollectionView refresh", collectionViewContainer.gqlModelInfo)
        if (collectionViewContainer.collectionViewModel && collectionViewContainer.collectionViewModel.ContainsKey("Headers")){
            var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("Headers");
            tableInternal.headers = dataModelLocal;
        }
        else{
            headerInfoModel.updateModel()
            isHeaderUpdated = true
        }

        if (collectionViewContainer.collectionViewModel && collectionViewContainer.collectionViewModel.ContainsKey("data")){
            let dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");
            tableInternal.elements = 0;
            tableInternal.elements = dataModelLocal;

            var selectedIndexLocal = collectionViewContainer.collectionViewModel.GetData("selectedIndex");
            console.log("selectedIndexLocal", selectedIndexLocal);
            if (selectedIndexLocal != undefined) {
                tableInternal.selectedIndex = -1;
                tableInternal.selectedIndex = selectedIndexLocal;
                console.log("tableInternal.selectedIndex", tableInternal.selectedIndex);
            }

            if (collectionViewContainer.autoRefresh){
                modelItems.updateModel();
            }
        }
        else if(!isHeaderUpdated){
            modelItems.updateModel();
        }
    }

    function menuActivated(menuId){
        console.log("CollectionView menuActivated", menuId);

        var itemId = tableInternal.getSelectedId();
        var name = tableInternal.getSelectedName();

        if (menuId  === "New"){
            collectionViewContainer.itemSelect("", "")
        }
        else if (menuId  === "Edit"){
            if (tableInternal.selectedIndex >= 0){
                let curName = tableInternal.elements.GetData("Name", tableInternal.selectedIndex);
                collectionViewContainer.itemSelect(itemId, curName);
            }
        }
        else if (menuId  === "Remove"){
            if (itemId !== ""){
                var source = "AuxComponents/MessageDialog.qml";
                var parameters = {};
                parameters["message"] = "Remove selected file from the collection ?";
                parameters["nameDialog"] = "Remove dialog";
                parameters["dialogId"] = "RemoveDialog";
                parameters["resultItem"] = collectionViewContainer;

                thubnailDecoratorContainer.openDialog(source, parameters);
            }
        }
    }

    function removeSelectedItem(){
        console.log("CollectionView removeSelectedItem", collectionViewContainer.itemId);
        collectionViewContainer.itemId = tableInternal.getSelectedId();
        removeModel.updateModel();
    }

    Rectangle {
        id: backgroundTable;

        anchors.fill: parent;
        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
        anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;
        anchors.rightMargin: thubnailDecoratorContainer.mainMargin;

        color: Style.baseColor;

        radius: thubnailDecoratorContainer.mainRadius;

        AuxTable {
            id: tableInternal;

            anchors.fill: parent;
            anchors.topMargin: thubnailDecoratorContainer.mainMargin;
            anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
            anchors.bottom: pagination.visible ? pagination.top : parent.bottom;

            onSelectItem: {
                console.log("CollectionView AuxTable onSelectItem", idSelected, name);
                collectionViewContainer.itemSelect(idSelected, name);
            }

            onRightButtonMouseClicked: {
                console.log("CollectionView AuxTable onRightButtonMouseClicked");
                collectionViewContainer.collectionViewRightButtonMouseClicked(item, mouseX, mouseY);
            }

            onSelectedIndexChanged: {
                console.log(" CollectionView AuxTable onSelectedIndexChanged", collectionViewContainer.selectedIndex, tableInternal.selectedIndex);
                collectionViewContainer.collectionViewModel.SetData("selectedIndex", tableInternal.selectedIndex);
                collectionViewContainer.selectedIndex = tableInternal.selectedIndex;

                //tableInternal.elementsList.selectedId = elements.GetData("Id", tableInternal.selectedIndex);
            }

            onSetActiveFocusFromTable: {
                console.log("CollectionView AuxTable onSetActiveFocusFromTable");
                collectionViewContainer.setActiveFocusFromCollectionView();
            }

            onTextFilterChanged: {
                modelFilter.SetData("TextFilter", text);
                modelItems.updateModel();
            }

    //        onFilterChanged: {
    //            modelFilter.SetData("TextFilter", text);
    //            modelItems.updateModel();
    ////            if (modelFilter.ContainsKey("filterLocal")){

    ////                let filterLocal = modelFilter.GetData("filterLocal");
    ////                filterLocal.SetData("Text", text, index);
    ////                filterLocal.SetData("Id", id, index);
    //////                for (let i = 0; i < tableInternal.headers.GetItemsCount(); i++){
    //////                    let index = filterLocal.InsertNewItem()
    //////                    filterLocal.SetData("Id", tableInternal.headers.GetData("Id", i), index);
    //////                    filterLocal.SetData("Text", "", index);
    //////                }
    ////                modelItems.updateModel();
    ////            }
    //        }

            onHeaderOnClicked: {
                console.log("CollectionView AuxTable onHeaderOnClicked", headerId, sortOrder);
                collectionViewContainer.headerClicked(headerId, sortOrder);

//                var filterLocal = modelFilter.GetData("Sort");
//                filterLocal.SetData("HeaderId", headerId);
//                filterLocal.SetData("SortOrder", sortOrder);

                modelItems.updateModel();
            }
        }
    }

    function setHeaderSort(headerId, sortOrder){
        var filterLocal = modelFilter.GetData("Sort");
        filterLocal.SetData("HeaderId", headerId);
        filterLocal.SetData("SortOrder", sortOrder);
    }

    TreeItemModel {
        id: modelFilter;

        Component.onCompleted: {
            modelFilter.AddTreeModel("FilterIds");
            modelFilter.AddTreeModel("Sort");
        }
    }

    Pagination {
        id: pagination;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        pagesSize: 1;

        visible: collectionViewContainer.hasPagination;

        onCurrentValueChanged: {
            console.log("Pagination onCurrentValueChanged", pagination.currentValue);
            tableInternal.selectedIndex = -1;
            modelItems.updateModel();
        }
    }

    GqlModel {
        id: headerInfoModel;

        function updateModel() {
            console.log( "headerInfoModel update", collectionViewContainer.gqlModelInfo);


            var query = Gql.GqlRequest("query", collectionViewContainer.gqlModelInfo);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("LanguageId");
            inputParams.InsertFieldArgument("LanguageId", Style.language);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, headerInfoModel);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (headerInfoModel.ContainsKey("errors")){

                    dataModelLocal = headerInfoModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo);

                        if (dataModelLocal.ContainsKey("message")){

                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("Error dialog", message, "ErrorDialog");
                        }

                    }

                    return;
                }

                if (headerInfoModel.ContainsKey("data")){
                    dataModelLocal = headerInfoModel.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo);

                        if(dataModelLocal.ContainsKey("headers")){
                            dataModelLocal = dataModelLocal.GetData("headers");

                            if (dataModelLocal.ContainsKey("Headers")){
                                tableInternal.headers = dataModelLocal.GetData("Headers")
                                console.log("collectionViewContainer.collectionViewModel");
                                if (collectionViewContainer.collectionViewModel){
                                    collectionViewContainer.collectionViewModel.SetExternTreeModel('Headers', tableInternal.headers)
                                }
                            }

                            if (dataModelLocal.ContainsKey("FilterSearch")){
                                let filterSearchModel = dataModelLocal.GetData("FilterSearch")
                                modelFilter.SetData("FilterIds", filterSearchModel);
                            }

                            if (dataModelLocal.ContainsKey("LanguageId")){
                                let langId = dataModelLocal.GetData("LanguageId");
                                if (Style.language != langId){
                                    Style.language = langId;
                                }
                            }

                            modelItems.updateModel();
                        }
                    }
                }
            }
        }
    }

    Timer {
        id: elementsTimer;
        property var elementsmodel;
        interval: 10;
        onTriggered: {
            tableInternal.elements = elementsTimer.elementsmodel;
        }
    }

    GqlModel {
        id: modelItems;

        function updateModel() {
            console.log( "collectionViewContainer updateModel", collectionViewContainer.gqlModelItems, collectionViewContainer.itemId);
            let height = collectionViewContainer.height - pagination.height - tableInternal.itemHeight; //Убрать высоту от заголовка и меню пагинации
            let count = Math.floor(height / tableInternal.itemHeight);
            let offset = (pagination.currentValue - 1) * count;
            var query = Gql.GqlRequest("query", collectionViewContainer.gqlModelItems);
            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset");
            viewParams.InsertFieldArgument("Offset", offset);
            viewParams.InsertField("Count");
            viewParams.InsertFieldArgument("Count", count);
            viewParams.InsertField("FilterModel");
            var jsonString = modelFilter.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            viewParams.InsertFieldArgument("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);

            if(collectionViewContainer.itemId != ""){
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", collectionViewContainer.itemId);
            }
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            for(var i = 0; i < tableInternal.headers.GetItemsCount(); i++){
                queryFields.InsertField(tableInternal.headers.GetData("Id", i));
            }
            query.AddField(queryFields);

            var notifyFields = Gql.GqlObject("Notification");
            notifyFields.InsertField("PagesCount");
            query.AddField(notifyFields);

            var gqlData = query.GetQuery();
            console.log("collectionViewContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, modelItems);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (modelItems.ContainsKey("errors")){
                    dataModelLocal = modelItems.GetData("errors");
                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);
                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("Error dialog", message, "ErrorDialog");
                        }
                    }

                    return;
                }

                if (modelItems.ContainsKey("data")){
                    dataModelLocal = modelItems.GetData("data");
                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);
                        if (dataModelLocal.ContainsKey("items")){
                            tableInternal.elements = dataModelLocal.GetData("items");
                            collectionViewContainer.collectionViewModel.SetExternTreeModel('data', tableInternal.elements);
                            //collectionViewContainer.collectionViewModel = dataModelLocal.GetData("items");
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");

                            if (dataModelLocal.ContainsKey("PagesCount")){
                                var pagesCount = dataModelLocal.GetData("PagesCount");
                                if (pagesCount != pagination.pagesSize){
                                    pagination.pagesSize = pagesCount;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: removeModel;

        function updateModel() {
            console.log( "updateModel removeModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(collectionViewContainer.itemId != ""){
                query = Gql.GqlRequest("query", collectionViewContainer.gqlModelRemove);
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", tableInternal.getSelectedId());
                queryFields = Gql.GqlObject("removedNotification");
                query.AddParam(inputParams);

                queryFields.InsertField("Id");
                queryFields.InsertField("Successed");

                query.AddField(queryFields);

                var gqlData = query.GetQuery();
                console.log("removeModel query ", gqlData);
                this.SetGqlQuery(gqlData);
            }
        }

        onStateChanged: {
            console.log("State:", this.state, removeModel);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (removeModel.ContainsKey("errors")){
                    dataModelLocal = removeModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelRemove)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelRemove);
                    }

                    return;
                }

                if (removeModel.ContainsKey("data")){
                    dataModelLocal = removeModel.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelRemove)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelRemove);

                        if (dataModelLocal.ContainsKey("removedNotification")){
                            dataModelLocal = dataModelLocal.GetData("removedNotification");

                            if (dataModelLocal.ContainsKey("Id")){
                                var itemId = dataModelLocal.GetData("Id");
                                collectionViewContainer.removedItem(itemId)
                            }
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: renameQuery;

        function rename(newName) {
            console.log( "CollectionView renameQuery rename");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", collectionViewContainer.gqlModelRename);
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", tableInternal.getSelectedId());

            inputParams.InsertField("NewName");
            inputParams.InsertFieldArgument("NewName", newName);

            query.AddParam(inputParams);

            queryFields = Gql.GqlObject("rename");
            queryFields.InsertField("NewName");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView renameQuery rename", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, renameQuery);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (renameQuery.ContainsKey("errors")){
                    dataModelLocal = renameQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelRename)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelRename);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("Error dialog", message, "ErrorDialog");
                        }
                    }

                    return;
                }

                if (renameQuery.ContainsKey("data")){
                    dataModelLocal = renameQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelRename)) {
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelRename);

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var newId = dataModelLocal.GetData("NewId");

//                            if (collectionViewContainer.gqlModelInfo === "ProductCollectionInfo"){
//                                newId = tableInternal.getSelectedId();
//                            }

                            var newName = dataModelLocal.GetData("NewName");
                            collectionViewContainer.renamedItem(tableInternal.getSelectedId(), newId);
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: setDescriptionQuery;

        function setDescription(description){
            console.log( "CollectionView setDescriptionQuery");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", collectionViewContainer.gqlModelSetDescription);
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", tableInternal.getSelectedId());

            inputParams.InsertField("Description");
            inputParams.InsertFieldArgument("Description", description);

            query.AddParam(inputParams);

            queryFields = Gql.GqlObject("setDescription");
            queryFields.InsertField("Description");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView setDescription query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, setDescriptionQuery);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (setDescriptionQuery.ContainsKey("errors")){
                    dataModelLocal = setDescriptionQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelSetDescription)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("Error dialog", message, "ErrorDialog");
                        }
                    }

                    return;
                }

                if (setDescriptionQuery.ContainsKey("data")){
                    dataModelLocal = setDescriptionQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelSetDescription)) {

                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var description = dataModelLocal.GetData("Description");

                            collectionViewContainer.updateItemAfterSetDescription(description);
                            collectionViewContainer.setDescriptionItem(description);
                        }
                    }
                }
            }
        }
    }
}
