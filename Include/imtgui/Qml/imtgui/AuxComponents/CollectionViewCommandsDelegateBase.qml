import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    /**
        Collection view
    */
    property Item collectionView;

    /**
        The table inside collection view
    */
    property Item tableData;

    /**
        The property for providing a command model
    */
    property Item commandsProvider;

    /**
        The property for tracking changes to the selected item
    */
    property int selectedIndex: container.tableData.selectedIndex;

    property string commandsId;

    /**
        Requests ids for GQL models
    */
    property string gqlModelRemove;
    property string gqlModelRename;
    property string gqlModelSetDescription;
    property string gqlModelItem;
    property string gqlModelMetaInfo;


    //TODO -> onItemSelectionChanged
    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged", container.selectedIndex, container);
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";

        container.commandsProvider.changeCommandMode("Remove", mode);
        container.commandsProvider.changeCommandMode("Edit", mode);
    }

    Component {
        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let itemId = tableData.getSelectedId();
                    removeModel.updateModel(itemId);
                }
            }
        }
    }

    /**
        Basic command click handler
    */
    function commandHandle(commandId){
        console.log("CollectionView commandActivated", commandId);
        if (commandId === "New"){
            container.collectionView.baseCollectionView.selectedItem("", "<new item>");
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected item from the collection ?")});
        }
        else if (commandId === "Edit"){
            let itemId = tableData.getSelectedId();
            let itemName = tableData.getSelectedName();
            container.collectionView.baseCollectionView.selectedItem(itemId, itemName);
        }
    }

    /**
        GQL Request for removing element from the collection by object id
    */
    GqlModel {
        id: removeModel;

        function updateModel(itemId) {
            console.log( "updateModel removeModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(itemId != ""){
                query = Gql.GqlRequest("query", container.gqlModelRemove);
                inputParams.InsertField("Id", itemId);
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
                    if (dataModelLocal.ContainsKey(container.gqlModelRemove)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlModelRemove);
                    }

                    return;
                }

                if (removeModel.ContainsKey("data")){
                    dataModelLocal = removeModel.GetData("data");

                    if (dataModelLocal.ContainsKey(container.gqlModelRemove)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlModelRemove);

                        if (dataModelLocal.ContainsKey("removedNotification")){
                            dataModelLocal = dataModelLocal.GetData("removedNotification");

                            if (dataModelLocal.ContainsKey("Id")){
                                var itemId = dataModelLocal.GetData("Id");

                                container.collectionView.updateGui();
                            }
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: renameQuery;

        function rename(itemId, newName) {
            console.log( "CollectionView renameQuery rename");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", collectionViewBaseContainer.gqlModelRename);
            inputParams.InsertField("Id", itemId);

            inputParams.InsertField("NewName", newName);

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

                    if (dataModelLocal.ContainsKey(collectionViewBaseContainer.gqlModelRename)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewBaseContainer.gqlModelRename);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewBaseContainer.openMessageDialog("Error", message, "ErrorDialog");
                        }
                    }

                    return;
                }

                if (renameQuery.ContainsKey("data")){
                    dataModelLocal = renameQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewBaseContainer.gqlModelRename)) {
                        dataModelLocal = dataModelLocal.GetData(collectionViewBaseContainer.gqlModelRename);

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var newId = dataModelLocal.GetData("NewId");
                            var newName = dataModelLocal.GetData("NewName");

//                            collectionViewBaseContainer.renamedItem(tableInternal.getSelectedId(), newId);

//                            featuresTreeView.loadFeaturesModel();
//                            featuresTreeView.loadDependModel();
//                            treeViewModel.reloadModel();
//                            featureDependenciesModel.reloadModel();
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

            query = Gql.GqlRequest("query", collectionViewBaseContainer.gqlModelSetDescription);
            inputParams.InsertField("Id", tableInternal.getSelectedId());

            inputParams.InsertField("Description", description);

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
                let keys = setDescriptionQuery.GetKeys();
                if (!keys || keys.length == 0){
                    thubnailDecoratorContainer.setInvalidConnection(true);
                    return;
                }

                var dataModelLocal;

                if (setDescriptionQuery.ContainsKey("errors")){
                    dataModelLocal = setDescriptionQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewBaseContainer.gqlModelSetDescription)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewBaseContainer.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewBaseContainer.openMessageDialog("Error", message, "ErrorDialog");
                        }
                    }

                    return;
                }

                if (setDescriptionQuery.ContainsKey("data")){
                    dataModelLocal = setDescriptionQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewBaseContainer.gqlModelSetDescription)) {

                        dataModelLocal = dataModelLocal.GetData(collectionViewBaseContainer.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var id = dataModelLocal.GetData("Id");
                            var description = dataModelLocal.GetData("Description");

                            collectionViewBaseContainer.updateItemAfterSetDescription(description);
                            collectionViewBaseContainer.setDescriptionItem(id, description);
                        }
                    }
                }
            }
        }
    }
}

