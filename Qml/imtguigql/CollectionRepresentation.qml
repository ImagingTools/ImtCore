import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: root;

    property string collectionId: "";

    property TreeItemModel elementsModel: TreeItemModel {}
    property TreeItemModel headersModel: TreeItemModel {}
    property TreeItemModel objectEditorInfoModel: TreeItemModel {}

    property TreeItemModel notificationModel: TreeItemModel {}

    property TreeItemModel filterableHeadersModel: TreeItemModel {}

    property var additionalFieldIds: []

    signal removed(string objectId);
    signal renamed(string objectId, string newName);
    signal descriptionSetted(string objectId, string description);

    property alias removeGqlModel: removeModel;
    property alias objectViewGqlModel: objectViewModel;
    property alias headersGqlModel: headersModel;

    signal beginUpdate();
    signal endUpdate();

    onCollectionIdChanged: {
        console.log("onCollectionIdChanged", collectionId)
        updateModel();
    }

    onEndUpdate: {
        internal.elementsUpdatingBlock = false;
    }

    QtObject {
        id: internal;

        property string removeGqlCommand: root.collectionId + "Remove";
        property string renameGqlCommand: root.collectionId + "Rename";
        property string setDescriptionGqlCommand: root.collectionId + "SetDescription";

        property bool elementsUpdatingBlock: false;
        property bool headersUpdatingBlock: false;
    }

    function updateModel(){
        console.log("updateModel")

        updateHeaders();

        updateObjectEditorInfo();
    }

    function updateObjectEditorInfo(){
        if (root.collectionId === ""){
            console.error("Unable to update object view 'collectionId' is empty!");

            return;
        }

        objectViewModel.getObjectView();
    }

    function updateHeaders(){
        if (internal.headersUpdatingBlock){
            return;
        }

        if (root.collectionId === ""){
            console.error("Unable to update headers 'collectionId' is empty!");

            return;
        }

        headersModel.getHeaders();
    }

    function updateElements(count, offset, filterModel){
        if (internal.elementsUpdatingBlock){
            return;
        }

        if (root.collectionId === ""){
            console.error("Unable to update elements 'collectionId' is empty!");

            return;
        }

        elementsGqlModel.getElements(count, offset, filterModel)
    }

    function getElementsRepresentation(){
        return root.elementsModel;
    }

    function getHeadersRepresentation(){
        return root.headersModel;
    }

    function insertNewObject(typeId){
    }

    function getObjectData(objectId, callback){
    }

    function setObjectData(objectId, objectData, callback){
    }

    function removeElement(elementIndex){
        if (elementIndex < 0 || elementIndex >= root.elementsModel.GetItemsCount()){
            console.error();

            return;
        }

        let elementId = root.elementsModel.GetData("Id", elementIndex);

        removeModel.remove(elementId)
    }

    function setElementName(elementIndex, name){
        if (elementIndex < 0 || elementIndex >= root.elementsModel.GetItemsCount()){
            console.error();

            return;
        }

        let elementId = root.elementsModel.GetData("Id", elementIndex);

        renameQuery.rename(elementId, name)
    }

    function setElementDescription(elementIndex, description){
        if (elementIndex < 0 || elementIndex >= root.elementsModel.GetItemsCount()){
            console.error();

            return;
        }

        let elementId = root.elementsModel.GetData("Id", elementIndex);

        setDescriptionQuery.setDescription(elementId, description)
    }

    function getAdditionalInputParams(){
        return {};
    }

    GqlModel {
        id: removeModel;

        function remove(id) {
            var query = Gql.GqlRequest("mutation", internal.removeGqlCommand);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", id);

            let additionInputParams = root.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("removedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;
                if (removeModel.ContainsKey("errors")){
                    dataModelLocal = removeModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(internal.removeGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(internal.removeGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (removeModel.ContainsKey("data")){
                    dataModelLocal = removeModel.GetData("data");

                    if (dataModelLocal.ContainsKey(internal.removeGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(internal.removeGqlCommand);

                        if (dataModelLocal.ContainsKey("removedNotification")){
                            dataModelLocal = dataModelLocal.GetData("removedNotification");

                            if (dataModelLocal.ContainsKey("Id")){
                                var itemId = dataModelLocal.GetData("Id");

                                root.removed(itemId);
                            }
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: renameQuery;

        function rename(id, name) {
            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("mutation", internal.renameGqlCommand);

            inputParams.InsertField("Id", id);
            inputParams.InsertField("NewName", name);

            let additionInputParams = root.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            queryFields = Gql.GqlObject("rename");
            queryFields.InsertField("NewName");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, renameQuery);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (renameQuery.ContainsKey("errors")){
                    dataModelLocal = renameQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(internal.renameGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(internal.renameGqlCommand);

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        ModalDialogManager.showWarningDialog(message)
                    }

                    return;
                }

                if (renameQuery.ContainsKey("data")){
                    dataModelLocal = renameQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(internal.renameGqlCommand)) {
                        dataModelLocal = dataModelLocal.GetData(internal.renameGqlCommand);

                        let id = dataModelLocal.GetData("Id");
                        let newName = dataModelLocal.GetData("Name");

                        root.renamed(id, newName);
                    }
                }
            }
        }
    }

    GqlModel {
        id: setDescriptionQuery;

        function setDescription(id, description){
            var query = Gql.GqlRequest("mutation", internal.setDescriptionGqlCommand);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", id);
            inputParams.InsertField("Description", description);

            let additionInputParams = root.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("setDescription");
            queryFields.InsertField("Description");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (setDescriptionQuery.ContainsKey("errors")){
                    dataModelLocal = setDescriptionQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(internal.setDescriptionGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(internal.setDescriptionGqlCommand);

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        ModalDialogManager.showWarningDialog(message)
                    }

                    return;
                }

                if (setDescriptionQuery.ContainsKey("data")){
                    dataModelLocal = setDescriptionQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(internal.setDescriptionGqlCommand)) {

                        dataModelLocal = dataModelLocal.GetData(internal.setDescriptionGqlCommand);

                        var id = dataModelLocal.GetData("Id");
                        var description = dataModelLocal.GetData("Description");

                        root.descriptionSetted(id ,description);
                    }
                }
            }
        }
    }

    GqlModel {
        id: headersModel;

        function getHeaders() {
            var query = Gql.GqlRequest("query", root.collectionId + "Info");

            var inputParams = Gql.GqlObject("input");

            let additionInputParams = root.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryHeaders = Gql.GqlObject("headers");
            queryHeaders.InsertField("Id");
            queryHeaders.InsertField("Name");
            query.AddField(queryHeaders);

            var gqlData = query.GetQuery();

            internal.headersUpdatingBlock = true;
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (this.ContainsKey("errors")){
                    dataModelLocal = this.GetData("errors");

                    if (dataModelLocal.ContainsKey(root.collectionId + "Info")){
                        dataModelLocal = dataModelLocal.GetData(root.collectionId + "Info");
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (this.ContainsKey("data")){
                    dataModelLocal = this.GetData("data");

                    if (dataModelLocal.ContainsKey(root.collectionId + "Info")){
                        dataModelLocal = dataModelLocal.GetData(root.collectionId + "Info");
                        if (!dataModelLocal){
                            return;
                        }

                        if (dataModelLocal.ContainsKey("FilterSearch")){
                            let filterSearchModel = dataModelLocal.GetData("FilterSearch")

                            root.filterableHeadersModel = filterSearchModel;
                        }

                        if(dataModelLocal.ContainsKey("Headers")){
                            dataModelLocal = dataModelLocal.GetData("Headers");

                            root.headersModel = dataModelLocal;

                            internal.headersUpdatingBlock = false;
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: elementsGqlModel;

        function getElements(count, offset, filterModel) {
            var query = Gql.GqlRequest("query", root.collectionId + "List");

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Count", count);
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("FilterModel", filterModel.ToJson());

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);

            let additionInputParams = root.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");

            queryFields.InsertField("Id");
            queryFields.InsertField("Name");

            for(var i = 0; i < root.headersModel.GetItemsCount(); i++){
                let headerId = root.headersModel.GetData("Id", i);
                queryFields.InsertField(headerId);
            }

            for (let i = 0; i < root.additionalFieldIds.length; i++){
                queryFields.InsertField(root.additionalFieldIds[i]);
            }

            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            root.beginUpdate();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("List", this.ToJson());
            if (this.state === "Ready"){
                var dataModelLocal;
                if (this.ContainsKey("errors")){
                    dataModelLocal = this.GetData("errors");

                    if (dataModelLocal.ContainsKey(root.collectionId + "List")){
                        dataModelLocal = dataModelLocal.GetData(root.collectionId + "List");
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (this.ContainsKey("data")){
                    dataModelLocal = this.GetData("data");
                    if (dataModelLocal.ContainsKey(root.collectionId + "List")){
                        dataModelLocal = dataModelLocal.GetData(root.collectionId + "List");
                        if (dataModelLocal.ContainsKey("notification")){
                            root.notificationModel = dataModelLocal.GetData("notification");
                        }

                        if (!dataModelLocal.ContainsKey("items")){
                            dataModelLocal.AddTreeModel("items")
                        }

                        if (dataModelLocal.ContainsKey("items")){
                            root.elementsModel = dataModelLocal.GetData("items");
                        }
                    }
                }
            }
            if (this.state !== "Loading"){
                root.endUpdate();
            }
        }
    }

    GqlModel {
        id: objectViewModel;

        function getObjectView(){
            var query = Gql.GqlRequest("query", root.collectionId + "ObjectView");

            var inputParams = Gql.GqlObject("input");

            let additionInputParams = root.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("objectView");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (this.ContainsKey("errors")){
                    dataModelLocal = this.GetData("errors");

                    if (dataModelLocal.ContainsKey(root.collectionId + "ObjectView")){
                        dataModelLocal = dataModelLocal.GetData(root.collectionId + "ObjectView");
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                dataModelLocal = this.GetData("data");
                if(!dataModelLocal)
                    return;

                if (dataModelLocal.ContainsKey(root.collectionId + "ObjectView")){
                    dataModelLocal = dataModelLocal.GetData(root.collectionId + "ObjectView");

                    root.objectEditorInfoModel = dataModelLocal;
                }
            }
        }
    }
}
